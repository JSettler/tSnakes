cmake_minimum_required(VERSION 3.10)
project(tSnakes)

# Set C++ standard (e.g., C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# --- Set Default Build Type to Release ---
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)." FORCE)
endif()
# --- End Set Default Build Type ---

# --- Compiler Flags for Release Mode ---
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -flto")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -flto")
# --- End Compiler Flags for Release Mode ---


# Find SFML
# The find_package command below should work for SFML 2.5.x and correctly define
# the variables for the component libraries (e.g., SFML_GRAPHICS_LIBRARY).
# For SFML 2.5.x, find_package(SFML ...) defines variables like SFML_LIBRARIES or individual component libraries.
# The target_link_libraries then uses these.
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED) 
# If you add audio or network later, add them to the COMPONENTS list above.

# Add your source files
add_executable(tSnakes 
    main.cpp 
    bot.cpp 
    common.cpp 
    radar.cpp 
    util.cpp
)

# Link SFML libraries (Corrected for SFML 2.5.x)
# For SFML 2.5.x, find_package makes variables like SFML_SYSTEM_LIBRARY, SFML_WINDOW_LIBRARY, SFML_GRAPHICS_LIBRARY available.
# Or, more commonly, the direct names sfml-system, sfml-window, sfml-graphics work if SFML's CMake find script sets them up as import targets or provides linker flags.
# The list below is generally robust for SFML 2.5.x installations.
target_link_libraries(tSnakes PRIVATE 
    sfml-graphics
    sfml-window
    sfml-system
    # If you were to use audio: sfml-audio
    # If you were to use network: sfml-network
)

# Alternative for SFML 2.5.x if the above doesn't work (less common with modern SFML find scripts):
# target_link_libraries(tSnakes PRIVATE ${SFML_GRAPHICS_LIBRARY} ${SFML_WINDOW_LIBRARY} ${SFML_SYSTEM_LIBRARY})

# If you ever upgrade to SFML 2.6+, you would use:
# target_link_libraries(tSnakes PRIVATE SFML::graphics SFML::window SFML::system)


