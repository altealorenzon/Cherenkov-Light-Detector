#include <vector>
#include <iostream>
#include "Muon.h"
#include "Vector.h"
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
        evNumber = i+1;
        
        //set all coordinates to (0,0,0)
        for( int j=0; j<200; j++ ) {
            x[j] = y[j] = z[j] = 0;
        }
        
        Muon* mu = muList->at( i );
        
        id = 13;
        energy = muList->at( i )->getEnergy();
        
        //loop on muon positions
        int nPos = mu->getPositionList()->size();
        for( int j=0; j<nPos; j++ ) {
            Vector* x_mu = mu->getPositionList()->at( j );
            x[j] = x_mu->getX();
            y[j] = x_mu->getY();
            z[j] = x_mu->getZ();
        }
        //fill tree with muon variables
        tree->Fill();
        
        //set all coordinates to (0,0,0)
        for( int j=0; j<200; j++ ) {
            x[j] = y[j] = z[j] = 0;
        }
        
        //Loop on photons
        id = 22;
        for( std::vector<Photon*>::iterator it = mu->getPhotonList()->begin(); it != mu->getPhotonList()->end(); it++ ) {
            Photon* ph = *it;
            energy = ph->getEnergy();
            //Loop on photons positions
            nPos = ph->getPositionList()->size();
            for( int j=0; j<nPos; j++ ) {
                Vector* x_ph = ph->getPositionList()->at( j );
                x[j] = x_ph->getX();
                y[j] = x_ph->getY();
                z[j] = x_ph->getZ();
            }  
            //fill tree with photon variables
            tree->Fill();
        }
    }
    
    tree->Write();
    file->Close();
    
};
