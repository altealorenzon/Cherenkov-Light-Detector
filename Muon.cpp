#include "Muon.h"

Muon::Muon( Vector* x_0, double e, double theta_0, double phi_0, int anti) :  Particle( anti*13, x_0, e, theta_0, phi_0 ) {
};

void Muon::Cherenkov() {
    
}

// std::vector<Photon*>* Muon::getPhotonList() {
    
//}
