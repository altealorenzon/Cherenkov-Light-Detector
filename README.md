# Description
Code for MC Simulation of Cherenkov effect by cosmic rays in a given material. The charged particle trigges two scintillator detectors positioned above and below the radiator material. Cherenkov photons are gathered by a PMT.
All the information about the propagation of the charged particles and the photons are saved in a ROOT tuple. The utils directory contains a ROOT 

# How to compile
make

# How to run
./Cherenkov [number of events] [type of detector: c/p] [lateral walls property: r/a]

where:
* c = cylinder
* p = parallelepiped
* r = reflecting lateral walls
* a = absorbing lateral walls

# Note
Some parameters are still encoded.
* in Setup.cpp: refraction index, dimensions of detector, distance of the trigger scintillator, distance of the PMT plane.
* in Particle.h: VERBOSE variable
* in Particle.cpp: particles' data (mass, charge, step length)
* in SaveTree.cpp: the name of the output file

Total reflection on top/bottom is implemented for both a parallelepiped and a cylinder.
Total reflection on lateral wall is implemented only for a cylinder.
The choiche of absorbing/reflecting lateral walls can be done only for a cylinder. Lateral walls of a parallelepiped are always absorbing.

# About the directories
* The *output* directory contains the ROOT tuples produced running the Cherenkov simulation.
* The *utils* directory contains a ROOT macro to visualize the signals in the PMT plane and some photons' statistics from the simulation.
