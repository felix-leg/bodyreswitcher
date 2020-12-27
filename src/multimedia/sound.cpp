/**
 * Module for playing SDL2 Sounds and Musics.
*/

#include "errors.hpp"
#include "base.hpp"
#include "sound.hpp"
#include "filesystem.hpp"
#include "mls.hpp"


namespace multimedia {
	
	/*
	======================== Music class
	*/

	Music::Music(std::string filename) {
		filename = filesystem::getDataPath(filename);
		if( !filesystem::pathExists(filename) ) {
			ShowError( _("Unable to load music file '%{filename}%': file doesn't exists")
				.apply("filename",filename).get() );
			throw CriticalSDLError();
		}
		
		musicPtr = Mix_LoadMUS(filename.c_str());
		if( musicPtr == nullptr ) {
			ShowError( _("Unable to load music file '%{filename}%': ")
				.apply("filename",filename).get() , Mix_GetError() );
			throw CriticalSDLError();
		}
	}
	
	Music::~Music() {
		Mix_FreeMusic( musicPtr );
	}
	
	///plays the music
	void Music::play() {
		if( Mix_PlayMusic( musicPtr, -1/* loop forever */) < 0 ) {//error?
			ShowError( _("Can't play the music: ").get() , Mix_GetError() );
			return;
		}
		Mix_VolumeMusic(MIX_MAX_VOLUME / 2);//set music volume to half
	}
	
	///stops the music
	void Music::stop(int fadeOutInMiliseconds) {
		if( Mix_PlayingMusic() == 0 ) return;//music is not playing
		
		if( fadeOutInMiliseconds >= 0 ) {
			Mix_FadeOutMusic( fadeOutInMiliseconds );
		} else {
			Mix_HaltMusic();
		}
	}
	
	///checks the play state
	bool Music::isPlaying() const {
		return Mix_PlayingMusic() == 1;
	}
	
	/*
	======================== SoundSample class
	*/

	SoundSample::SoundSample(std::string filename) {
		filename = filesystem::getDataPath(filename);
		if( !filesystem::pathExists(filename) ) {
			ShowError( _("Unable to load sound file '%{filename}%': file doesn't exists")
				.apply("filename",filename).get() );
			throw CriticalSDLError();
		}
		
		soundPtr = Mix_LoadWAV(filename.c_str());
		if( soundPtr == nullptr ) {
			ShowError( _("Unable to load sound file '%{filename}%': ")
			.apply("filename",filename).get() , Mix_GetError() );
			throw CriticalSDLError();
		}
		
		channel = -1;
	}
	
	SoundSample::~SoundSample() {
		if( isPlaying() ) stop();
		Mix_FreeChunk( soundPtr );
	}
	
	void SoundSample::play() {
		if( (channel = Mix_PlayChannel(-1, soundPtr, 0)) < 0 ) {
			//sound can't be played
			ShowError( _("Unable to play sound: ").get() , Mix_GetError() );
			channel = -1;
		}
	}
	
	void SoundSample::stop() {
		if( channel != -1 ) {
			Mix_HaltChannel( channel );
			channel = -1;
		}
	}
	
	bool SoundSample::isPlaying() const {
		if( channel != -1 ) {
			return Mix_Playing( channel ) != 0;
		} else {
			return false;
		}
	}
	
}//end namespace multimedia

