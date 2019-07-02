#include <vector>
#include <iostream>
#include "Muon.h"
#include "Vector.h"
#include "TFile.h"
#include "TTree.h"
#include <stdio.h>

static void SaveTree( std::vector<Muon*>* muList, int maxPos ) {
    
    std::cout << "* Saving events!" << std::endl;
    
    int nEvents = muList->size();
    
    TFile *file = new TFile( "./output/Cherenkov_MC.root", "RECREATE" );
    TTree *tree = new TTree( "Cherenkov", "Cherenkov" );
    
    //const int length = 10000;
    int length = maxPos;
    TString xstring = Form("x[%i]/D",maxPos);
    TString ystring = Form("y[%i]/D",maxPos);
    TString zstring = Form("z[%i]/D",maxPos);
    
    int evNumber;       tree->Branch( "evNumber",     &evNumber,     "evNumber/I"    );
    int id;             tree->Branch( "id",           &id,           "id/I"          );
    double energy;      tree->Branch( "energy",       &energy,       "energy/D"      );
    double x[maxPos];   tree->Branch( "x",             x,             xstring);
    double y[maxPos];   tree->Branch( "y",             y,            ystring  ); 
    double z[maxPos];   tree->Branch( "z",             z,            zstring );
    double theta_out;   tree->Branch( "theta_out",    &theta_out,    "theta_out/D"   );
    double phi_out;     tree->Branch( "phi_out",      &phi_out,      "phi_out/D"     );
    int position_out;   tree->Branch( "position_out", &position_out, "position_out/I");
    double x_PM;        tree->Branch( "x_PM",         &x_PM,         "x_PM/D"       );
    double y_PM;        tree->Branch( "y_PM",         &y_PM,         "y_PM/D"       );
    double z_PM;        tree->Branch( "z_PM",         &z_PM,         "z_PM/D"       );
    int phNumber;       tree->Branch( "phNumber",     &phNumber,     "phNumber/I"    );
    
    for( int i=0; i<nEvents; i++ ) {
        
        if(i%5==0) std::cout << "* ..." << int(i*1.0/nEvents*100) << "\% completed" << std::endl;
        
        evNumber = i+1;
        
        //set all coordinates to (0,0,0)
        for( int j = 0; j < length; j++ ) {
            x[j] = y[j] = z[j] = -999;
        }
        
        Muon* mu = muList->at( i );
        
        id = 13;
        energy = muList->at( i )->getEnergy();
        
        //loop on muon positions
        int nPos = mu->getPositionList()->size();
        
        for( int j = 0; j < nPos; j++ ) {
            Vector* x_mu = mu->getPositionList()->at( j );
            x[j] = x_mu->getX();
            y[j] = x_mu->getY();
            z[j] = x_mu->getZ();
        }
        
        position_out = 1;
        theta_out = mu->getTheta();
        phi_out = mu->getPhi();
        x_PM = x[nPos-1];
        y_PM = y[nPos-1];
        z_PM = z[nPos-1];
        phNumber = -999;
        //fill tree with muon variables
        tree->Fill();
        
        //Loop on photons
        id = 22;
        phNumber = 0;
        for( std::vector<Photon*>::iterator it = mu->getPhotonList()->begin(); it != mu->getPhotonList()->end(); it++ ) {
            x_PM = -999;
            y_PM = -999;
            z_PM = -999;
            for( int j=0; j<length; j++ ) {
                x[j] = y[j] = z[j] = -999;
            }
            
            Photon* ph = *it;
            energy = ph->getEnergy();
            ++phNumber;
            //Loop on photons positions
            nPos = ph->getPositionList()->size();
            for( int j = 0; j < nPos; j++ ) {
                Vector* x_ph = ph->getPositionList()->at( j );
                x[j] = x_ph->getX();
                y[j] = x_ph->getY();
                z[j] = x_ph->getZ();
            }
            position_out = ph->getPosition_out();
            theta_out = ph->getThetaOut_ph();
            phi_out   = ph->getPhiOut_ph();
            
            if( position_out == 1 ) {
                x_PM = x[nPos-1];
                y_PM = y[nPos-1];
                z_PM = z[nPos-1];
            }
            
            //fill tree with photon variables
            tree->Fill();
        }
    }
    
    tree->Write();
    file->Close();
    std::cout << "* ...100\% completed!" << std::endl;
};
