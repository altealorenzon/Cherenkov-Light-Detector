#ifndef Particle_h
#define Particle_h
#include "Vector.h"
#include <vector>

class Particle {

public:
    Particle( int id, Vector* x_0, double e, double theta_0, double phi_0 );
    int              getID();
    double           getMass();
    double           getCharge();
    double           getEnergy();
    double           getP();
    void             updatePosition();
    Vector*          getLastPosition();
    vector<Vector*>* getPositionList();

private:
    const int        p_id;
    const double     mass;
    const double     charge;
    double           energy;
    Vector*          x;
    double           p;
    const double     step;
    const double     theta;
    const double     phi;
    vector<Vector*>* position;
    
};

#endif
