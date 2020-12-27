/**
 * The SDL2 class representing SDL2 system.
*/

#include "system.hpp"
#include "mls.hpp"
#include "rect.hpp"
#include "errors.hpp"


namespace multimedia {
	
	///ctor initializing the SDL2 environment
	SDL2_System::SDL2_System() {
		//Initialize SDL
    	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
			//some error during initialization
			ShowError( _("Unable to initialize SDL2: ").get() , SDL_GetError() );
			sdlMainInitialized = false;
			return;
		}
		sdlMainInitialized = true;
		
		//Initialize SDL Image
		const int imgFlags = IMG_INIT_PNG;
		if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
			//some error during initialization
			ShowError( _("Unable to initialize SDL2 Image module: ").get() , IMG_GetError() );
			imageInitialized = false;
			return;
		}
		imageInitialized = true;
		
		//Initialize SDL Font
		if( TTF_Init() == -1 ) {
			//some error during initialization
			ShowError( _("Unable to initialize SDL2 Font module: ").get() , TTF_GetError() );
			fontInitialized = false;
			return;
		}
		fontInitialized = true;
		
		//Initialize SDL Sound
		const int soundFlags = MIX_INIT_OGG;
		if( !(Mix_Init(soundFlags) & soundFlags) ) {
			//some error during initialization
			ShowError( _("Unable to initialize SDL2 Mixer module: ").get() , Mix_GetError() );
			soundInitialized = false;
			return;
		}
		//Open audio system
		if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
			//some error
			ShowError( _("Unable to open SDL2 audio system: ").get() , Mix_GetError() );
			soundInitialized = false;
			return;
		}
		soundInitialized = true;
	}
	
	///tells if the system has been initialized
	bool SDL2_System::initialized() {
		return sdlMainInitialized && imageInitialized && fontInitialized && soundInitialized;
	}
	
	///dtor quiting the SDL2 environment
	SDL2_System::~SDL2_System() {
		if( soundInitialized ) {
			Mix_CloseAudio();
			Mix_Quit();
		}
		
		if( fontInitialized ) {
			TTF_Quit();
		}
		
		if( imageInitialized ) {
			IMG_Quit();
		}
		
		if( sdlMainInitialized ) {
			SDL_Quit();
		}		
	}
	
	///
	std::vector<VideoMode> SDL2_System::getSupportedVideoModes() {
		std::vector<VideoMode> resultList;
		if( sdlMainInitialized == false ) throw CriticalSDLError();
		
		int availableModes = SDL_GetNumDisplayModes(0);
		if( availableModes < 1 ) { //error?
			ShowError( _("Unable to get available display modes on 1st monitor: ").get() ,
				SDL_GetError() );
			throw CriticalSDLError();
		}
		
		SDL_DisplayMode mode;
		for(int i = 0; i < availableModes; ++i) {
			if( SDL_GetDisplayMode(0,i,&mode) != 0 ) {//error?
				ShowError( _("Unable to get information about %{#display#}%mode number %{num}%: ")
				.apply("num",i+1).get() , SDL_GetError() );
				//throw CriticalSDLError();
				continue;
			}
			//filter out too small modes
			if( mode.w < 800 || mode.h < 600 ) continue;
			
			VideoMode newMode;
			newMode.width = mode.w;
			newMode.height = mode.h;
			newMode.bitsPerPixel = SDL_BITSPERPIXEL(mode.format);
			newMode.refreshRate = mode.refresh_rate;
			newMode.source = mode;
			
			resultList.push_back(newMode);
		}
		
		return resultList;
	}
	
	///returns default window size for cases when the game couldn't load configuration
	VideoMode SDL2_System::getDefaultWindowSize() {
		SDL_DisplayMode dm;
		if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {//error?
			ShowError( _("Unable to get information about desktop's display mode: ").get() ,
				SDL_GetError() );
			throw CriticalSDLError();
		}
		VideoMode new_mode;
		Rect r_mode{0,0,dm.w,dm.h};
		//scale down the size by 300px of width, but keep the screen ratio
		r_mode.setWidthWithConstRatio( r_mode.getWidth() - 300 );
		new_mode.width = r_mode.getWidth();
		new_mode.height = r_mode.getHeight();
		new_mode.bitsPerPixel = SDL_BITSPERPIXEL(dm.format);
		new_mode.refreshRate = dm.refresh_rate;
		
		return new_mode;
	}
	
}//end namespace multimedia

 