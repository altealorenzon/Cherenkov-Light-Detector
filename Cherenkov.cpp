#include <iostream>
#include <random>
#include <vector>
#include "Setup.h"
#include "Vector.h"
#include "Muon.h"
<<<<<<< HEAD
//#include "TFile.h"
//include "TTree.h"
//#include "SaveTree.cpp" //e' la funzione che permette di salvare tutto in root e include anche "TFile.h" e "TTree.h"
=======
#include "SaveTree.cpp"
>>>>>>> 6641db7221ee782fa46adecc3ee30e88669206dd

int main( int argc, char* argv[] ) {
    
    Particle::setParticlesData();
    int nEvents = atoi( argv[1] );
    
    std::cout << "Number of events: " << nEvents << std::endl;
    
    Setup* setup = new Setup( argv[2] );
    
    std::vector<Muon*>* muList = new std::vector<Muon*>();
    
    for( int i=0; i<nEvents; i++ ) {
        
        Vector* x_0   = setup->generateInitialPoint();
        double* angle = setup->generateInitialAngle(); // 0 theta, 1 phi
        
        Muon* mu = new Muon( x_0, 4000, angle[0], angle[1] ); //TODO add charge
        mu->updatePosition();
        
        while ( setup->checkPosition( mu->getLastPosition() ) == true ) {
            mu->Cherenkov( setup->getRefractionIndex() ); //TODO n becomes global
            mu->updatePosition();
        }
<<<<<<< HEAD
        
=======
>>>>>>> 6641db7221ee782fa46adecc3ee30e88669206dd
        std::vector<Photon*>* phList = mu->getPhotonList();
        
        for( int j=0; j < phList->size(); j++ ) {
            std::cout << "PHOTON NUMBER " << j+1 << "." << std::endl;
            phList->at( j )->rotateProjections( angle[0], angle[1] ); //new projections in the global rf
            while( setup->checkPosition( phList->at( j )->getLastPosition() ) == true ) {
//              here we can add possible interactions
                phList->at( j )->updatePositionPh( angle[0], angle[1], setup);  //new position in the global rf
                }
            
<<<<<<< HEAD
=======
            phList->at( j )->rotatePosition( angle[0], angle[1] );
            while( setup->checkPosition( phList->at( j )->getLastPosition() ) == true ) {
//                 here we can add possible interactions riflessione! diffusione

                phList->at( j )->rotatePosition( angle[0], angle[1] ); 
            }
>>>>>>> 6641db7221ee782fa46adecc3ee30e88669206dd
        }
        
        
        muList->push_back( mu );
    }
    
<<<<<<< HEAD
    //SaveTree( muList ); //va messo per salvare tutto in root
=======
    SaveTree( muList );
>>>>>>> 6641db7221ee782fa46adecc3ee30e88669206dd
    
    return 0;
}
