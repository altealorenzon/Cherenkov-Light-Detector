#include <vector>
#include <iostream>
#include "Muon.h"
#include "TFile.h"
#include "TTree.h"

static void SaveTree( std::vector<Muon*>* muList ) {
    
    int nEvents = muList->size();
    
    TFile *file = new TFile( "./output/Cherenkov_MC.root", "RECREATE" );
    TTree *tree = new TTree( "Cherenkov", "Cherenkov" );
    
    int evNumber;       tree->Branch( "event",     &evNumber,  "event/I"     );
    int id;             tree->Branch( "id",        &id,        "id/I"        );
    double energy;      tree->Branch( "energy",    &energy,    "energy/D"    );
    double x[200];      tree->Branch( "x",         x,          "x[200]/D"    );
    double y[200];      tree->Branch( "y",         y,          "y[200]/D"    );
    double z[200];      tree->Branch( "z",         z,          "z[200]/D"    );
    
    for( int i=0; i<nEvents; i++ ) {
        
        for( int j=0; j<200; j++ ) {
            x[j] = y[j] = z[j] = {0};
        }
        
        evNumber = i+1;
        Muon* mu = muList->at( i );
        int nPos = mu->getPositionList()->size();
        
        //TODO add a loop on the number of particles to include photons
        id = 13;
        energy = muList->at( i )->getEnergy();
        for( int j=0; j<nPos; j++ ) {
            x[j] = mu->getPositionList()->at( j )->getX();
            y[j] = mu->getPositionList()->at( j )->getY();
            z[j] = mu->getPositionList()->at( j )->getZ();
            std::cout << z[j] << std::endl;
        }
        tree->Fill();
        
        for( int j=0; j<200; j++ ) {
            x[j] = y[j] = z[j] = 0;
        }
        
        id = 22;
        for(std::vector<Photon*>::iterator it = mu->getPhotonList()->begin(); it != mu->getPhotonList()->end(); it++) {
            Photon* ph = *it;
            energy = ph->getEnergy();
            nPos = ph->getPositionList()->size();
            for( int j=0; j<nPos; j++ ) {
                x[j] = ph->getPositionList()->at( j )->getX();
                y[j] = ph->getPositionList()->at( j )->getY();
                z[j] = ph->getPositionList()->at( j )->getZ();
            }   
            tree->Fill();
        }
        
    }
    
    tree->Write();
    file->Close();
};
