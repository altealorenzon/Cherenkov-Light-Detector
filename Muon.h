#ifndef Muon_h
#define Muon_h
#include "Particle.h"
// #include "Photon.h"

class Muon: public Particle {
    
public:
    Muon( Vector* x_0, double e, double theta_0, double phi_0, int anti = 1 );
    void Cherenkov();
    //std::vector<Photon*>* getPhotonList();
    
private:
    //std::vector<Photon*>* photons;
    
};

#endif
