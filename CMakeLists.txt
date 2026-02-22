
# -- CMake Setup ---------------------------------------------------------------
cmake_minimum_required(VERSION 3.31.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# -- Project Definition --------------------------------------------------------
project(demigod)

# -- Vendored Libraries --------------------------------------------------------
set(DMG_VENDOR_PATH ${PROJECT_SOURCE_DIR}/vendor/)
if(EXISTS ${DMG_VENDOR_PATH}/SDL/CMakeLists.txt)
   add_subdirectory(${DMG_VENDOR_PATH}/SDL EXCLUDE_FROM_ALL)
else()
   find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

# -- Module --------------------------------------------------------------------
set(DMG_SOURCE_PATH ${PROJECT_SOURCE_DIR}/source/)
set(DMG_SOURCES
   ${DMG_SOURCE_PATH}/demigod.c
   ${DMG_SOURCE_PATH}/dmg_display.c
)

set(DMG_HEADER_PATH ${PROJECT_SOURCE_DIR}/header/)
set(DMG_HEADERS
   ${DMG_HEADER_PATH}/demigod.h
   ${DMG_HEADER_PATH}/dmg_display.h
)

add_executable(demigod WIN32 ${DMG_SOURCES} ${DMG_HEADERS})

target_link_libraries(demigod PUBLIC SDL3::SDL3)
target_include_directories(demigod PUBLIC ${DMG_HEADER_PATH})

