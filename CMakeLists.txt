cmake_minimum_required(VERSION 3.10.3)

#use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(BodyReSwitcher)
set(BRS_Version "2.0")
option(USE_CONSOLE_OUTPUT "Print error messages to console instead of showing a message window" OFF)

#add SDL2 libraries searching modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)
#find the SDL2 modules required by the game
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_gfx REQUIRED)
find_package(SDL2_ttf REQUIRED)

#the source list
set(BRS_sources src/main.cpp
				src/filesystem.cpp
				src/animation.cpp
				src/rect.cpp
				src/ui.cpp
				src/game_config.cpp
				src/errors.cpp
				src/mls.cpp
				src/multimedia/system.cpp
				src/multimedia/base.cpp
				src/multimedia/graphics.cpp
				src/multimedia/font.cpp
				src/multimedia/sound.cpp
				src/game_math/vector.cpp
				src/logic/common.cpp
				src/logic/module.cpp
				src/logic/buttons.cpp
				src/logic/main_menu_background.cpp
				src/logic/main_menu.cpp
				src/logic/about_menu.cpp
				src/logic/options_menu.cpp
				src/logic/screen_size_menu.cpp
				src/logic/new_game_menu.cpp
				src/logic/game_panel.cpp
				src/logic/person_graph.cpp
)

add_executable(bodyreswitcher ${BRS_sources})
#link with found SDL2 libraries
target_link_libraries(bodyreswitcher SDL2::Main SDL2::Mixer SDL2::Image SDL2::TTF SDL2::GFX)
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
set(PROJECT_INSTALL_ROOT "${CMAKE_INSTALL_PREFIX}\\BodyReSwitcher")
else()
set(PROJECT_INSTALL_ROOT "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/bodyreswitcher")
endif()

#Locale (used by GetText) library -----------
include(FindIntl)
find_package(Intl REQUIRED)

target_include_directories(bodyreswitcher PUBLIC "${Intl_INCLUDE_DIRS}")
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

#the game's main program file
install( TARGETS bodyreswitcher RUNTIME
DESTINATION "${PROJECT_INSTALL_ROOT}"
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
endif()

#CPack
set(CPACK_PROJECT_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cpack_config.cmake")
include(CPack)
