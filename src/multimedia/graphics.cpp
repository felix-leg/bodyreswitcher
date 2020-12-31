/**
 * Window class. Manages SDL display windows and game loop.
*/

#include "errors.hpp"
#include "graphics.hpp"
#include "mls.hpp"
#include "filesystem.hpp"
#include "SDL2_framerate.h"
#include "SDL2_gfxPrimitives.h"

namespace multimedia {
	
	/*
	========================= DirtyFlag class =======================
	*/
	
	DirtyFlag::DirtyFlag(): flag(true) {}
	
	///returns true if the flag is dirty
	bool DirtyFlag::needsRedraw() {
		if( flag ) {
			flag = false;
			return true;
		} else {
			return false;
		}
	}
	
	void DirtyFlag::invalidate() {
		flag = true;
	}
	
	/*
	========================= Window class =======================
	*/
	
	Window::Window(std::string title, VideoMode video_mode, bool fullscreen, SDL2_System *system):
	windowTitle(title),
	sdlSystem(system) {
		//set the window
		uint32_t createFlags = 0;
		fullscreenState = false;
		if(fullscreen) {
			createFlags |= SDL_WINDOW_FULLSCREEN;
			fullscreenState = true;
		}
		windowHandle = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED, //X position
			SDL_WINDOWPOS_CENTERED, //Y position
			video_mode.width,
			video_mode.height,
			createFlags
			);
		if( windowHandle == nullptr) { //error?
			ShowError( _("Unable to create a window: ").get() , SDL_GetError() );
			throw CriticalSDLError();
		}
		
		//get the surface
		windowSurface = SDL_GetWindowSurface( windowHandle );
		if( windowSurface == nullptr ) { //error?
			ShowError( _("Unable to get the surface from the window: ").get() , SDL_GetError() );
			SDL_DestroyWindow( windowHandle );
			throw CriticalSDLError();
		}
		
		//get the window renderer
		windowRenderer = SDL_CreateSoftwareRenderer( windowSurface );
		//SDL_CreateRenderer( windowHandle, -1, SDL_RENDERER_ACCELERATED );
		if( windowRenderer == nullptr ) {//error?
			ShowError( _("Unable to create window renderer: ").get() , SDL_GetError() );
			SDL_DestroyWindow( windowHandle );
			throw CriticalSDLError();
		}
		
	}
	
	Window::~Window() {
		if( windowHandle != nullptr ) SDL_DestroyWindow( windowHandle );
		for(auto flag : flags) {
			delete flag;
		}
	}
	
	DirtyFlag* Window::createDirtyFlag() {
		DirtyFlag* flag = new DirtyFlag();
		flags.push_back( flag );
		return flag;
	}
	
	void Window::invalidateFlags() {
		for(auto flag : flags) {
			flag->invalidate();
		}
	}
	
	void Window::registerUpdateFunction(std::function<void(int,int,bool)> callback) {
		updateCallback = callback;
	}
	
	void Window::registerDrawFunction(std::function<void()> callback) {
		drawCallback = callback;
	}
	
	bool Window::isFullscreen() const {
		return fullscreenState;
	}
	
	VideoMode Window::getDisplayMode() const {
		VideoMode mode;
		mode.width = 0;
		mode.height = 0;
		mode.bitsPerPixel = 0;
		mode.refreshRate = 0;
		if( fullscreenState ) {
			//is fullscreen
			if( SDL_GetCurrentDisplayMode(0, &mode.source) != 0 ) {
				//error on geting display mode
				ShowError( _("Unable to get current display mode: ").get() , SDL_GetError() );
				return mode;
			}
			
			mode.width = mode.source.w;
			mode.height = mode.source.h;
			mode.bitsPerPixel = SDL_BITSPERPIXEL(mode.source.format);
			mode.refreshRate = mode.source.refresh_rate;
		} else {
			//is windowed
			int w,h;
			SDL_GetWindowSize( windowHandle, &w, &h);
			SDL_GetDesktopDisplayMode(0, &mode.source);
			
			mode.width = w;
			mode.height = h;
			mode.bitsPerPixel = SDL_BITSPERPIXEL(mode.source.format);
			mode.refreshRate = mode.source.refresh_rate;
		}
		
		return mode;
	}
	
	void Window::switchFullscreen() {
		int flagsToSet;
		if( fullscreenState ) {
			//off
			flagsToSet = 0;
		} else {
			//on
			//SDL_WINDOW_FULLSCREEN_DESKTOP
			flagsToSet = SDL_WINDOW_FULLSCREEN;
		}
		
		if( SDL_SetWindowFullscreen(windowHandle, flagsToSet) != 0 ) {
			ShowError( _("Can't change the fullscreen state: ").get() , SDL_GetError() );
			return;
		}
		fullscreenState = !fullscreenState;
		//Let the event SDL_WINDOWEVENT_SIZE_CHANGED do this in the main loop ⇓
		//redrawFlag += 1;
		//windowSurface = SDL_GetWindowSurface( windowHandle );
	}
	
	void Window::switchToVideoMode(VideoMode mode) {
		if( fullscreenState ) {
			//if is fullscreen
			if( SDL_SetWindowDisplayMode( windowHandle , &mode.source ) != 0 ) {
				//error while changing mode
				ShowError( _("Unable to change into display mode '%{mode_text}%': ")
					.apply("mode_text", mode.to_string()).get() , SDL_GetError() );
			}
			invalidateFlags();//SDL seems not to fire up the window change size event
		} else {
			//if is windowed
			SDL_SetWindowSize( windowHandle , mode.width , mode.height );
		}
	}
	
	///the main game loop
	void Window::mainLoop() {
		try{
			SDL_Event e;//for storing events from the system
			const Uint32 blackFilling = Color(0,0,0).to_rgb(windowSurface->format);
			int mouseX, mouseY;
			bool mouseMoved = true;
			bool mouseClicked = false;
			FPSmanager fps;//for slowing down animation to a set FPS
			
			SDL_initFramerate(&fps);//init FPS manager to default 30Hz framerate
			
			SDL_GetMouseState(&mouseX, &mouseY);
			
			#ifdef CMAKE_DEBUG
			Surface mousePointer = Surface::newRGB_Surface(10,10);
			mousePointer.fill(Color(255,0,255));
			#endif
			
			while( true ) {
				//gather events from the system
				while( SDL_PollEvent(&e) != 0 ) {
					//User request quit
					if( e.type == SDL_QUIT )
						throw SignaledGameEnd();
					
					//User request quit by pressing ESC key
					if( e.type == SDL_KEYUP ) {
						if( e.key.keysym.sym == SDLK_ESCAPE )
							throw SignaledGameEnd();
						if( e.key.keysym.sym == SDLK_f )
							switchFullscreen();
					}
					
					//User moved a mouse
					if( e.type == SDL_MOUSEMOTION ) {
						mouseX = e.motion.x;
						mouseY = e.motion.y;
						//announce mouse motion
						mouseMoved = true;
						continue;
					}
					
					//User clicked a mouse button
					if( e.type == SDL_MOUSEBUTTONUP ) {
						mouseClicked = true;
					}
					
					//Window changed size
					if( e.type == SDL_WINDOWEVENT ) {
						if( e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ) {
							windowSurface = SDL_GetWindowSurface( windowHandle );
							windowRenderer = SDL_CreateSoftwareRenderer( windowSurface );
							invalidateFlags();
							SDL_GetMouseState(&mouseX, &mouseY);
							mouseMoved = true;
						}
					}
				}
				//:::update
				if( updateCallback != nullptr ) {
					if( mouseMoved )
						updateCallback(mouseX, mouseY, mouseClicked);
					else
						updateCallback(-1, -1, mouseClicked);
				}
				mouseMoved = false;
				mouseClicked = false;
				//:::draw 
				//fill the screen with black
				SDL_FillRect( windowSurface, nullptr, blackFilling );
				//call draw callback
				if( drawCallback != nullptr ) drawCallback();
				#ifdef CMAKE_DEBUG
				Rect r = mousePointer.getSize();
				r.setCenterX(mouseX);
				r.setCenterY(mouseY);
				SDL_BlitSurface(mousePointer.surface, nullptr, windowSurface, r.sdl_rect());
				#endif
				//switch buffers
				SDL_UpdateWindowSurface( windowHandle );
				//slow down to a constant framerate
				SDL_framerateDelay(&fps);
			}			
		}
		catch( SignaledGameEnd ) {
			return;
		}
	}
	
	Surface Window::getWindowSurface() {
		return Surface(windowSurface, true);
	}
	
	Drawer Window::getWindowDrawer() {
		return Drawer(windowRenderer);
	}
	
	int Window::getWidth() const {
		if( fullscreenState ) {
			//is fullscreen
			SDL_DisplayMode dm;
			SDL_GetCurrentDisplayMode( 0 , &dm );
			return dm.w;
		} else {
			//is window
			int w,h;
			SDL_GetWindowSize( windowHandle, &w, &h);
			return w;
		}
	}
	int Window::getHeight() const {
		if( fullscreenState ) {
			//is fullscreen
			SDL_DisplayMode dm;
			SDL_GetCurrentDisplayMode( 0 , &dm );
			return dm.h;
		} else {
			//is window
			int w,h;
			SDL_GetWindowSize( windowHandle, &w, &h);
			return h;
		}
	}
	
	/*
	========================= Surface class =======================
	*/

	//private ctor
	Surface::Surface(SDL_Surface* new_surface, bool isWindow):
		surface(new_surface), isWindowSurface(isWindow) {}
	
	//empty public ctor
	Surface::Surface(SDL_Surface* extSurface):surface(extSurface), isWindowSurface(false) {}
	
	//public dtor
	Surface::~Surface() {
		if( isWindowSurface ) return;//don't remove window surface
		
		if( surface != nullptr ) SDL_FreeSurface( surface );
	}
	//*
	//move operator
	Surface& Surface::operator=(Surface&& other) {
		if( this == &other ) return *this;
		if( isWindowSurface || other.isWindowSurface ) {
			ShowError( _("Tried to move window surface").get() );
			throw CriticalSDLError();
		}
		
		if( surface != nullptr ) SDL_FreeSurface( surface );
		surface = other.surface;
		other.surface = nullptr;
		
		return *this;
	}//*
	
	Surface Surface::fromFile(std::string filename) {
		filename = filesystem::getDataPath(filename);
		if( !filesystem::pathExists(filename) ) {
			ShowError( _("Can't load file '%{filename}%': file doesn't exist")
				.apply("filename", filename).get() );
			throw CriticalSDLError();
		}
		
		SDL_Surface* loaded = IMG_Load(filename.c_str());
		if( loaded == nullptr ) {
			ShowError( _("Can't load file '%{filename}%':%{#SDL Image Error#}% ")
				.apply("filename",filename).get() , IMG_GetError() );
			throw CriticalSDLError();
		}
		
		return Surface(loaded, false);
	}
	
	//defines default masks for newRGB[A]_Surface(...) functions
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		#define DEFAULT_R_MASK 0xff000000
		#define DEFAULT_G_MASK 0x00ff0000
		#define DEFAULT_B_MASK 0x0000ff00
		#define DEFAULT_A_MASK 0x000000ff
	#else
		#define DEFAULT_R_MASK 0x000000ff
		#define DEFAULT_G_MASK 0x0000ff00
		#define DEFAULT_B_MASK 0x00ff0000
		#define DEFAULT_A_MASK 0xff000000
	#endif
	
	Surface::Surface(const Surface& other):isWindowSurface(false) {
		Rect size = other.getSize();
		Uint32 Amask;
		if( other.hasAlphaChannel() ) Amask = DEFAULT_A_MASK;
		else Amask = 0;
		surface = SDL_CreateRGBSurface(0, size.getWidth(), size.getHeight(), 32,
			DEFAULT_R_MASK, DEFAULT_G_MASK, DEFAULT_B_MASK, Amask
		);
		SDL_BlitSurface( other.surface, nullptr, surface, nullptr);
	}
	
	Surface& Surface::operator=(const Surface& other) {
		if( this == &other ) return *this;
		
		if( isWindowSurface ) {
			ShowError(_("Tried to overwrite window surface").get());
			throw CriticalSDLError();
		}
		
		SDL_FreeSurface( surface );
		
		Rect size = other.getSize();
		Uint32 Amask;
		if( other.hasAlphaChannel() ) Amask = DEFAULT_A_MASK;
		else Amask = 0;
		surface = SDL_CreateRGBSurface(0, size.getWidth(), size.getHeight(), 32,
			DEFAULT_R_MASK, DEFAULT_G_MASK, DEFAULT_B_MASK, Amask
		);
		SDL_BlitSurface( other.surface, nullptr, surface, nullptr);
		
		if( other.hasAlphaChannel() )
			SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
		
		return *this;
	}
	
	Surface Surface::newRGB_Surface(unsigned int width, unsigned int height) {
		SDL_Surface* newSurface = SDL_CreateRGBSurface(0, width, height, 32,
			DEFAULT_R_MASK, DEFAULT_G_MASK, DEFAULT_B_MASK, 0/*= no alpha */
		);
		
		if( newSurface == nullptr ) {//error?
			ShowError( _("Unable to create a RGB Surface: ").get() , SDL_GetError() );
			throw CriticalSDLError();
		}
		
		return Surface(newSurface, false);
	}
	
	Surface Surface::newRGBA_Surface(unsigned int width, unsigned int height) {
		SDL_Surface* newSurface = SDL_CreateRGBSurface(0, width, height, 32,
			DEFAULT_R_MASK, DEFAULT_G_MASK, DEFAULT_B_MASK, DEFAULT_A_MASK
		);
		
		if( newSurface == nullptr ) {//error?
			ShowError( _("Unable to create a RGBA Surface: ").get() , SDL_GetError() );
			throw CriticalSDLError();
		}
		
		return Surface(newSurface, false);
	}
	
	///converts this surface to a pixelformat suitable to draw on other surface
	bool Surface::optimizeForDrawingOn(Surface otherSurface) {
		if( isWindowSurface ) return false;//don't optimize window surface
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		
		SDL_Surface* optimizedSurface = SDL_ConvertSurface(surface, otherSurface.surface->format, 0);
		
		if( optimizedSurface == nullptr ) {//error?
			ShowError( _("Can't optimize surface: ").get() , SDL_GetError() );
			return false;
		}
		
		SDL_FreeSurface( surface );
		surface = optimizedSurface;
		return true;
	}
	
	Rect Surface::getSize() const {
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		return Rect{0,0, surface->w, surface->h};
	}
	
	bool Surface::hasAlphaChannel() const {
		if( surface == nullptr ) return false;
		return surface->format->Amask != 0;
	}
	
	Drawer Surface::getDrawer() {
		return Drawer( SDL_CreateSoftwareRenderer(surface) );
	}
	
	void Surface::blit(Rect topLeft, const Surface& otherSurface) {
		blit(topLeft.getLeft(), topLeft.getTop(), otherSurface);
	}
	
	///draws other surface on this surface, the fastest way
	void Surface::blit(int x, int y, const Surface& otherSurface) {
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		if( otherSurface.surface == nullptr ) return;//don't draw empty surface
		
		SDL_Rect dest = {x,y,0,0};//width and height are ignored
		
		SDL_BlitSurface( otherSurface.surface, nullptr, surface, &dest);
	}
	
	void Surface::blit(int x, int y, Rect sourceRect, const Surface& otherSurface) {
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		if( otherSurface.surface == nullptr ) return;//don't draw empty surface
		
		SDL_Rect dest = {x,y,0,0};
		
		SDL_BlitSurface( otherSurface.surface, sourceRect.sdl_rect(), surface, &dest);
	}
	
	void Surface::fill(Color color) {
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		SDL_FillRect( surface, nullptr, color.to_rgba(surface->format));
	}
	
	void Surface::crop(Rect cropRect) {
		if( isWindowSurface ) return;//don't crop window surface
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		
		bool hasAlpha = (surface->format->Amask != 0);
		
		//create new surface with cropped size and the same pixel format
		SDL_Surface* newSurface = SDL_CreateRGBSurface(0,
			cropRect.getWidth(), cropRect.getHeight(),
			surface->format->BitsPerPixel,
			surface->format->Rmask,
			surface->format->Gmask,
			surface->format->Bmask,
			surface->format->Amask
		);
		
		if( newSurface == nullptr ) {
			ShowError( _("Unable to create a RGBA Surface: ").get() , SDL_GetError() );
			return;
		}
		
		//draw on the new surface
		SDL_Rect dest = {0,0,0,0};
		SDL_BlitSurface( surface, cropRect.sdl_rect(), newSurface, &dest);
		
		//switch to the new surface
		SDL_FreeSurface( surface );
		surface = newSurface;
		
		if( hasAlpha ) {
			SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
		}
	}
	
	void Surface::scale(double factor) {
		scale(factor, factor);
	}
	void Surface::scale(double x_factor, double y_factor ) {
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		
		Rect newRect = getSize();
		newRect.setWidth(static_cast<int>(newRect.getWidth() * x_factor));
		newRect.setHeight(static_cast<int>(newRect.getHeight() * y_factor));
		
		scale(newRect);
	}
	void Surface::scale(Rect destRect) {
		if( isWindowSurface ) return; //don't scale window surface
		if( surface == nullptr ) throw CriticalSDLError();//need some surface
		
		bool hasAlpha = (surface->format->Amask != 0);
		
		//create new surface with destination size and the same pixel format
		SDL_Surface* newSurface = SDL_CreateRGBSurface(0,
			destRect.getWidth(), destRect.getHeight(),
			32,//must be or will not work: https://stackoverflow.com/questions/40850196/sdl2-resize-a-surface
			surface->format->Rmask,
			surface->format->Gmask,
			surface->format->Bmask,
			surface->format->Amask
		);
		
		if( newSurface == nullptr ) {
			ShowError( _("Unable to create a RGBA Surface: ").get() , SDL_GetError() );
			return;
		}
		
		SDL_BlitScaled( surface, nullptr, newSurface, nullptr);
		
		SDL_FreeSurface( surface );
		surface = newSurface;
		
		if( hasAlpha ) {
			SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
		}
	}
	
	/*
	========================= Drawer class =======================
	*/

	Drawer::Drawer(SDL_Renderer *rend):
		renderer(rend), rendColor(0,0,0,255) {}
	
	Drawer::~Drawer() {
		if( renderer != nullptr ) SDL_DestroyRenderer( renderer );
	}
	
	void Drawer::setColor(Color color) {
		rendColor = color;
	}
	
	void Drawer::drawRectangle(Rect rect, bool filled) {
		drawRectangle(rect.getLeft(), rect.getTop(), rect.getRight(), rect.getBottom(), filled);
	}
	
	void Drawer::drawRectangle(int x1, int y1, int x2, int y2, bool filled) {
		if( renderer == nullptr ) return;
		
		int result;
		if( filled )
			result = boxRGBA(renderer, x1, y1, x2, y2, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		else
			result = rectangleRGBA(renderer, x1, y1, x2, y2, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		
		if( result != 0 ) {
			ShowError( _("Unable to draw with renderer: ").get() , SDL_GetError() );
		}
	}
	
	void Drawer::drawCircle(Rect bound, bool filled) {
		int x = bound.getCenterX();
		int y = bound.getCenterY();
		int rad = 0;
		if( bound.getWidth() >= bound.getHeight() )
			rad = bound.getHeight();
		else
			rad = bound.getWidth();
		rad /= 2;
		drawCircle(x, y, rad, filled);
	}
	
	void Drawer::drawCircle(int x, int y, int rad, bool filled) {
		if( renderer == nullptr ) return;
		
		int result;
		if( filled )
			result = filledCircleRGBA(renderer, x, y, rad, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		else
			result = aacircleRGBA(renderer, x, y, rad, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		
		if( result != 0 ) {
			ShowError( _("Unable to draw with renderer: ").get() , SDL_GetError() );
		}
	}
	
	void Drawer::drawEllipse(Rect bound, bool filled) {
		int x = bound.getCenterX();
		int y = bound.getCenterY();
		int rx = bound.getWidth() / 2;
		int ry = bound.getHeight() / 2;
		
		drawEllipse(x, y, rx, ry, filled);
	}
	
	void Drawer::drawEllipse(int x, int y, int radX, int radY, bool filled) {
		if( renderer == nullptr ) return;
		
		int result;
		if( filled )
			result = filledEllipseRGBA(renderer, x, y, radX, radY, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		else
			result = aaellipseRGBA(renderer, x, y, radX, radY, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		
		if( result != 0 ) {
			ShowError( _("Unable to draw with renderer: ").get() , SDL_GetError() );
		}
	}
	
	void Drawer::drawLine(int x1, int y1, int x2, int y2, unsigned char width) {
		if( renderer == nullptr ) return;
		
		int result;
		if( width == 0 ) {//draw simple line
			if( x1 == x2 ) //draw a vertical line
				result = vlineRGBA(renderer, x1, y1, y2, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
			else if( y1 == y2 )//draw a horizontal line
				result = hlineRGBA(renderer, x1, x2, y1, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
			else //draw an AA line
				result = aalineRGBA(renderer, x1, y1, x2, y2, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		} else {//draw a thick line
			result = thickLineRGBA(renderer, x1, y1, x2, y2, width, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		}
		
		if( result != 0 ) {
			ShowError( _("Unable to draw with renderer: ").get() , SDL_GetError() );
		}
	}
	
	void Drawer::drawPolygon(const std::vector<std::pair<int,int>> &points, bool filled) {
		if( renderer == nullptr ) return;
		
		int result;
		const int polySize = points.size();
		Sint16 *x = new Sint16[polySize];
		Sint16 *y = new Sint16[polySize];
		
		//copy x-es and y-es
		int pos = 0;
		for(const auto& point : points) {
			x[pos] = point.first;
			y[pos] = point.second;
			++pos;
		}
		
		//draw polygon
		if( filled )
			result = filledPolygonRGBA(renderer, x, y, polySize, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		else
			result = aapolygonRGBA(renderer, x, y, polySize, rendColor.r, rendColor.g, rendColor.b, rendColor.a);
		
		//delete temps
		delete[] x;
		delete[] y;
		
		if( result != 0 ) {
			ShowError( _("Unable to draw with renderer: ").get() , SDL_GetError() );
		}
	}
	
	void Drawer::drawCLines(const std::vector<std::pair<int,int>> &points, unsigned char width) {
		if( renderer == nullptr ) return;
		
		if( points.size() < 2 ) return;//needs at least 2 points to draw lines
		
		auto fromIt = points.begin();
		auto toIt = points.begin(); ++toIt;
		while( toIt != points.end() ) {
			drawLine(fromIt->first, fromIt->second, toIt->first, toIt->second, width );
			++toIt;
			++fromIt;
		}
	}
	
}//end namespace multimedia
