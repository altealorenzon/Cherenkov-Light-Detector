#ifndef Muon_h
#define Muon_h
#include "Particle.h"
#include "Photon.h"
#include <random>

class Muon: public Particle {
    
public:
    Muon( Vector* x_0, double e, double theta_0, double phi_0, int anti = 1 );
    void Cherenkov( double n );
    std::vector<Photon*>* getPhotonList();
    
private:
    std::vector<Photon*>* photons;
    std::default_random_engine poisson_gen;
};

#endif
