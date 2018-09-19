#ifndef Photon_h
#define Photon_h
#include "Particle.h"


class Photon: public Particle {

public:
    Photon( Vector* x_0, double e, double theta_0, double phi_0, int anti = 1 );
    
    int  getnReflections();
    bool getcheckInside();
    
    void updatePositionPh( double theta_1, double phi_1, Setup* setup );
    void rotateProjections(double theta_1, double phi_1);
    void reflectionPh(double r); //returns the reflected position
    bool checkPositionPh();
    void printSummary();
    
private:
    int  nReflections; //number of reflections on the side walls
    bool checkInside;  //check if the photon is inside/outside the box
    double proj_x;                  //x projection of the step_length
    double proj_y;                  //y projection of the step_length
    double proj_z;                  //z projeciton of the step_length
    double norm_proj;
    double theta_ph_out;            //Azimuthal angle of the trajectory of the photon at the box exit
    double phi_ph_out;              //x-y angle of the trjectory of the phton at the box exit
    

};

#endif
