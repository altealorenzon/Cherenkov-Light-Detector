#ifndef Particle_h
#define Particle_h
#include "Vector.h"
#include <vector>

struct particles_data {
    particles_data(double mass = 0, double charge = 0, double step_length = 0): m(mass), q(charge), step(step_length) {};
    double m;
    double q;
    double step;
};

class Particle {

protected:
    Particle( int id, Vector* x_0, double e, double theta_0, double phi_0 );

public:
    int              getID();
    double           getMass();
    double           getCharge();
    double           getEnergy();
    double           getP();
    double           getSpeed();
    void             updatePosition();
    void             rotatePosition( double theta_1, double phi_1 );
    Vector*          getLastPosition();
    std::vector<Vector*>* getPositionList();

    static particles_data my_particles[30];
    static void setParticlesData();

private:
    int     p_id;                   // id of the particle
    double  mass;                   // mass of the particle 
    double  charge;                 // charge of the particle
    double  energy;                 // energy of the particle
    Vector* x;                      // position of the particle
    double  p;                      // momentum of the particle
    double  v;                      // speed of the particle 
    double  step_length;            // propagation step 
    double  theta;                  // azimuthal angle of the particle' momentum
    double  phi;                    // angle on x,y plane of the particle' momentum
    std::vector<Vector*>* position; // vector containing all the positions
    
};

#endif
