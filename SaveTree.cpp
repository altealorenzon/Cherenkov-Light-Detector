#include <vector>
#include <iostream>
#include "Muon.h"
#include "Vector.h"
#include "TFile.h"
#include "TTree.h"

static void SaveTree( std::vector<Muon*>* muList ) {
    
    std::cout << "* Saving events!" << std::endl;
    
    int nEvents = muList->size();
    
    TFile *file = new TFile( "./output/Cherenkov_MC.root", "RECREATE" );
    TTree *tree = new TTree( "Cherenkov", "Cherenkov" );
    
    const int length = 10000;
    
    int evNumber;       tree->Branch( "event",        &evNumber,     "event/I"       );
    int id;             tree->Branch( "id",           &id,           "id/I"          );
    double energy;      tree->Branch( "energy",       &energy,       "energy/D"      );
    double theta_out;   tree->Branch( "theta_out",    &theta_out,    "theta_out/D"   );
    double phi_out;     tree->Branch( "phi_out",      &phi_out,      "phi_out/D"     );
    int position_out;   tree->Branch( "position_out", &position_out, "position_out/I");
    double x[length];   tree->Branch( "x",             x,            "x[1000]/D"     );
    double y[length];   tree->Branch( "y",             y,            "y[1000]/D"     );
    double z[length];   tree->Branch( "z",             z,            "z[1000]/D"     );
    int phNumber;       tree->Branch( "phNumber",     &phNumber,     "phNumber/I"    );
    
    for( int i=0; i<nEvents; i++ ) {
        
        if(i%5==0) std::cout << "* ..." << int(i*1.0/nEvents*100) << "\% completed" << std::endl;
        
        evNumber = i+1;
        
        //set all coordinates to (0,0,0)
        for( int j = 0; j < length; j++ ) {
            x[j] = y[j] = z[j] = 0;
        }
        
        Muon* mu = muList->at( i );
        
        id = 13;
        energy = muList->at( i )->getEnergy();
        
        //loop on muon positions
        int nPos = mu->getPositionList()->size();
        for( int j = 0; j <nPos; j++ ) {
            Vector* x_mu = mu->getPositionList()->at( j );
            x[j] = x_mu->getX();
            y[j] = x_mu->getY();
            z[j] = x_mu->getZ();
        }
        //fill tree with muon variables
        tree->Fill();
        
        //Loop on photons
        id = 22;
        phNumber = 0;
        for( std::vector<Photon*>::iterator it = mu->getPhotonList()->begin(); it != mu->getPhotonList()->end(); it++ ) {
            for( int j=0; j<length; j++ ) {
                x[j] = y[j] = z[j] = 0;
            }
            Photon* ph = *it;
            energy = ph->getEnergy();
            theta_out = ph->gettheta_out_ph();
            phi_out   = ph->getphi_out_ph();
            ++phNumber;
            //Loop on photons positions
            nPos = ph->getPositionList()->size();
            position_out = ph->getPosition_out();
            for( int j = 0; j < nPos; j++ ) {
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
    std::cout << "* ...100\% completed!" << std::endl;
};
