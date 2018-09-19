#include "Muon.h"
#include <cmath>
#include <iostream>

Muon::Muon( Vector* x_0, double e, double theta_0, double phi_0, int anti) :  Particle( anti*13, x_0, e, theta_0, phi_0 ) {
    photons = new std::vector<Photon*>();
    std::random_device rdv;
    gen.seed( rdv() );
}

void Muon::Cherenkov( double n ) {
    
    double v = this->getSpeed();
    Vector* x_0 = this->getLastPosition();
    std::uniform_real_distribution<double> dist(0, 1);
    
    if( v > 1/n ) {
        std::cout << "I can do Cherenkov! "<< v << ">" << 1/n << std::endl;
        
        double doCherenkov = dist( gen );
        double lambda = 300000000; //fm //TODO 
        double theta_0 = acos( 1/v/n );

        if( doCherenkov >= 0.5 && doCherenkov < 1 ) {
            std::cout << "New photon!" << std::endl;
<<<<<<< HEAD
            Photon* ph = new Photon( x_0, 197.4/lambda, theta_0, 2*M_PI*dist( gen ) );
=======
            Photon* ph = new Photon( new Vector( *x_0 ), 197.4/lambda, theta_0, 2*M_PI*dist( gen ) );
>>>>>>> 6641db7221ee782fa46adecc3ee30e88669206dd
            photons->push_back( ph );
        } 

    } 
    
    return;
    
}

std::vector<Photon*>* Muon::getPhotonList() {
    return photons;
}
