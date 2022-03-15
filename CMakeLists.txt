cmake_minimum_required(VERSION 3.10.3)

#use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BodyReSwitcher VERSION 2.0)
set(BRS_Version "2.0")
option(USE_CONSOLE_OUTPUT "Print error messages to console instead of showing a message window" OFF)

if(MSVC)
	if(VCPKG_HOME)
		list(APPEND CMAKE_PREFIX_PATH "${VCPKG_HOME}/installed/x64-windows/share")
		include( "${VCPKG_HOME}/scripts/buildsystems/vcpkg.cmake" )
	else()
		message(WARNING "Under MSVC you may need VCPKG. If you do so, run CMake with -DVCPKG_HOME=<your vcpkg home directory>")
	endif()
endif()

#add SDL2 libraries searching modules
if(NOT MSVC)
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
	#find the SDL2 modules required by the game
	find_package(SDL2 REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	find_package(SDL2_image REQUIRED)
	find_package(SDL2_gfx REQUIRED)
	find_package(SDL2_ttf REQUIRED)
else()
	#find the SDL2 modules required by the game
	find_package(SDL2 REQUIRED)
	find_package(SDL2-mixer REQUIRED)
	find_package(SDL2-image REQUIRED)
	find_package(SDL2-ttf REQUIRED)
	find_package(SDL2-gfx REQUIRED)
endif()

#the source list
set(BRS_sources src/main.cpp
				src/filesystem.cpp
				src/filesystem.hpp
				src/animation.cpp
				src/animation.hpp
				src/rect.cpp
				src/rect.hpp
				src/ui.cpp
				src/ui.hpp
				src/game_config.cpp
				src/game_config.hpp
				src/errors.cpp
				src/errors.hpp
				src/mls.cpp
				src/mls.hpp
				src/multimedia/system.cpp
				src/multimedia/system.hpp
				src/multimedia/base.cpp
				src/multimedia/base.hpp
				src/multimedia/graphics.cpp
				src/multimedia/graphics.hpp
				src/multimedia/font.cpp
				src/multimedia/font.hpp
				src/multimedia/sound.cpp
				src/multimedia/sound.hpp
				src/game_math/vector.cpp
				src/game_math/vector.hpp
				src/game_math/graph.hpp
				src/logic/common.cpp
				src/logic/common.hpp
				src/logic/module.cpp
				src/logic/module.hpp
				src/logic/buttons.cpp
				src/logic/buttons.hpp
				src/logic/main_menu_background.cpp
				src/logic/main_menu_background.hpp
				src/logic/main_menu.cpp
				src/logic/main_menu.hpp
				src/logic/about_menu.cpp
				src/logic/about_menu.hpp
				src/logic/options_menu.cpp
				src/logic/options_menu.hpp
				src/logic/screen_size_menu.cpp
				src/logic/screen_size_menu.hpp
				src/logic/new_game_menu.cpp
				src/logic/new_game_menu.hpp
				src/logic/game_panel.cpp
				src/logic/game_panel.hpp
				src/logic/person_graph.cpp
				src/logic/person_graph.hpp
)

add_executable(bodyreswitcher ${BRS_sources})
#link with found SDL2 libraries
if(NOT MSVC)
	target_link_libraries(bodyreswitcher SDL2::Main SDL2::Mixer SDL2::Image SDL2::TTF SDL2::GFX)
else()
	target_link_libraries(bodyreswitcher SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_mixer SDL2::SDL2_image SDL2::SDL2_ttf SDL2::SDL2_gfx)
endif()

target_compile_definitions(bodyreswitcher PRIVATE "$<$<CONFIG:DEBUG>:CMAKE_DEBUG>")
if(USE_CONSOLE_OUTPUT)
	target_compile_definitions(bodyreswitcher PRIVATE "USE_CONSOLE_OUTPUT")
endif()

target_include_directories(bodyreswitcher PUBLIC "${CMAKE_SOURCE_DIR}/src")
target_include_directories(bodyreswitcher PUBLIC "${CMAKE_SOURCE_DIR}/vendor")

#tests-----------

#required for test cases
find_package(Boost 1.56 COMPONENTS unit_test_framework)
#generate Boost tests
if( Boost_FOUND )
	message("BOOST Unit Test found")
	enable_testing()
	add_subdirectory( tests )
endif()

#Folder settings ------------------
include(GNUInstallDirs)

if(WIN32)
	set(PROJECT_INSTALL_ROOT ".")
	set(PROJECT_EXE_ROOT ".")
else()
	set(PROJECT_INSTALL_ROOT "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/bodyreswitcher")
	set(PROJECT_EXE_ROOT "${PROJECT_INSTALL_ROOT}")
endif()

#Locale (used by GetText) library -----------
include(FindIntl)
find_package(Intl REQUIRED)

target_include_directories(bodyreswitcher PUBLIC "${Intl_INCLUDE_DIRS}")
if(MSVC)
	list(APPEND Intl_LIBRARIES "${VCPKG_HOME}installed/x64-windows/lib/intl.lib;${VCPKG_HOME}installed/x64-windows/lib/iconv.lib") #MSVC seems to forget about these libraries 
endif()
target_link_libraries(bodyreswitcher "${Intl_LIBRARIES}")

#GetText library ----------------------------
include(FindGettext)
find_package(Gettext)

if(GETTEXT_FOUND)
	set(SourcesPrefixed "")
	set(po_files "")
	set(BRS_LANGS "pl_PL")
	
	foreach(source ${BRS_sources})
		list(APPEND SourcesPrefixed "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
	endforeach()
	
	foreach(lang ${BRS_LANGS})
		list(APPEND po_files "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}/bodyreswitcher.po")
		add_custom_command(
			OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}/bodyreswitcher.po"
			COMMAND "${GETTEXT_MSGMERGE_EXECUTABLE}" -U "--lang=${lang}" "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}/bodyreswitcher.po" "${CMAKE_CURRENT_SOURCE_DIR}/po/bodyreswitcher.pot"
			DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/po/bodyreswitcher.pot"
		)
		#generates `pofiles` make command
		gettext_process_po_files( "${lang}"
			INSTALL_DESTINATION "${PROJECT_INSTALL_ROOT}/locale"
			PO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/po/${lang}/bodyreswitcher.po"
		)
	endforeach()
	add_custom_target( generate_po DEPENDS ${po_files} )
	
	#--join-existing 
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/po/bodyreswitcher.pot"
		COMMAND xgettext --keyword=_ --language=C++ --add-comments --package-name=bodyreswitcher --package-version=${BRS_Version} --from-code=UTF-8 -o "${CMAKE_CURRENT_SOURCE_DIR}/po/bodyreswitcher.pot" ${SourcesPrefixed}
		DEPENDS ${BRS_sources}
	)
	add_custom_target( generate_pot DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/po/bodyreswitcher.pot" )
	
endif()

#installation --------------------------

#data dir
install( DIRECTORY "data"
DESTINATION "${PROJECT_INSTALL_ROOT}"
CONFIGURATIONS Release
)

#locale dir
install( DIRECTORY "locale"
DESTINATION "${PROJECT_INSTALL_ROOT}"
CONFIGURATIONS Release
)

#the game's main program file
install( TARGETS bodyreswitcher RUNTIME
DESTINATION "${PROJECT_EXE_ROOT}"
CONFIGURATIONS Release
)

if(NOT WIN32)
#make link in the bin folder with the destination to the main program
set(DOLAR "$")

install(CODE "execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory \
\"${DOLAR}ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin\")")

install(CODE "execute_process( \
COMMAND ${CMAKE_COMMAND} -E create_symlink \
\"${PROJECT_INSTALL_ROOT}/bodyreswitcher\" \
\"${DOLAR}ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/bodyreswitcher\" \
)")

#menu entry on Debian-like systems
install(FILES "${CMAKE_SOURCE_DIR}/deb/bodyreswitcher.desktop"
		DESTINATION "/usr/share/applications"
		CONFIGURATIONS Release
		COMPONENT debfiles
		EXCLUDE_FROM_ALL
		)
install(FILES "${CMAKE_SOURCE_DIR}/deb/icon.png"
		DESTINATION "${PROJECT_INSTALL_ROOT}"
		CONFIGURATIONS Release
		COMPONENT debfiles
		EXCLUDE_FROM_ALL
		)
else()
	#Install dll-s too
	# install(TARGETS 
			# "brotlicommon.dll"
			# "brotlidec.dll"
			# "bz2.dll"
			# "freetype.dll"
			# "iconv-2.dll"
			# "intl-8.dll"
			# "libpng16.dll"
			# "ogg.dll"
			# "SDL2.dll"
			# "SDL2_gfx.dll"
			# "SDL2_image.dll"
			# "SDL2_mixer.dll"
			# "SDL2_ttf.dll"
			# "vorbis.dll"
			# "vorbisfile.dll"
			# "zlib1.dll"
			# DESTINATION "${PROJECT_EXE_ROOT}"
			# CONFIGURATIONS Release
		# )
	install(CODE "set(BUILD_ROOT \"${CMAKE_BINARY_DIR}\")") 
	install(CODE [[
	file(GET_RUNTIME_DEPENDENCIES 
		RESOLVED_DEPENDENCIES_VAR BRS_Dep
		UNRESOLVED_DEPENDENCIES_VAR BRS_Undep
		EXECUTABLES Release/bodyreswitcher.exe
		POST_EXCLUDE_REGEXES "^[A-Z]:"
		)
		list(TRANSFORM BRS_Dep PREPEND "${BUILD_ROOT}/")
		file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/." FILES ${BRS_Dep})
		]])
	set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
	set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/win32/icon.ico")
endif()

#CPack
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cpack_config.cmake")
include(CPack)

#for Visual Studio
if(MSVC)
	source_group(TREE "${CMAKE_SOURCE_DIR}/src" FILES "${BRS_sources}")
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT bodyreswitcher)
endif()
