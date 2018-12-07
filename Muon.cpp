#include "Muon.h"
#include <cmath>
#include <iostream>

Muon::Muon( Vector* x_0, double e, double theta_0, double phi_0, int anti) : Particle( anti*13, x_0, e, theta_0, phi_0 ) {
    photons = new std::vector<Photon*>();
}

void Muon::Cherenkov( double n ) {
    
    double v = this->getSpeed();
    Vector* x_0 = this->getLastPosition();
    
    if( v > 1/n ) {

        if(VERBOSE) std::cout << "I can do Cherenkov! "<< v << ">" << 1/n << std::endl;       
        
        double theta_0  = acos( 1/v/n ); //Cherenkov angle
        double k_prime  = 0.0010585;     //nm^-1 calculated with WolframAlpha
        double efficiency_max = 0.2;
        double k        = 2*M_PI*(1.0/137)*k_prime*efficiency_max; //nm^-1
        double lambda_c = 1/(k*sin(theta_0)*sin(theta_0))*0.0000001; //cm
        double lambda   = 300*1000000; //fm photon wavelength
        
        std::uniform_real_distribution<double> unif_dist(0,1);
        
        if( unif_dist(gen)<step_length/lambda_c ) {
            Photon* ph = new Photon( new Vector(*x_0), 197.4/lambda, theta_0, 2*M_PI*unif_dist( gen ) );
            photons->push_back( ph );
        } else if(VERBOSE) {
            std::cout << "No photons generated" << std::endl;
        }
        
    } 
    
    return;
    
}

std::vector<Photon*>* Muon::getPhotonList() {
    return photons;
}
