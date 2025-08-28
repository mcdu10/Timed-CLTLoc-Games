cmake_minimum_required(VERSION 3.29)
project(Timed_CLTLoc_Games)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Timed_CLTLoc_Games
        Region.cpp
    Region.h
        Transition.cpp
        Transition.h
        Guard.cpp
        Guard.h
        TimedArena.cpp
        TimedArena.h
        main.cpp
        CLTLocFormula.cpp
        CLTLocFormula.h
        RegionTransitionSystem.cpp
        RegionTransitionSystem.h
        image.cpp
        image.h
)
