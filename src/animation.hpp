#pragma once

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <functional>
#include <chrono>

namespace animation {
	using timePoint = decltype(std::chrono::steady_clock::now());
	
	long long newSpanAnimation(double seconds, std::function<void(double)> callback);
	long long newShiftedSpanAnimation(timePoint start_time, double seconds, std::function<void(double)> callback);
	long long newPointAnimation(timePoint time_point, std::function<void()> callback);
	
	void applyTime(timePoint currentTime);
	void remove(long long ref);
	
	timePoint getNow();
	timePoint addSeconds(double seconds, timePoint time);
	
}//end namespace animation

#endif /* ANIMATION_HPP */
