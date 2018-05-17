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
    void             updatePosition();
    Vector*          getLastPosition();
    std::vector<Vector*>* getPositionList();

    static std::vector<particles_data*> my_particles;
    static void setParticlesData();

private:
    int     p_id;                   // id 
    double  mass;                   // massa 
    double  charge;                 // carica 
    double  energy;                 // energia 
    Vector* x;                      // vettore posizione 
    double  p;                      // modulo del momento 
    double  v;                      // velocità 
    double  step_length;            // passo di propagazione 
    double  theta;                  // angolo rispetto a z
    double  phi;                    // angolo sul piano x,y
    std::vector<Vector*>* position; // vettore con tutte le posizioni
    
};


#endif
