/**************************************************************
 *                       Reader.C                             *
 * ************************************************************
 * OBJECT
 * This macro reads data file of Silicon detectors, Scintillators and 
 * a PMT and store it in a root file.
 * 
 * DATA STRUCTURE
 * A Record is made by  words organized in this sequence:
 * 
 * ->  4x Si positions  -->  1x x position on the top Si
 *                      -->  1x y position on the top Si
 *                      -->  1x x position on the bottom Si
 *                      -->  1x y position on the bottom Si
 * ->  8x dgtz 25 ph  --> 1x ch0 :
 *                    --> 1x ch1 :
 *                    --> 1x ch2 :
 *                    --> 5x not used
 * ->  8x dgtz 20 ph  --> 1x ch0 : PMT ch 21
 *                    --> 1x ch1 : PMT ch 22
 *                    --> 1x ch2 : PMT ch 29
 *                    --> 1x ch3 : PMT ch 30
 *                    --> 1x ch4 : PMT ch 37
 *                    --> 1x ch5 : PMT ch 38
 *                    --> 1x ch6 : PMT ch 45
 *                    --> 1x ch7 : PMT ch 46
 * ->  8x dgtz 25 time
 * ->  8x dgtz 20 time 
 * ->  3x other words
 * 
 * TO COMPILE/RUN
 * $ root -l -q Reader.C+(SiRunNumber)
 * 
 */
 
#include <fstream>
#include "TH1F.h"
#include "TGraph.h"
#include "TString.h"
#include <iostream>
#include "TFile.h"
#include "TTree.h"

using namespace std;

const Int_t nSiLayers = 2;
const Int_t nChannelsPmt = 8;
const Int_t nTokensInRecord = 39;
const Int_t activeChannels[nChannelsPmt] = {21,22,29,30,37,38,45,46}; //copy the order above

typedef struct {
	Double_t xHit[nSiLayers];
	Double_t yHit[nSiLayers];
	Double_t z_xHit[nSiLayers];
	Double_t z_yHit[nSiLayers];
        Double_t theta; //Be carefull: it's a cos
        Double_t phi; //Be carefull: it's a cos
} Si_t;

typedef struct {
	Int_t    ChannelID[nChannelsPmt];
	Double_t PulseHeight[nChannelsPmt];
	Double_t Time[nChannelsPmt];
	Double_t xRadiator;
	Double_t yRadiator;
	Double_t zRadiator;
} Pmt_t;

typedef struct {
	Si_t  SiHit;
	Pmt_t PmtSignal;
} Ev_t;

void fillEvent( Ev_t &Ev, Double_t* vRecord );
void printEvent( Ev_t &Ev );
void projRad( Ev_t &Ev );

void ReadEvent( Int_t SiRunNumber, bool debug=false ) {

	TString outFile(Form("/home/sam/Cherenkov-Light-Detector/Data-Analysis/run%i.root",SiRunNumber));
	TFile* file = new TFile( outFile,"recreate" );
	TTree* tree = new TTree("Cherenkov","Tree with data from PMT, Silicon detectors and Scintillators in Cherenkov experiment");

	Ev_t Ev;
	TString s;
	Int_t evNumber=0;     				tree->Branch("evNumber",&evNumber,"evNumber/I");
	s = Form("xHit[%i]/D",nSiLayers); 		tree->Branch("xHit",Ev.SiHit.xHit,s);
	s = Form("yHit[%i]/D",nSiLayers); 		tree->Branch("yHit",Ev.SiHit.yHit,s);
	s = Form("z_xHit[%i]/D",nSiLayers); 		tree->Branch("z_xHit",Ev.SiHit.z_xHit,s);
	s = Form("z_yHit[%i]/D",nSiLayers); 		tree->Branch("z_yHit",Ev.SiHit.z_yHit,s);
							tree->Branch("phi",&Ev.SiHit.phi,"phi/D");
							tree->Branch("theta",&Ev.SiHit.theta,"theta/D");
	s = Form("PmtChannelID[%i]/I",nChannelsPmt);	tree->Branch("PmtChannelID",Ev.PmtSignal.ChannelID,s);
	s = Form("PmtPulseHeight[%i]/D",nChannelsPmt);  tree->Branch("PmtPulseHeight",Ev.PmtSignal.PulseHeight,s);
	s = Form("PmtTime[%i]/D",nChannelsPmt);         tree->Branch("PmtTime",Ev.PmtSignal.Time,s);
							tree->Branch("xRadiator",&Ev.PmtSignal.xRadiator,"xRadiator/D");
							tree->Branch("yRadiator",&Ev.PmtSignal.yRadiator,"yRadiator/D");
							tree->Branch("zRadiator",&Ev.PmtSignal.zRadiator,"zRadiator/D");
	//TODO add scintillators 
	
	ifstream file_ascii(Form("run%i.dat",SiRunNumber));
	Int_t evCounter=0;	
	Int_t nRecord=0;
	Double_t vRecord[nTokensInRecord];
	
	if(file_ascii.is_open()) {
		while(file_ascii>>vRecord[nRecord]) {
			++nRecord;
			if(nRecord==nTokensInRecord) {
				++evCounter;
				evNumber=evCounter;
				fillEvent(Ev,vRecord);
				if ( debug ) printEvent(Ev);
				tree->Fill();
				nRecord=0;
			}
		}
	} else {
        	cout << "ERROR: Unable to open file" << endl;
        	return;
        }
        tree->Write();
	return;

}

//\\//\\//\\//\\ FILL EVENT //\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

void fillEvent( Ev_t &Ev, Double_t* vRecord ) {
	Ssiz_t it=0;
	Ev.SiHit.xHit[0]=vRecord[it++];
	Ev.SiHit.yHit[0]=vRecord[it++];
	Ev.SiHit.z_xHit[0]= 9.00; //FIXME
	Ev.SiHit.z_yHit[0]= 10.5; //FIXME
	Ev.SiHit.xHit[1]=vRecord[it++];
	Ev.SiHit.yHit[1]=vRecord[it++];
	Ev.SiHit.z_xHit[1]= 19.2; //FIXME
	Ev.SiHit.z_yHit[1]= 20.7; //FIXME
	
	Ev.PmtSignal.zRadiator= 30.0; //FIXME questa è proprio a caso 
	
	//Function to calculate x,y projections and polar angles
	projRad(Ev);
	
	it+=8;
	for(Int_t i=0; i<nChannelsPmt; ++i) {
		Ev.PmtSignal.PulseHeight[i]=vRecord[it++];
	}
	it+=8;
	for(Int_t i=0; i<nChannelsPmt; ++i) {
		Ev.PmtSignal.Time[i]=vRecord[it++];
	}
	it+=3;
	if(it!=nTokensInRecord) {
		cout << "ERROR in parsing silicon data" << endl;
        	return;
	}
	for(Int_t i=0; i<nChannelsPmt; ++i) {
		Ev.PmtSignal.ChannelID[i]=activeChannels[i];
	}
}

//\\//\\//\\//\\ DEBUG: PRINT EVENT //\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

void printEvent( Ev_t &Ev ) {	cout << endl;
	cout << "******************* new event! *******************" << endl;
	cout << "* First Silicon layer" << endl;
	cout << "*      x = " << Ev.SiHit.xHit[0] << endl;
	cout << "*      y = " << Ev.SiHit.yHit[0] << endl;
	cout << "* Second Silicon layer" << endl;
	cout << "*      x = " << Ev.SiHit.xHit[1] << endl;
	cout << "*      y = " << Ev.SiHit.yHit[1] << endl;
	cout << "* PMT Pulse Heights " << endl;
	for(Int_t i=0; i<nChannelsPmt; ++i) {
		cout << "*      ch" << Ev.PmtSignal.ChannelID[i] << " :" << Ev.PmtSignal.PulseHeight[i] << endl;	
	}
	cout << "* PMT times " << endl;
	for(Int_t i=0; i<nChannelsPmt; ++i) {
		cout << "*      ch" << Ev.PmtSignal.ChannelID[i] << " :" << Ev.PmtSignal.Time[i] << endl;	
	}
	return;
}


//Function to compute the projection of the muons' trajectories on the xy plan of the radiator
void projRad(Ev_t &Ev) {

  //Compute the tanget  (angular coefficient of the direction) for x and y
  Double_t tgThetax = ( Ev.SiHit.z_xHit[1]- Ev.SiHit.z_xHit[0])/( Ev.SiHit.xHit[1]- Ev.SiHit.xHit[0] );
  Double_t tgThetay = ( Ev.SiHit.z_yHit[1]- Ev.SiHit.z_yHit[0])/( Ev.SiHit.yHit[1]- Ev.SiHit.yHit[0] );

  //Compute the projections on the radiator plan
  Ev.PmtSignal.xRadiator = (Ev.PmtSignal.zRadiator - Ev.SiHit.z_xHit[1] + tgThetax*Ev.SiHit.xHit[1])/tgThetax;
  Ev.PmtSignal.yRadiator = (Ev.PmtSignal.zRadiator - Ev.SiHit.z_yHit[1] + tgThetay*Ev.SiHit.yHit[1])/tgThetay;

  Double_t xyradius = sqrt(pow((Ev.SiHit.xHit[1] - Ev.SiHit.xHit[0]),2) + pow((Ev.SiHit.yHit[1] - Ev.SiHit.yHit[0]),2));
  
  //Compute the cos of the polar and azimuthal angles
  Ev.SiHit.phi = (Ev.SiHit.xHit[1] - Ev.SiHit.xHit[0])/xyradius; //cos of the azimuthal angle
  Ev.SiHit.theta = (Ev.SiHit.z_yHit[1] - Ev.SiHit.z_yHit[0])/sqrt(pow(xyradius,2) + pow((Ev.SiHit.z_yHit[1] - Ev.SiHit.z_yHit[0]),2)); //cos of the polar angle

  return;
}


