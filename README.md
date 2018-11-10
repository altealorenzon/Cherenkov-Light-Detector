# Description
Code for MC Simulation of Cherenkov effect by cosmic rays in a given material.

# How to compile
make

# How to run
./Cherenkov [number of events] [type of detector: cylinder/parallelepiped]

# Note
Some parameters are still encoded.
* in Setup.cpp: refraction index, dimensions of detector, distance of the trigger scintillator
* in Particle.h: VERBOSE variable
* in Particle.cpp: particles' data (mass, charge, step lenght)
