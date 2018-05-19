#include <iostream>
#include <random>
#include <vector>
#include "Setup.h"
#include "Vector.h"
#include "Muon.h"
#include "TFile.h"
#include "TTree.h"

int main( int argc, char* argv[] ) {
    
    Particle::setParticlesData();
    int nEvents = atoi( argv[1] );
    
    std::cout << "Number of events: " << nEvents << std::endl;
    
    Setup* setup = new Setup( argv[2] );
    
    std::vector<Muon*>* muList = new std::vector<Muon*>();
    std::vector<Photon*>* phList = new std::vector<Photon*>();
    
    for( int i=0; i<nEvents; i++ ) {
        
        Vector* x_0   = setup->generateInitialPoint();
        double* angle = setup->generateInitialAngle(); // 0 theta, 1 phi
        
        Muon* mu = new Muon( x_0, 4000, angle[0], angle[1] ); //TODO add charge
        mu->updatePosition();
        
        while ( setup->checkPosition( mu->getLastPosition() ) == true ) {
            mu->Cherenkov( setup->getRefractionIndex() ); //TODO n becomes global
            mu->updatePosition();
        }
        
        phList = mu->getPhotonList();
        
        for( int j=0; j<phList->size(); j++ ) {
            
            phList->at( j )->updatePosition(); //new position in the muon rf
            phList->at( j )->rotatePosition( angle[0], angle[1] ); //new position in the global rf
            while( setup->checkPosition( phList->at( j )->getLastPosition() ) ) {
                //here we can add possible interactions
                phList->at( j )->updatePosition();  //new position in the muon rf
                phList->at( j )->rotatePosition( angle[0], angle[1] );  //new position in the global rf
            }
        }
        
        muList->push_back( mu );
    }
    
    return 0;
}
