#include "animation.hpp"

/**
 * Animation module — elements to plan execute some code at 
 * the different time points
*/

#include <chrono>
#include <map>
#include <memory>

namespace animation {
	
	enum class AnimationResult {
		CONTINUE,
		FINISHED,
		NOT_READY
	};
	
	class AnimationBase {
		public:
			virtual AnimationResult applyTime(timePoint time_point) = 0;
			virtual ~AnimationBase() {};
	};
	
	///class for animations defined by amount of seconds it takes
	class SpanAnimation : public AnimationBase {
		using secDur = std::chrono::duration<double, std::ratio<1>>;
		public:
			SpanAnimation(	timePoint start_time, 
							double durationInSeconds, 
							std::function<void(double)> callback_function) {
				startTime = start_time;
				duration = secDur(durationInSeconds);
				callback = callback_function;
			}
			
			virtual AnimationResult applyTime(timePoint time_point) override {
				if( time_point < startTime ) //is time point before start time?
					return AnimationResult::NOT_READY;
				
				std::chrono::duration<double> actualDuration = time_point - startTime;
				double timeRatio = std::chrono::duration_cast<secDur>(actualDuration).count() / duration.count();
				callback(timeRatio);
				if( timeRatio >= 1.0 ) //animation finished?
					return AnimationResult::FINISHED;
				else
					return AnimationResult::CONTINUE;
			}
		private:
			timePoint startTime;
			secDur duration;
			std::function<void(double)> callback;
	};
	
	//class for animations that should run at a specific time point
	class PointAnimation : public AnimationBase {
		public:
			PointAnimation(timePoint at, std::function<void()> aCallback):
				exeTime(at), callback(aCallback) {};
			
			virtual AnimationResult applyTime(timePoint time_point) override {
				if( time_point < exeTime ) //not yet ready?
					return AnimationResult::NOT_READY;
				//we are ready
				callback();
				return AnimationResult::FINISHED;
			}
		private:
			timePoint exeTime;
			std::function<void()> callback;
	};
	
	
	std::map<long long, std::unique_ptr<AnimationBase>> pendingAnimations;
	
	//functions for adding different types of animations-----------------------------
	
	///an animation with a span form given time
	long long newShiftedSpanAnimation(timePoint start_time, double seconds, std::function<void(double)> callback) {
		SpanAnimation* span = new SpanAnimation(
			start_time,
			seconds,
			callback
		);
		long long ref = reinterpret_cast<long long>(span);
		pendingAnimations.emplace( ref, span);
		return ref;
	}
	///an animation with a span from current time
	long long newSpanAnimation(double seconds, std::function<void(double)> callback) {
		return newShiftedSpanAnimation(std::chrono::steady_clock::now(), seconds, callback);
	}
	
	///an animation executed at a specific time point
	long long newPointAnimation(timePoint time_point, std::function<void()> callback) {
		PointAnimation* pa = new PointAnimation(time_point, callback);
		long long ref = reinterpret_cast<long long>(pa);
		pendingAnimations.emplace( ref, pa );
		return ref;
	}
	
	//executing animations-------------------------------------
	void applyTime(timePoint currentTime) {
		//execute and remove all animations that report a finished state
		
		auto it = pendingAnimations.begin();
		while( it != pendingAnimations.end() ) {
			if( it->second->applyTime(currentTime) == AnimationResult::FINISHED ) {
				it = pendingAnimations.erase( it );
			} else {
				++it;
			}
		}
	}
	
	//removing animations---------------------------
	void remove(long long ref) {
		auto it = pendingAnimations.find(ref);
		if( it != pendingAnimations.end() )
			pendingAnimations.erase( it );
	}
	
	timePoint getNow() {
		return std::chrono::steady_clock::now();
	}
	
	timePoint addSeconds(double seconds, timePoint time) {
		std::chrono::milliseconds secInMs(static_cast<int64_t>(1000 * seconds));
		return time + secInMs;
	}
	
}//end namespace animation


