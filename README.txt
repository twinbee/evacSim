## Description

EvacSim is a massive crowd simulation developed for the 2006-2007 Department of Homeland Security (DHS) Southeastern Regional Initiative (SERI) grant at the University of Southern Mississippi by Matthew Bennett, Mike Johnson, and Mike Erwin.

The simulation is used in training exercises for stadium and sports security personel as well as "what if" analysis. It was very advanced for the time, able to simulate 100000 simultaneous physically simulated flocking agents in egress from a stadium or surrounding areas such as roadways.

The project took first prize out of 14 projects and secured SERI grant funding for two years. 

https://www.youtube.com/watch?v=sNIrYu38E4E
[![Koanity Demo](https://img.youtube.com/vi/sNIrYu38E4E/0.jpg)](https://www.youtube.com/watch?v=sNIrYu38E4E) 


## Release notes

March, 2008

EvacSim 0.1

Requirements:

microsoft windows XP, Vista
2 GB RAM
2 GHz or higher processor
3d-accelerated graphics
2 GB disk space


Version history: (odd versions are development versions)

0.1 Initial release features:

explosions
basic scripting
multiple cameras
OBJ loader for navmeshes exported from Google Sketchup
OBJ loader for 3d decorative models exported from Google Sketchup
vision occlusion
cylindrical physical approximation of bodies
box physical approximation of walls
physical accuracy levels
rendering to jpeg
video post-compiling (with VirtualDub)
near-real time mode for 

Known issues:

The last time this project was built, it was with MinGW and the IDE Dev-C++ on windows XP in 2006. It was cross-compiled to work on linux, approximately Fedora 5, and used OpenGL 1.2 and ODE. This needs updating, obviously. New target (2018) might be something like cmake running on Centos 7 plus windows 7, along with more modern OpenGL and C++ compiler. 

Outstanding Tasks:

cmake-ify
make it run on windows and linux again
Remove any dependencies on hard to find or old libraries such as ODE and older SDL / OpenGL instances
Remove any Dev-C++ specific build artifacts
Make some new videos with modern graphics hardware, not the 2004-ish NVidia cards that were used for the older videos
