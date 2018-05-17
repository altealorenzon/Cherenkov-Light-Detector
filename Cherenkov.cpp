#include <iostream>
#include <random>
#include <vector>
#include "Setup.h"
#include "Vector.h"
#include "Muon.h"


int main( int argc, char* argv[] ) {
    
    Particle::setParticlesData();
    int nEvents = atoi( argv[1] );
    
    Setup* setup = new Setup( argv[2] );
    
    std::vector<Muon*>* muList;
    
    for( int i=0; i<nEvents; ++i ) {
        
        Vector* x_0   = setup->generateInitialPoint();
        double* angle = setup->generateInitialAngle();
        
        Muon* mu = new Muon( x_0, 4000, angle[0], angle[1] ); //TODO add charge
        mu->updatePosition();
        
        while ( setup->checkPosition( mu->getLastPosition() ) == true ) {
            //mu->Cherenkov();
            mu->updatePosition();
        }
        
        muList->push_back( mu );
    }
    
    return 0;
}
