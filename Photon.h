#ifndef Photon_h
#define Photon_h
#include "Particle.h"

class Photon: public Particle {

public:
    Photon( Vector* x_0, double e, double theta_0, double phi_0, int anti = 1 );
    
    int    getnReflections();
    double gettheta_out_ph();
    double getphi_out_ph();
    double getReflectionAngle(double r);
    void   updatePositionPh( double theta_1, double phi_1, Setup* setup );
    void   rotateProjections(double theta_1, double phi_1);
    void   reflectionPhWall(); //returns the reflected position
    void   printSummary();
    int    getPosition_out();
    
private:
    int    nReflections; //number of reflections on the side walls
    double proj_x      ; //x projection of the step_length
    double proj_y      ; //y projection of the step_length
    double proj_z      ; //z projeciton of the step_length
    double norm_proj   ; //norm of the shift
    double cos_theta_0 ;
    double cos_phi_0   ;
    double vers_r_x    ;
    double vers_r_y    ;
    double vers_r_z    ;
    double theta_ph_out; //Azimuthal angle of the trajectory of the photon at the box exit
    double phi_ph_out  ; //x-y angle of the trjectory of the phton at the box exit
    int    position_out; //= 1 if the photon goes out through the bottom wall of the detector

};

#endif
