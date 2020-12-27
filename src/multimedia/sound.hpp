#pragma once

#ifndef MULTIMEDIA_SOUND_HPP
#define MULTIMEDIA_SOUND_HPP

#include <string>
#include <SDL_mixer.h>

namespace multimedia {
	
	class Music {
		public:
			Music(std::string filename);
			~Music();
			
			///
			void play();
			///
			void stop(int fadeOutInMiliseconds = -1);
			///
			bool isPlaying() const;
		private:
			Mix_Music *musicPtr;
	};//end class Music
	
	class SoundSample {
		public:
			SoundSample(std::string filename);
			~SoundSample();
			
			///
			void play();
			///
			void stop();
			///
			bool isPlaying() const;
		private:
			Mix_Chunk *soundPtr;
			int channel;
	};//end class SoundSample
	
}//end namespace multimedia

#endif /* MULTIMEDIA_SOUND_HPP */

