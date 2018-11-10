#ifndef Particle_h
#define Particle_h
#include "Vector.h"
#include <vector>
#include "Setup.h"

struct particles_data {
    particles_data( double mass = 0, double charge = 0, double step_length = 0): 
                    m(mass), q(charge), step(step_length) {};
    double m;
    double q;
    double step;
};

static bool VERBOSE = 0;

class Particle {

protected:
    Particle( int id, Vector* x_0, double e, double theta_0, double phi_0 );
    Vector* x;                      // position of the particle
    std::vector<Vector*>* position; // vector containing all the positions
    double  step_length;            // cm propagation step
    int     p_id;                   // id of the particle
    double  mass;                   // mass of the particle
    double  charge;                 // charge of the particle
    double  energy;                 // energy of the particle
    double  p;                      // momentum of the particle
    double  v;                      // speed of the particle
    double  theta;                  // azimuthal angle of the particle' momentum
    double  phi;                    // angle on x,y plane of the particle' momentum
    std::mt19937 gen;
    
public:
    int                   getID();
    double                getMass();
    double                getCharge();
    double                getEnergy();
    double                getP();
    Vector*               getX();
    double                getSpeed();
    void                  updatePosition();
    Vector*               getLastPosition();
    std::vector<Vector*>* getPositionList();
        
    static particles_data my_particles[30];
    static void setParticlesData();
    
};

#endif
