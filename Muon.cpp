#include "Muon.h"
#include <cmath>
#include <iostream>

Muon::Muon( Vector* x_0, double e, double theta_0, double phi_0, int anti) :  Particle( anti*13, x_0, e, theta_0, phi_0 ) {
    photons = new std::vector<Photon*>();
}

void Muon::Cherenkov( double n ) {
    
    double v = this->getSpeed();
    Vector* x_0 = this->getLastPosition();
    
    if( v > 1/n ) {
        std::cout << "I can do Cherenkov! "<< v << ">" << 1/n << std::endl;
        double lambda = 300000000; //fm
        double theta_0 = acos( 1/v/n );
        std::poisson_distribution<int> poisson_dist(5.0);
        
        int nPhotonGenerated = poisson_dist( poisson_gen );
        
        for( int nPh=0; nPh<nPhotonGenerated; ++nPh) {
            std::cout << "New photon!" << std::endl;
            Photon* ph = new Photon( new Vector(*x_0), 197.4/lambda, theta_0, 2*M_PI*dist( gen ) );
            photons->push_back( ph );
        } 
    } 
    
    return;
    
}

std::vector<Photon*>* Muon::getPhotonList() {
    return photons;
}
