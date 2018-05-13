#ifndef Muon_h
#define Muon_h
#include "Particle.h"
#include "Photon.h"

class Muon: public Particle {
    
public:
    Muon( Vector* x_0, double e, double theta_0, double phi_0  );
    void Cherenkov();
    vector<Photon*>* getPhotonList();
    
private:
    vector<Photon*>* photons;
};

#endif
