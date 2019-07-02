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

const Int_t nSiLayers = 2;
const Int_t nChannelsPmt = 8;
const Int_t nTokensInRecord = 39;
const Int_t activeChannels[nChannelsPmt] = {21,22,29,30,37,38,45,46}; //copy the order above

typedef struct {
	Double_t xHit[nSiLayers];
	Double_t yHit[nSiLayers];
} Si_t;

typedef struct {
	Int_t    ChannelID[nChannelsPmt];
	Double_t PulseHeight[nChannelsPmt];
	Double_t Time[nChannelsPmt];
} Pmt_t;

typedef struct {
	Si_t  SiHit;
	Pmt_t PmtSignal;
} Ev_t;

void fillEvent( Ev_t &Ev, Double_t* vRecord );
void printEvent( Ev_t &Ev );

void ReadEvent( Int_t SiRunNumber, bool debug=false ) {

	TString outFile(Form("/home/altea/Documents/Cherenkov_Analysis/run%i.root",SiRunNumber));
	TFile* file = new TFile( outFile,"recreate" );
	TTree* tree = new TTree("Cherenkov","Tree with data from PMT, Silicon detectors and Scintillators in Cherenkov experiment");

	Ev_t Ev;
	TString s;
	Int_t evNumber=0;     				tree->Branch("evNumber",&evNumber,"evNumber/I");
	s = Form("xHit[%i]/D",nSiLayers); 		tree->Branch("xHit",Ev.SiHit.xHit,s);
	s = Form("yHit[%i]/D",nSiLayers); 		tree->Branch("yHit",Ev.SiHit.yHit,s);
	s = Form("PmtChannelID[%i]/I",nChannelsPmt);	tree->Branch("PmtChannelID",Ev.PmtSignal.ChannelID,s);
	s = Form("PmtPulseHeight[%i]/D",nChannelsPmt);  tree->Branch("PmtPulseHeight",Ev.PmtSignal.PulseHeight,s);
	s = Form("PmtTime[%i]/D",nChannelsPmt);         tree->Branch("PmtTime",Ev.PmtSignal.Time,s);
	//TODO add scintillators if necessary
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
	Ev.SiHit.xHit[1]=vRecord[it++];
	Ev.SiHit.yHit[1]=vRecord[it++];
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

void printEvent( Ev_t &Ev ) {
	cout << endl;
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

