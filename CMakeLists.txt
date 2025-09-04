cmake_minimum_required(VERSION 3.29)
project(Timed_CLTLoc_Games)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${PROJECT_SOURCE_DIR}/TimedGames/include
        ${PROJECT_SOURCE_DIR}/Utility/include
)

add_executable(TimedGames
        TimedGames/source/Region.cpp
        TimedGames/include/Region.h
        TimedGames/source/Transition.cpp
        TimedGames/include/Transition.h
        TimedGames/source/Guard.cpp
        TimedGames/include/Guard.h
        TimedGames/source/TimedArena.cpp
        TimedGames/include/TimedArena.h
        main.cpp
        TimedGames/source/RegionTransitionSystem.cpp
        TimedGames/include/RegionTransitionSystem.h
        Utility/source/image.cpp
        Utility/include/image.h
        Utility/source/Validation.cpp
        Utility/include/Validation.h
)
