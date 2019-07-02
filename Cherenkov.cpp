#include <iostream>
#include <vector>
#include <cmath>
#include "Setup.h"
#include "Vector.h"
#include "Muon.h"
#include "SaveTree.cpp"
#include "Photon.h"

int main( int argc, char* argv[] ) {
    
    //Definition of particles
    Particle::setParticlesData();
    
    //Choice of number of events to be simulated
    int nEvents = atoi( argv[1] );
    
    std::cout << "******************** NEW SIMULATION! ********************" << std::endl;
    std::cout << "* Number of events: " << nEvents << std::endl;
    
    //Choice of the setup geometry
    Setup* setup = new Setup( argv[2], argv[3] );
    
    std::vector<Muon*>* muList = new std::vector<Muon*>();
    
    for( int i=0; i<nEvents; i++ ) {
        
        std::cout << "* ...generating event " << i+1 << std::endl;
        
        //Generation and propagation of muons
        Vector* x_0   = setup->generateInitialPoint();
        double* angle = setup->generateInitialAngle(); // element 0 = theta, element 1 = phi
        
        Muon* mu = new Muon( x_0, 4000, angle[0], angle[1] );
        mu->updatePosition();
        
        while ( setup->checkPosition( mu->getLastPosition() ) == true ) {
            //Generation of Cherenkov photons
            mu->Cherenkov( setup->getRefractionIndex() ); 
            mu->updatePosition();
        }
        
        mu->hitPM( setup->getPMdistance(), angle[0], angle[1] );
        //Propagation of photons
        std::vector<Photon*>* phList = mu->getPhotonList();
        
        for( int j=0; j < phList->size(); j++ ) {
            //new projections in the global rf
            phList->at( j )->rotateProjections( angle[0], angle[1] ); 
            while( setup->checkPosition( phList->at( j )->getLastPosition() ) == true ) {
                //new position in the global rf
                phList->at( j )->updatePositionPh( angle[0], angle[1], setup);
            }
            if( phList->at( j )->getPosition_out() == 1 ) {
                double theta_prime = asin( setup->getRefractionIndex()*sin( phList->at( j )->getThetaOut_ph() ) );
                double phi_prime   = phList->at( j )->getPhiOut_ph();
                
                phList->at( j )->hitPM( setup->getPMdistance(), theta_prime, phi_prime );
            }  
        }
        
        muList->push_back( mu );
    }
    
    //Save events in a root TTree
    int maxPos = 0;
    for(int m = 0; m < muList->size(); m++) {
    
        Muon* mu = muList->at(m);
        int maxPos_temp = mu->nPos;
        std::vector<Photon*>* photons = mu->getPhotonList();
        for(int p = 0; p < photons->size(); p++) {
            if(photons->at(p)->nPos > maxPos_temp) maxPos_temp = photons->at(p)->nPos;
        }
        if(maxPos_temp > maxPos) maxPos = maxPos_temp;
    
    }
    SaveTree( muList, maxPos );

    std::cout << "*********************************************************" << std::endl;
    
    return 0;
}
