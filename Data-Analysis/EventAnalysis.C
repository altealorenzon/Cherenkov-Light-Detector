/*********************EventAnalysis.C******************
 *
 * To compile: $ root -l 
 *             .L EventAnalysis.C
 *             xyrad_histo(SiRunNumber,thr_theta, thr_xRadiator, thr_yRadiator, thr_x0, thr_y0)
 *	       RunStatsPmt(SiRunNumber)
 *	       ShowPmtSignal(SiRunNumber,EventNumber)
 *
 * * * RunStatsPmt
 * Input:
 * + Int_t SiRunNumber : number of the data taking run (300126,300127,300128,300129)
 *
 * Output:
 * + A canvas with the time spectrum histograms for each channel
 * + A canvas with the pulse height spectrum histograms before and after a time window cut, for each channel
 * + A list of events with high pulse height in at least 4 channels
 * 
 * * * xyrad_histo
 * Input:
 * + Double_t thr_theta    : the LOWER threshold on the cos of the polar angle 
 * + Double_t thr_xRadiator: the maximum x displacement between the hit point and the radiator's center 
 * + Double_t thr_yRadiator: the maximum y displacement between the hit point and the radiator's center
 * + Double_t thr_x0       : the maximum x displacement between the hit point of the UPPER Si detecror and it's center
 * + Double_t thr_y0       : the maximum y displacement between the hit point on the UPPER Si detector and it's center 
 *
 * Output: 
 * + A canvas with four histograms:
 *    1: The hit on the x axis of the radiator without (blue line) and with (red line) filters;
 *    2: " " "          y "    "  "                  
 *    3: A two 2D histogram x vs y, with the filters
 *    4: All the cos of the polar angles
 * + A canvas of three histograms with the hits on the UPPER and LOWER Si and on the Radiator.
 * + A txt file with the list of selected events
 *
 * Example: xyrad_histo(300126, 0.99, 2.0, 2.0, 10.0, 10.0).
 * --> I'm filtering only on the radiator since the thr on the upper Si is higher than the Si's dimensions.
 *
 *
  * * * ShowPmtSignal
 * Input:
 * + Int_t SiRunNumber : number of the data taking run (300126,300127,300128,300129)
 * + Int_t evNumber    : number of selected events
 *
 * Output:
 * + A canvas with the a 2d histogram representing the signal intensity of the Pmt in that particular event
 ********************************************************/



#include <fstream>
#include <iostream>
#include "TH1F.h"
#include "TGraph.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"

using namespace std;

const Int_t nSiLayers = 2;
const Int_t nChannelsPmt = 26; // or 26
const Int_t activeChannels[26] = {21,22,29,30,37,38,45,46,5,6,13,14,53,54,61,62,40,39,32,31,24,23,16,15,7,8};
//(x,y) bin numbers corresponding to the active channels of the Pmt
//			21 22 29 30 37 38 45 46  5  6 13 14 53 54 61 62 40 39 32 31 24 23 16 15  7  8
const Int_t xBin[26] = { 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 8, 7, 8, 7, 8, 7, 8, 7, 7, 8};
const Int_t yBin[26] = { 6, 6, 5, 5, 4, 4, 3, 3, 8, 8, 7, 7, 2, 2, 1, 1, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8};

const Double_t xcenterRadiator = 4.96292;
const Double_t ycenterRadiator = 3.97302;
const Double_t Sidistx = 12.05;
const Double_t Sidisty = 8.7;

typedef struct {
	Double_t xHit[nSiLayers];
	Double_t yHit[nSiLayers];
	Double_t z_xHit[nSiLayers];
	Double_t z_yHit[nSiLayers];
	Double_t theta; //Be careful: it's a cos
        Double_t phi;   //Be careful: it's a cos
} Si_t;

typedef struct {
	Int_t    DgtzID[nChannelsPmt];
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

void RunStatsPmt(Int_t SiRunNumber);
void xyrad_histo(Int_t SiRunNumber,Double_t thr_theta,Double_t thr_Radiator,Double_t thr_x0,Double_t thr_y0);
void ShowPmtSignal(Int_t SiRunNumber, Int_t evNumber);
void PrintEventOnFile(Int_t SiRunNumber, Int_t EvNumber);
void FillEvHisto(TTree* intree, Int_t ev, TH2F* h2_Dgtz20, TH2F* h2_Dgtz25, TH2F* h2_Dgtz31, TH2F* h2_all, Int_t norm);
void CumPmtSignal(Int_t SiRunNumber, Int_t mod, Int_t ev);

//\\//\\//\\//\\// RUNSTATSPMT //\\//\\//\\//\\//\\//\\//
//See the time spectrum and PH spectrum of PMT active channels and select events in a particular time window
void RunStatsPmt(Int_t SiRunNumber) {
	
	TString infile_name = Form("run%i.root",SiRunNumber);
	TFile* infile = new TFile(infile_name,"READ");
  	if(infile->IsOpen()) printf("File opened successfully\n");
	TTree* intree = (TTree*)infile->Get("Cherenkov");
	
	Int_t nEntries = intree->GetEntries();
	cout << "Total number of events: " << nEntries << endl;
	
	Ev_t Ev;
	Int_t evNumber;     				
	intree->SetBranchAddress("evNumber",&evNumber);
	intree->SetBranchAddress("PmtTime",Ev.PmtSignal.Time);
	intree->SetBranchAddress("PmtPulseHeight",Ev.PmtSignal.PulseHeight);
	intree->SetBranchAddress("PmtChannelID",Ev.PmtSignal.ChannelID);
	intree->SetBranchAddress("DgtzID",Ev.PmtSignal.DgtzID);
	
	TH1F* PmtTime_Histo[nChannelsPmt];
	TH1F* PmtPulseHeight_Histo[nChannelsPmt];
	TH1F* PmtPulseHeight_HistoInTime[nChannelsPmt];
	
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		PmtTime_Histo[iChannel] = new TH1F(Form("PmtTime_%i",iChannel),Form("ch %i Time [a.u.]",activeChannels[iChannel]),50,50,250);
		PmtTime_Histo[iChannel]->SetFillColor(kAzure-8);
		PmtTime_Histo[iChannel]->SetLineColor(kAzure-8);
		PmtPulseHeight_Histo[iChannel] = new TH1F(Form("PmtPulseHeight_Histo_%i",iChannel),Form("ch %i Pulse Height [a.u.]",activeChannels[iChannel]),200,0,1000);
		PmtPulseHeight_Histo[iChannel]->SetLineColor(kBlue+2);
		PmtPulseHeight_Histo[iChannel]->SetFillColor(kBlue+2);
		PmtPulseHeight_HistoInTime[iChannel] = new TH1F(Form("PmtPulseHeight_HistoInTime_%i",iChannel),Form("ch %i",activeChannels[iChannel]),200,0,1000);
		PmtPulseHeight_HistoInTime[iChannel]->SetLineColor(kRed-7);
		PmtPulseHeight_HistoInTime[iChannel]->SetFillColor(kRed-7);
	}
	
	
	Int_t timeWindow_lowerBound;
	Int_t timeWindow_upperBound;
	
	for(Int_t iEntry=0; iEntry<nEntries; ++iEntry) {
		intree->GetEntry(iEntry);
		Int_t chCounter=0;
		for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		  PmtTime_Histo[iChannel]->Fill(Ev.PmtSignal.Time[iChannel]);
		  if(Ev.PmtSignal.DgtzID[iChannel]!=31) {
		    PmtPulseHeight_Histo[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]);
		    timeWindow_lowerBound=80;
		    timeWindow_upperBound=120;
		  } else if(Ev.PmtSignal.DgtzID[iChannel]==31) {
		    PmtPulseHeight_Histo[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]/4);
		    timeWindow_lowerBound=190;
		    timeWindow_upperBound=230;
			}
		  if(Ev.PmtSignal.Time[iChannel]>=timeWindow_lowerBound && Ev.PmtSignal.Time[iChannel]<=timeWindow_upperBound) {
		    if(Ev.PmtSignal.DgtzID[iChannel]==31) {
		      PmtPulseHeight_HistoInTime[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]/4);
		      //if(Ev.PmtSignal.PulseHeight[iChannel]>=150) ++chCounter;
		    }
		    else {
		      PmtPulseHeight_HistoInTime[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]);
		    }
		  }
		}
		//if(nChannelsPmt==8&&chCounter>=4) cout << "High PH in " << chCounter << " channels. Event number = " << evNumber << endl;
		//if(nChannelsPmt==26&&chCounter>=17) cout << "High PH in " << chCounter << " channels. Event number = " << evNumber << endl;
	}
	
	gStyle->SetTitleFontSize(0.07);
	
	TLine* line_lowerBound[nChannelsPmt]; 
	TLine* line_upperBound[nChannelsPmt];
	
	TCanvas* c_time = new TCanvas("c_time","",1500,750);
	TPaveText* pt_time[nChannelsPmt];
	Int_t nRows=2;
	Int_t nColumns=4;
	if(nChannelsPmt==26) {
		nRows=4;
		nColumns=7;
	}
	c_time->Divide(nColumns,nRows);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		c_time->cd(iChannel+1);
		c_time->cd(iChannel+1)->SetLogy();
		c_time->cd(iChannel+1)->SetLeftMargin(0.1484999);
		PmtTime_Histo[iChannel]->SetStats(0);
		//PmtTime_Histo[iChannel]->GetXaxis()->SetTitle("Time [a.u.]");
		PmtTime_Histo[iChannel]->GetXaxis()->SetNdivisions(505);
		PmtTime_Histo[iChannel]->GetXaxis()->SetLabelFont(42);
    		PmtTime_Histo[iChannel]->GetXaxis()->SetLabelSize(0.06);
    		PmtTime_Histo[iChannel]->GetYaxis()->SetLabelFont(42);
    		PmtTime_Histo[iChannel]->GetYaxis()->SetLabelSize(0.06);
		PmtTime_Histo[iChannel]->Draw();
		if(iChannel>=8&&iChannel<24) {
			timeWindow_lowerBound=190;
			timeWindow_upperBound=230;
		} else {
			timeWindow_lowerBound=80;
			timeWindow_upperBound=120;
		}
		line_lowerBound[iChannel] = new TLine(timeWindow_lowerBound,0,timeWindow_lowerBound,PmtTime_Histo[iChannel]->GetBinContent(PmtTime_Histo[iChannel]->GetMaximumBin()));
		line_lowerBound[iChannel]->SetLineColor(kBlack);
		line_lowerBound[iChannel]->Draw("SAME");
		line_upperBound[iChannel] = new TLine(timeWindow_upperBound,0,timeWindow_upperBound,PmtTime_Histo[iChannel]->GetBinContent(PmtTime_Histo[iChannel]->GetMaximumBin()));
		line_upperBound[iChannel]->SetLineColor(kBlack);
		line_upperBound[iChannel]->Draw("SAME");
		if(iChannel>=8&&iChannel<24) {
			pt_time[iChannel] = new TPaveText(0.18,0.65,0.62,0.86,"blNDC");
		} else {
			pt_time[iChannel] = new TPaveText(0.55,0.65,0.99,0.86,"blNDC");
		}
		pt_time[iChannel]->SetBorderSize(1);
		pt_time[iChannel]->SetLineColor(kBlack);
		pt_time[iChannel]->SetFillColor(kWhite);
		pt_time[iChannel]->SetTextFont(42);
    		pt_time[iChannel]->SetTextSize(0.07);
    		pt_time[iChannel]->AddText(Form("Events: %i", (Int_t)PmtTime_Histo[iChannel]->Integral(PmtTime_Histo[iChannel]->FindBin(timeWindow_lowerBound),PmtTime_Histo[iChannel]->FindBin(timeWindow_upperBound))));
    		pt_time[iChannel]->Draw();
	}
	
	TCanvas* c_ph = new TCanvas("c_ph","",1500,750);
	TPaveText* pt[nChannelsPmt];
	c_ph->Divide(nColumns,nRows);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		c_ph->cd(iChannel+1);
		PmtPulseHeight_Histo[iChannel]->SetStats(0);
		/*if(iChannel<8 || iChannel>23)*/ PmtPulseHeight_Histo[iChannel]->GetXaxis()->SetRangeUser(0,300);
		PmtPulseHeight_Histo[iChannel]->GetXaxis()->SetNdivisions(505);
		PmtPulseHeight_Histo[iChannel]->GetXaxis()->SetLabelFont(42);
    		PmtPulseHeight_Histo[iChannel]->GetXaxis()->SetLabelSize(0.06);
		PmtPulseHeight_Histo[iChannel]->GetYaxis()->SetNdivisions(505);
    		PmtPulseHeight_Histo[iChannel]->GetYaxis()->SetLabelFont(42);
    		PmtPulseHeight_Histo[iChannel]->GetYaxis()->SetLabelSize(0.06);
		PmtPulseHeight_Histo[iChannel]->Draw();
		PmtPulseHeight_HistoInTime[iChannel]->SetStats(0);
		PmtPulseHeight_HistoInTime[iChannel]->Draw("SAME");
		pt[iChannel] = new TPaveText(0.23,0.65,0.99,0.86,"blNDC");
		pt[iChannel]->SetBorderSize(1);
		pt[iChannel]->SetLineColor(kBlack);
		pt[iChannel]->SetFillColor(kWhite);
		pt[iChannel]->SetTextFont(42);
    		pt[iChannel]->SetTextSize(0.07);
    		pt[iChannel]->AddText(Form("Selected events: %2.1f%%", PmtPulseHeight_HistoInTime[iChannel]->GetEntries()*1.0/PmtPulseHeight_Histo[iChannel]->GetEntries()*100));
    		pt[iChannel]->Draw();
	}
	
	return;
}

//\\//\\//\\//\\// XYRAD_HIST0 //\\//\\//\\//\\//\\//\\//
// Select events by cutting on track direction and hits positions 
void xyrad_histo(Int_t SiRunNumber,
		 Double_t thr_theta,
		 Double_t thr_Radiator,
		 Double_t thr_x0,
		 Double_t thr_y0) {

	TString infile_name = Form("run%i.root",SiRunNumber);
	TFile* infile = new TFile(infile_name, "READ");
  	if(infile->IsOpen()) printf("File opened successfully\n");
  	TTree* intree = (TTree*)infile->Get("Cherenkov");
  	
  	//hit projection on radiator
  	TH1F* xRadiator_Histo = new TH1F("xRadiator_Histo", "x of Radiator", 50, 0, 10);
  	TH1F* yRadiator_Histo = new TH1F("yRadiator_Histo", "y of Radiator", 50, 0, 10);
  	TH1F* xRadiator_HistoInRange = new TH1F("xRadiator_HistoInRange", "", 50, 0, 10);
  	TH1F* yRadiator_HistoInRange = new TH1F("yRadiator_HistoInRange", "", 50, 0, 10);
  	TH1F* xRadiator_HistoInSpacialRange = new TH1F("xRadiator_HistoInSpacialRange", "", 50, 0, 10);
  	TH1F* yRadiator_HistoInSpacialRange = new TH1F("yRadiator_HistoInSpacialRange", "", 50, 0, 10);
  	TH1F* xRadiator_HistoInAngularRange = new TH1F("xRadiator_HistoInAngularRange", "", 50, 0, 10);
  	TH1F* yRadiator_HistoInAngularRange = new TH1F("yRadiator_HistoInAngularRange", "", 50, 0, 10);
  	TH2F* xyRadiator_Histo = new TH2F("xyRadiator_Histo", "Before cuts", 50, 0, 10, 50, 0, 10);
  	TH2F* xyRadiator_HistoInRange = new TH2F("xyRadiator_HistoInRange", "After cuts", 50, 0, 10, 50, 0, 10);
  	//hit on Silicon detetcors
  	TH1F* x0Hit_Histo = new TH1F("x0Hit_Histo", "x Hit of UPPER Si", 50, 0, 10);
  	TH1F* x1Hit_Histo = new TH1F("x1Hit_Histo", "x Hit of LOWER Si", 50, 0, 10);
  	TH1F* y0Hit_Histo = new TH1F("y0Hit_Histo", "y Hit of UPPER Si", 50, 0, 10);
  	TH1F* y1Hit_Histo = new TH1F("y1Hit_Histo", "y Hit of LOWER Si", 50, 0, 10);
  	TH1F* x0Hit_HistoInRange = new TH1F("x0Hit_HistoInRange", "", 50, 0, 10);
  	TH1F* x1Hit_HistoInRange = new TH1F("x1Hit_HistoInRange", "", 50, 0, 10);
  	TH1F* y0Hit_HistoInRange = new TH1F("y0Hit_HistoInRange", "", 50, 0, 10);
  	TH1F* y1Hit_HistoInRange = new TH1F("y1Hit_HistoInRange", "", 50, 0, 10);
  	TH1F* x0Hit_HistoInAngularRange = new TH1F("x0Hit_HistoInAngularRange", "", 50, 0, 10);
  	TH1F* x1Hit_HistoInAngularRange = new TH1F("x1Hit_HistoInAngularRange", "", 50, 0, 10);
  	TH1F* y0Hit_HistoInAngularRange = new TH1F("y0Hit_HistoInAngularRange", "", 50, 0, 10);
  	TH1F* y1Hit_HistoInAngularRange = new TH1F("y1Hit_HistoInAngularRange", "", 50, 0, 10);
  	TH1F* x0Hit_HistoInSpacialRange = new TH1F("x0Hit_HistoInSpacialRange", "", 50, 0, 10);
  	TH1F* x1Hit_HistoInSpacialRange = new TH1F("x1Hit_HistoInSpacialRange", "", 50, 0, 10);
  	TH1F* y0Hit_HistoInSpacialRange = new TH1F("y0Hit_HistoInSpacialRange", "", 50, 0, 10);
  	TH1F* y1Hit_HistoInSpacialRange = new TH1F("y1Hit_HistoInSpacialRange", "", 50, 0, 10);
  	TH2F* xy0_Histo = new TH2F("xy0_Histo", "UPPER Si, before cuts", 50, 0, 10, 50, 0, 10);
  	TH2F* xy1_Histo = new TH2F("xy1_Histo", "LOWER Si, before cuts", 50, 0, 10, 50, 0, 10);
  	TH2F* xy0_HistoInRange = new TH2F("xy0_HistoInRange", "UPPER Si, after cuts", 50, 0, 10, 50, 0, 10);
  	TH2F* xy1_HistoInRange = new TH2F("xy1_HistoInRange", "LOWER Si, after cuts", 50, 0, 10, 50, 0, 10);
  	//theta
  	TH1F* theta_Histo = new TH1F("theta_Histo", "cos(#theta)", 50, 0.9, 1);
  	TH1F* theta_HistoInAngularRange = new TH1F("theta_HistoInAngularRange", "", 50, 0.9, 1);
  	//trigger
  	TH1F* trgUp_Histo  = new TH1F("trgUp_Histo","Scintillator UP",50,0,700);
  	TH1F* trgUp_HistoInSpacialRange  = new TH1F("trgUp_HistoInSpacialRange","",50,0,700);
  	TH1F* trgUp_HistoInAngularRange  = new TH1F("trgUp_HistoInAngularRange","",50,0,700);
  	TH1F* trgUp_HistoInRange  = new TH1F("trgUp_HistoInRange","",50,0,700);
  	TH1F* trgDown_Histo  = new TH1F("trgDown_Histo","Scintillator DOWN",50,0,700);
  	TH1F* trgDown_HistoInSpacialRange  = new TH1F("trgDown_HistoInSpacialRange","",50,0,700);
  	TH1F* trgDown_HistoInAngularRange  = new TH1F("trgDown_HistoInAngularRange","",50,0,700);
  	TH1F* trgDown_HistoInRange  = new TH1F("trgDown_HistoInRange","",50,0,700);
  	TH1F* trgDown_HistoOut = new TH1F("trgDown_HistoOut","",50,0,700);
  	TH1F* Dinode_Histo  = new TH1F("Dinode_Histo","Dinode",50,0,700);
  	TH1F* Dinode_HistoInSpacialRange  = new TH1F("Dinode_HistoInSpacialRange","",50,0,700);
  	TH1F* Dinode_HistoInAngularRange  = new TH1F("Dinode_HistoInAngularRange","",50,0,700);
  	TH1F* Dinode_HistoInRange  = new TH1F("Dinode_HistoInRange","",50,0,700);
  	TH1F* Dinode_HistoLower = new TH1F("Dinode_HistoLower","",30,0,200);
  	TH1F* Dinode_HistoUpper = new TH1F("Dinode_HistoUpper","",30,0,200);
  	TH1F* trgSignal_Histo  = new TH1F("trgSignal_Histo","Trigger Signal",50,0,700);
  	TH1F* trgSignal_HistoInSpacialRange  = new TH1F("trgSignal_HistoInSpacialRange","",50,0,700);
  	TH1F* trgSignal_HistoInAngularRange  = new TH1F("trgSignal_HistoInAngularRange","",50,0,700);
  	TH1F* trgSignal_HistoInRange  = new TH1F("trgSignal_HistoInRange","",50,0,700);
  	// Pmt pulse height
  	TH1F* PmtIntegratedPulseHeight_HistoInRange = new TH1F("PmtIntegratedPulseHeight_HistoinRange","",70,500,15000);
  	TH1F* PmtIntegratedPulseHeight_HistoLower = new TH1F("PmtIntegratedPulseHeight_HistoLower","",70,500,15000);
  	TH1F* PmtIntegratedPulseHeight_HistoUpper = new TH1F("PmtIntegratedPulseHeight_HistoUpper","",70,500,15000);
  	
	Double_t xRadiator, yRadiator, theta;
	Double_t xHit[nSiLayers], yHit[nSiLayers];
	Double_t PmtTime[nChannelsPmt];
	Double_t PulseHeight[nChannelsPmt];
	Int_t ChannelID[nChannelsPmt];
	Int_t DgtzID[nChannelsPmt];
	Double_t trgUp;
	Double_t trgDown;
	Double_t Dinode;
	intree->SetBranchAddress("DgtzID",DgtzID);
	intree->SetBranchAddress("PmtTime",PmtTime);
	intree->SetBranchAddress("xRadiator", &xRadiator);  
  	intree->SetBranchAddress("yRadiator", &yRadiator);  
        intree->SetBranchAddress("xHit", xHit);             
        intree->SetBranchAddress("yHit", yHit);             
    	intree->SetBranchAddress("theta", &theta);
	intree->SetBranchAddress("PmtPulseHeight",PulseHeight);  //TH1F* PmtPulseHeight_HistoLower[nChannelsPmt];
                                                               	 //TH1F* PmtPulseHeight_HistoUpper[nChannelsPmt];
	intree->SetBranchAddress("PmtChannelID",ChannelID);
	intree->SetBranchAddress("TrgUp",&trgUp);
	intree->SetBranchAddress("TrgDown",&trgDown);
	intree->SetBranchAddress("Dinode",&Dinode);

/*	for(Int_t iChannel = 0; iChannel < nChannelsPmt; iChannel++) {
	  PmtPulseHeight_HistoLower[iChannel] = new TH1F(Form("PmtPulseHeight_HistoLower_%i",iChannel),Form("ch %i Pulse Height [a.u.]",activeChannels[iChannel]),200,0,1000);
	  PmtPulseHeight_HistoLower[iChannel]->SetLineColor(kBlue+2);
	  PmtPulseHeight_HistoLower[iChannel]->SetFillColor(kBlue+2);
	  PmtPulseHeight_HistoUpper[iChannel] = new TH1F(Form("PmtPulseHeight_HistoUpper_%i",iChannel),Form("ch %i",activeChannels[iChannel]),200,0,1000);
	  PmtPulseHeight_HistoUpper[iChannel]->SetLineColor(kRed-7);
	  PmtPulseHeight_HistoUpper[iChannel]->SetFillColor(kRed-7);
	  
	}*/
  	
  	vector<Int_t> selectedEvents;
  	Int_t timeWindow_lowerBound;
	Int_t timeWindow_upperBound;
	
  	for (Int_t i = 0; i < intree->GetEntries(); ++i) {
		intree->GetEntry(i);
    		Int_t nChannelsInTime = 0;
    		
		for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
			if(DgtzID[iChannel]!=31) {
				timeWindow_lowerBound=80;
				timeWindow_upperBound=120;
			} else if(DgtzID[iChannel]==31) {
				timeWindow_lowerBound=190;
				timeWindow_upperBound=230;
			}
			if( PmtTime[iChannel]>=timeWindow_lowerBound &&PmtTime[iChannel]<=timeWindow_upperBound ) nChannelsInTime += 1;
			//If all signals are in time then AllSignalsInTime is = number of PMT channels
		}
		// CUT on time of PMT signal 
    		if( nChannelsInTime==nChannelsPmt ) {
    			x0Hit_Histo->Fill(xHit[0]);
    			x1Hit_Histo->Fill(xHit[1]);
    			y0Hit_Histo->Fill(yHit[0]);
    			y1Hit_Histo->Fill(yHit[1]);	
    			xy0_Histo->Fill(xHit[0],yHit[0]);
    			xy1_Histo->Fill(xHit[1],yHit[1]);
    			xRadiator_Histo->Fill(xRadiator);
    			yRadiator_Histo->Fill(yRadiator);
    			xyRadiator_Histo->Fill(xRadiator,yRadiator);
    			theta_Histo->Fill(theta);
    			trgUp_Histo->Fill(trgUp);
    			trgDown_Histo->Fill(trgDown);
    			Dinode_Histo->Fill(Dinode);
    			trgSignal_Histo->Fill(trgDown+Dinode);
    			// CUT on tracks direction
    			if (theta > thr_theta) {
    				x0Hit_HistoInAngularRange->Fill(xHit[0]);
    				x1Hit_HistoInAngularRange->Fill(xHit[1]);
    				y0Hit_HistoInAngularRange->Fill(yHit[0]);
    				y1Hit_HistoInAngularRange->Fill(yHit[1]);
    				xRadiator_HistoInAngularRange->Fill(xRadiator);
    				yRadiator_HistoInAngularRange->Fill(yRadiator);
    				theta_HistoInAngularRange->Fill(theta);
    				trgUp_HistoInAngularRange->Fill(trgUp);
    				trgDown_HistoInAngularRange->Fill(trgDown);
    				Dinode_HistoInAngularRange->Fill(Dinode);
    				trgSignal_HistoInAngularRange->Fill(trgDown+Dinode);
    			}
    			// CUT on spacial distribution of hits
    			if ( (pow((xcenterRadiator - xRadiator),2)+pow((ycenterRadiator - yRadiator),2))<=pow(thr_Radiator,2) && abs(4.5 - xHit[0])<thr_x0 && abs(4.5 - yHit[0])<thr_y0 ) {
	   		     	x0Hit_HistoInSpacialRange->Fill(xHit[0]);
    				x1Hit_HistoInSpacialRange->Fill(xHit[1]);
    				y0Hit_HistoInSpacialRange->Fill(yHit[0]);
    				y1Hit_HistoInSpacialRange->Fill(yHit[1]);
	   			xRadiator_HistoInSpacialRange->Fill(xRadiator);
	   			yRadiator_HistoInSpacialRange->Fill(yRadiator);
	   			trgUp_HistoInSpacialRange->Fill(trgUp);
	   			trgDown_HistoInSpacialRange->Fill(trgDown);
    				Dinode_HistoInSpacialRange->Fill(Dinode);
    				trgSignal_HistoInSpacialRange->Fill(trgDown+Dinode);
			}
    			// CUT on track direction & spacial distribution of hits
    			if (theta > thr_theta && (pow((xcenterRadiator - xRadiator),2)+pow((ycenterRadiator - yRadiator),2))<=pow(thr_Radiator,2) && abs(5 - xHit[0]) < thr_x0 && abs(5 - yHit[0]) < thr_y0 ) {
	   			x0Hit_HistoInRange->Fill(xHit[0]);
    				x1Hit_HistoInRange->Fill(xHit[1]);
    				y0Hit_HistoInRange->Fill(yHit[0]);
    				y1Hit_HistoInRange->Fill(yHit[1]);
    				xy0_HistoInRange->Fill(xHit[0],yHit[0]);
    				xy1_HistoInRange->Fill(xHit[1],yHit[1]);
	   			xRadiator_HistoInRange->Fill(xRadiator);
	   			yRadiator_HistoInRange->Fill(yRadiator);
	   			xyRadiator_HistoInRange->Fill(xRadiator,yRadiator);
	   			trgUp_HistoInRange->Fill(trgUp);
	   			trgDown_HistoInRange->Fill(trgDown);
    				Dinode_HistoInRange->Fill(Dinode);
    				trgSignal_HistoInRange->Fill(trgDown+Dinode);
	   			selectedEvents.push_back(i+1);
				Double_t IntegratedSignal=0;
	   			for(Int_t ChId = 0; ChId<nChannelsPmt; ChId++) {
			      		if(DgtzID[ChId] == 31) PulseHeight[ChId]/=4;
			      		IntegratedSignal+=PulseHeight[ChId];
			    	}
			    	PmtIntegratedPulseHeight_HistoInRange->Fill(IntegratedSignal);
			}
			// Trying to find some kind of background on the Pmt channels
    			if ( (pow((xcenterRadiator - xRadiator),2)+pow((ycenterRadiator - yRadiator),2)) >= pow(thr_Radiator,2)) {
    				if( yRadiator < ycenterRadiator && yHit[0] < 1.0 ) {
    					Dinode_HistoLower->Fill(Dinode);
    					Double_t IntegratedSignal=0;
    					for(Int_t ChId = 0; ChId<nChannelsPmt; ChId++) {
			      			if(DgtzID[ChId] == 31) PulseHeight[ChId]/=4;
			      			//PmtPulseHeight_HistoLower[ChId]->Fill(PulseHeight[ChId]);
			      			IntegratedSignal+=PulseHeight[ChId];
			    		}
			    		PmtIntegratedPulseHeight_HistoLower->Fill(IntegratedSignal);
    				} else if(yRadiator > ycenterRadiator && yHit[0] > 8.0 ) {
    					Dinode_HistoUpper->Fill(Dinode);
    					Double_t IntegratedSignal=0;
    					for(Int_t ChId = 0; ChId<nChannelsPmt; ChId++) {
			      			if(DgtzID[ChId] == 31) PulseHeight[ChId]/=4;
			      			//PmtPulseHeight_HistoUpper[ChId]->Fill(PulseHeight[ChId]);
			      			IntegratedSignal+=PulseHeight[ChId];
			    		}
			    		PmtIntegratedPulseHeight_HistoUpper->Fill(IntegratedSignal);
    				}
    			}		
		}
	}
	
	cout << "Selected events = " << selectedEvents.size() << endl;
	ofstream selectedEvents_fout(Form("run%i_selectedEvents_thetaThr%1.1f_RadiatorThr%1.1f_xUpperSiThr%1.1f.txt",
					   SiRunNumber,thr_theta,thr_Radiator,thr_x0));
	
	for(Int_t iEv=0; iEv<selectedEvents.size(); ++iEv) {
		selectedEvents_fout << selectedEvents[iEv] << endl;
	}
	
	gStyle->SetPalette(kCherry);
	TColor::InvertPalette();
	//In this Canvas I plot the distribution of the muons on the xy plane of the radiator provided some filters
  	TCanvas* c = new TCanvas("c","",750,750);
  	c->Divide(2,2);
	c->cd(1);
	xRadiator_Histo->SetStats(0);
	xRadiator_Histo->SetLineColor(kBlack);
  	xRadiator_Histo->Draw();
  	xRadiator_HistoInSpacialRange->SetLineColor(kBlue);
  	xRadiator_HistoInSpacialRange->Draw("same");
  	xRadiator_HistoInAngularRange->SetLineColor(kGreen);
  	xRadiator_HistoInAngularRange->Draw("same");
  	xRadiator_HistoInRange->SetLineColor(kRed);
  	xRadiator_HistoInRange->Draw("same");
	c->cd(2);
  	yRadiator_Histo->SetStats(0);
	yRadiator_Histo->SetLineColor(kBlack);
  	yRadiator_Histo->Draw();
  	yRadiator_HistoInSpacialRange->SetLineColor(kBlue);
  	yRadiator_HistoInSpacialRange->Draw("same");
  	yRadiator_HistoInAngularRange->SetLineColor(kGreen);
  	yRadiator_HistoInAngularRange->Draw("same");
  	yRadiator_HistoInRange->SetLineColor(kRed);
  	yRadiator_HistoInRange->Draw("same");
	c->cd(3);
	xyRadiator_Histo->SetStats(0);
  	xyRadiator_Histo->Draw("colz");
	c->cd(4);
	xyRadiator_HistoInRange->SetStats(0);
  	xyRadiator_HistoInRange->Draw("colz");
	/*theta_Histo->SetStats(0);
	theta_Histo->SetLineColor(kBlack);
  	theta_Histo->Draw();
  	theta_HistoInAngularRange->SetLineColor(kGreen);
  	theta_HistoInAngularRange->Draw("same");*/
  
  	//In this canvas I plot selected hit on the UPPER and LOWER SI
  	TCanvas* c1 = new TCanvas("c1","",750,750);
  	c1->Divide(2,2);
	c1->cd(1);
	x0Hit_Histo->SetStats(0);
	x0Hit_Histo->SetLineColor(kBlack);
	x0Hit_Histo->Draw();
	x0Hit_HistoInSpacialRange->SetLineColor(kBlue);
  	x0Hit_HistoInSpacialRange->Draw("same");
  	x0Hit_HistoInAngularRange->SetLineColor(kGreen);
  	x0Hit_HistoInAngularRange->Draw("same");
  	x0Hit_HistoInRange->SetLineColor(kRed);
  	x0Hit_HistoInRange->Draw("same");
  	c1->cd(2);
  	y0Hit_Histo->SetStats(0);
	y0Hit_Histo->SetLineColor(kBlack);
	y0Hit_Histo->Draw();
	y0Hit_HistoInSpacialRange->SetLineColor(kBlue);
  	y0Hit_HistoInSpacialRange->Draw("same");
  	y0Hit_HistoInAngularRange->SetLineColor(kGreen);
  	y0Hit_HistoInAngularRange->Draw("same");
  	y0Hit_HistoInRange->SetLineColor(kRed);
  	y0Hit_HistoInRange->Draw("same");
  	c1->cd(3);
  	x1Hit_Histo->SetStats(0);
	x1Hit_Histo->SetLineColor(kBlack);
	x1Hit_Histo->Draw();
	x1Hit_HistoInSpacialRange->SetLineColor(kBlue);
  	x1Hit_HistoInSpacialRange->Draw("same");
  	x1Hit_HistoInAngularRange->SetLineColor(kGreen);
  	x1Hit_HistoInAngularRange->Draw("same");
  	x1Hit_HistoInRange->SetLineColor(kRed);
  	x1Hit_HistoInRange->Draw("same");
  	c1->cd(4);
  	y1Hit_Histo->SetStats(0);
	y1Hit_Histo->SetLineColor(kBlack);
	y1Hit_Histo->Draw();
	y1Hit_HistoInSpacialRange->SetLineColor(kBlue);
  	y1Hit_HistoInSpacialRange->Draw("same");
  	y1Hit_HistoInAngularRange->SetLineColor(kGreen);
  	y1Hit_HistoInAngularRange->Draw("same");
  	y1Hit_HistoInRange->SetLineColor(kRed);
  	y1Hit_HistoInRange->Draw("same");
  	
  	TCanvas* c2 = new TCanvas("c2","",750,750);
  	c2->Divide(2,2);
	c2->cd(1);
	xy0_Histo->SetStats(0);
	xy0_Histo->Draw("colz");
	c2->cd(2);
	xy0_HistoInRange->SetStats(0);
	xy0_HistoInRange->Draw("colz");
	c2->cd(3);
	xy1_Histo->SetStats(0);
	xy1_Histo->Draw("colz");
	c2->cd(4);
	xy1_HistoInRange->SetStats(0);
	xy1_HistoInRange->Draw("colz");
  	/*intree->Draw("yHit[0]:xHit[0]");
	c2->cd(2);
	intree->Draw("yHit[1]:xHit[1]");
	c2->cd(3);
	intree->Draw("yRadiator:xRadiator");*/


	TCanvas* c3 = new TCanvas("c3", "", 750, 750);
	c3->Divide(2,2);
	c3->cd(1);
	x0Hit_Histo->Draw();
	c3->cd(2);
	y0Hit_Histo->Draw();
	c3->cd(3);
	x1Hit_Histo->Draw();
	c3->cd(4);
	y1Hit_Histo->Draw();

	TCanvas* c4 = new TCanvas("c4","",750,750);
  	c4->Divide(2,2);
  	c4->cd(1);
  	c4->cd(1)->SetLogy();
  	trgUp_Histo->SetStats(0);
	trgUp_Histo->SetLineColor(kBlack);
	trgUp_Histo->Draw();
	trgUp_HistoInSpacialRange->SetLineColor(kBlue);
  	trgUp_HistoInSpacialRange->Draw("same");
  	trgUp_HistoInAngularRange->SetLineColor(kGreen);
  	trgUp_HistoInAngularRange->Draw("same");
  	trgUp_HistoInRange->SetLineColor(kRed);
  	trgUp_HistoInRange->Draw("same");
	c4->cd(2);
	c4->cd(2)->SetLogy();
	trgDown_Histo->SetStats(0);
	trgDown_Histo->SetLineColor(kBlack);
	trgDown_Histo->Draw();
	trgDown_HistoInSpacialRange->SetLineColor(kBlue);
  	trgDown_HistoInSpacialRange->Draw("same");
  	trgDown_HistoInAngularRange->SetLineColor(kGreen);
  	trgDown_HistoInAngularRange->Draw("same");
  	trgDown_HistoInRange->SetLineColor(kRed);
  	trgDown_HistoInRange->Draw("same");
  	c4->cd(3);
  	c4->cd(3)->SetLogy();
  	Dinode_Histo->SetStats(0);
	Dinode_Histo->SetLineColor(kBlack);
	Dinode_Histo->Draw();
	Dinode_HistoInSpacialRange->SetLineColor(kBlue);
  	Dinode_HistoInSpacialRange->Draw("same");
  	Dinode_HistoInAngularRange->SetLineColor(kGreen);
  	Dinode_HistoInAngularRange->Draw("same");
  	Dinode_HistoInRange->SetLineColor(kRed);
  	Dinode_HistoInRange->Draw("same");
  	c4->cd(4);
  	c4->cd(4)->SetLogy();
  	trgSignal_Histo->SetStats(0);
	trgSignal_Histo->SetLineColor(kBlack);
	trgSignal_Histo->Draw();
	trgSignal_HistoInSpacialRange->SetLineColor(kBlue);
  	trgSignal_HistoInSpacialRange->Draw("same");
  	trgSignal_HistoInAngularRange->SetLineColor(kGreen);
  	trgSignal_HistoInAngularRange->Draw("same");
  	trgSignal_HistoInRange->SetLineColor(kRed);
  	trgSignal_HistoInRange->Draw("same");

	TCanvas* c5 = new TCanvas("c5","",375,750);
	c5->Divide(1,2);
	c5->cd(1);
	Dinode_HistoLower->Scale(1.0/Dinode_HistoLower->Integral());
	Dinode_HistoLower->GetYaxis()->SetRangeUser(0.,0.2);
	Dinode_HistoLower->SetStats(0);
	Dinode_HistoLower->SetLineColor(kBlue+3);
	Dinode_HistoLower->SetFillColor(kBlue+3);
	Dinode_HistoLower->SetFillStyle(3004);
  	Dinode_HistoLower->Draw("HIST");
  	Dinode_HistoUpper->Scale(1.0/Dinode_HistoUpper->Integral());
	Dinode_HistoUpper->SetLineColor(kBlue-8);
	Dinode_HistoUpper->SetFillColor(kBlue-8);
	Dinode_HistoUpper->SetFillStyle(3005);
  	Dinode_HistoUpper->Draw("SAME HIST");
  	c5->cd(2);
	PmtIntegratedPulseHeight_HistoInRange->Scale(1.0/PmtIntegratedPulseHeight_HistoInRange->Integral());
	PmtIntegratedPulseHeight_HistoInRange->GetYaxis()->SetRangeUser(0.,0.2);
	PmtIntegratedPulseHeight_HistoInRange->SetStats(0);
	PmtIntegratedPulseHeight_HistoInRange->SetLineColor(kRed-10);
  	PmtIntegratedPulseHeight_HistoInRange->SetFillColor(kRed-10);
  	//PmtIntegratedPulseHeight_HistoInRange->SetFillStyle(3003);
  	PmtIntegratedPulseHeight_HistoInRange->Draw("HIST");
  	PmtIntegratedPulseHeight_HistoLower->Scale(1.0/PmtIntegratedPulseHeight_HistoLower->Integral());
  	PmtIntegratedPulseHeight_HistoLower->SetLineColor(kBlue+3);
  	PmtIntegratedPulseHeight_HistoLower->SetFillColor(kBlue+3);
  	PmtIntegratedPulseHeight_HistoLower->SetFillStyle(3004);
  	PmtIntegratedPulseHeight_HistoLower->Draw("SAME HIST");
  	PmtIntegratedPulseHeight_HistoUpper->Scale(1.0/PmtIntegratedPulseHeight_HistoUpper->Integral());
  	PmtIntegratedPulseHeight_HistoUpper->SetLineColor(kBlue-8);
  	PmtIntegratedPulseHeight_HistoUpper->SetFillColor(kBlue-8);
  	PmtIntegratedPulseHeight_HistoUpper->SetFillStyle(3005);
  	PmtIntegratedPulseHeight_HistoUpper->Draw("SAME HIST");
  	
  	
  	/*
	TCanvas* c_ph = new TCanvas("c_ph","",1500,750);
	TPaveText* pt[nChannelsPmt];
	c_ph->Divide(7,4);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
	  c_ph->cd(iChannel+1);
	  //c_ph->cd(iChannel+1)->SetLogy();
	  PmtPulseHeight_HistoLower[iChannel]->SetStats(0);
	  //PmtPulseHeight_Histo[iChannel]->GetXaxis()->SetTitle("PH [a.u.]");
	  if(iChannel<8 || iChannel>23)PmtPulseHeight_HistoLower[iChannel]->GetXaxis()->SetRangeUser(0,300);
	  PmtPulseHeight_HistoLower[iChannel]->GetXaxis()->SetNdivisions(505);
	  PmtPulseHeight_HistoLower[iChannel]->GetXaxis()->SetLabelFont(42);
	  PmtPulseHeight_HistoLower[iChannel]->GetXaxis()->SetLabelSize(0.06);
	  PmtPulseHeight_HistoLower[iChannel]->GetYaxis()->SetNdivisions(505);
	  PmtPulseHeight_HistoLower[iChannel]->GetYaxis()->SetLabelFont(42);
	  PmtPulseHeight_HistoLower[iChannel]->GetYaxis()->SetLabelSize(0.06);
	  PmtPulseHeight_HistoLower[iChannel]->Draw();
	  PmtPulseHeight_HistoUpper[iChannel]->SetStats(0);
	  PmtPulseHeight_HistoUpper[iChannel]->Draw("SAME");
	  pt[iChannel] = new TPaveText(0.23,0.65,0.99,0.86,"blNDC");
	  pt[iChannel]->SetBorderSize(1);
	  pt[iChannel]->SetLineColor(kBlack);
	  pt[iChannel]->SetFillColor(kWhite);
	  pt[iChannel]->SetTextFont(42);
	  pt[iChannel]->SetTextSize(0.07);
	  pt[iChannel]->AddText(Form("Selected events: %2.1f%%", PmtPulseHeight_HistoInTime[iChannel]->GetEntries()*1.0/PmtPulseHeight_Histo[iChannel]->GetEntries()*100));
	  pt[iChannel]->Draw();
	}*/
}

//\\//\\//\\//\\// SHOWPMTSIGNAL //\\//\\//\\//\\//\\//\\//
// Show the PMT signals of a particular events in a 2d histograms
void ShowPmtSignal(Int_t SiRunNumber, Int_t evNumber) {
	
	TString infile_name = Form("run%i.root",SiRunNumber);
	TFile* infile = new TFile(infile_name, "READ");
  	if(infile->IsOpen()) printf("File opened successfully\n");
  
	TTree* intree = (TTree*)infile->Get("Cherenkov");
	
	Double_t PmtPulseHeight[nChannelsPmt];    intree->SetBranchAddress("PmtPulseHeight",PmtPulseHeight);  
	
	TH2F* h_PmtPulseHeight = new TH2F("h_PmtPulseHeight", Form("Pmt Signal, event %i of run %i",evNumber,SiRunNumber),8,0,8,8,0,8);
	
	intree->GetEntry(evNumber-1);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
	   h_PmtPulseHeight->SetBinContent(xBin[iChannel],yBin[iChannel], PmtPulseHeight[iChannel]);
	}
	
		
	TLine* xLine[7];
	TLine* yLine[7];
	for( Int_t i=0; i<7; ++i) {
		xLine[i] = new TLine(0,i+1,8,i+1);
		yLine[i] = new TLine(i+1,0,i+1,8);
	}
	
	TCanvas* c = new TCanvas("c","",750,750);
	h_PmtPulseHeight->SetStats(0);
	h_PmtPulseHeight->Draw("colz");
	for(Int_t i=0; i<7; ++i) {
		xLine[i]->Draw("SAME");
		yLine[i]->Draw("SAME");
	}
}


void PrintEventOnFile(Int_t SiRunNumber, Int_t EvNumber) {

  TString infile_name = Form("run%i.root",SiRunNumber);
  TFile* infile = new TFile(infile_name, "READ");
  if(infile->IsOpen()) printf("File opened successfully\n");
  
  TTree* intree = (TTree*)infile->Get("Cherenkov");
  
  Double_t PmtPulseHeight[nChannelsPmt];    intree->SetBranchAddress("PmtPulseHeight",PmtPulseHeight);  
  Double_t xHit[nSiLayers];                 intree->SetBranchAddress("xHit", xHit);
  Double_t yHit[nSiLayers];                 intree->SetBranchAddress("yHit", yHit);
  Double_t zHit[nSiLayers];                 intree->SetBranchAddress("z_yHit", zHit);
  Double_t xRadiator      ;                 intree->SetBranchAddress("xRadiator", &xRadiator);
  Double_t yRadiator      ;                 intree->SetBranchAddress("yRadiator", &yRadiator);
  Double_t zRadiator      ;                 intree->SetBranchAddress("zRadiator", &zRadiator);

  Double_t x0_on_y0, x1_on_y1;
  
  //Print the event information on file
  ofstream outchannels, outhit;
  outchannels.open("hitchannels.txt");
  outhit.open("trace.txt");

  intree->GetEntry(EvNumber-1);
  
  for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
    if (PmtPulseHeight[iChannel] > 0.0) {
      outchannels << activeChannels[iChannel] << " ";
    }
  }

  x0_on_y0 = xHit[0] + (xHit[1] - xHit[0])/Sidistx*1.9; //projection of xHit[0] on the yHit[0] plane (below)
  x1_on_y1 = xHit[1] - (xHit[1] - xHit[0])/Sidistx*1.9; //projection of xHit[1] on the yHit[1] plane (above)
  
  outhit << x0_on_y0   << " " << yHit[0]   << " " << -zHit[0]   << endl;
  outhit << x1_on_y1   << " " << yHit[1]   << " " << -zHit[1]   << endl;
  
  for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
    if (PmtPulseHeight[iChannel] > 150.0) {
      outchannels << activeChannels[iChannel] << " ";
    }
  }
  
  outhit << xHit[0]   << " " << yHit[0]   << " " << -zHit[0]   << endl;
  outhit << xHit[1]   << " " << yHit[1]   << " " << -zHit[1]   << endl;
  outhit << xRadiator << " " << yRadiator << " " << -zRadiator << endl;
  
  outchannels.close();
  outhit.close();

  return;

}



void FillEvHisto(TTree* intree, Int_t ev, TH2F* h2_Dgtz20, TH2F* h2_Dgtz25, TH2F* h2_Dgtz31, TH2F* h2_all, Int_t norm) {
 
  Double_t PmtPulseHeight[nChannelsPmt];    intree->SetBranchAddress("PmtPulseHeight",PmtPulseHeight);
  Int_t DgtzID[nChannelsPmt];               intree->SetBranchAddress("DgtzID",DgtzID);
  Double_t BinContent = 0.0; 

  Int_t ChannelID = 0;
  Int_t Max20 = 0; 
  Int_t Max25 = 0;
  Int_t Max31 = 0;
  
  intree->GetEntry(ev-1);

  if (norm == 1) {
    for(Int_t ChannelID = 0; ChannelID<nChannelsPmt; ++ChannelID) {	
      if (DgtzID[ChannelID] == 20) {
	if (PmtPulseHeight[ChannelID] > Max20) {
	  Max20 = PmtPulseHeight[ChannelID];
	}
      }
      else if(DgtzID[ChannelID] == 25) {
	if (PmtPulseHeight[ChannelID] > Max25) {
	  Max25 = PmtPulseHeight[ChannelID];
	}
      }
      
      else if(DgtzID[ChannelID] == 31) {
	if (PmtPulseHeight[ChannelID] > Max31) {
	  Max31 = PmtPulseHeight[ChannelID];
	}
      }
    }
  }

  else if(norm == 0) {
    Max20 = 1;
    Max25 = 1;
    Max31 = 1;
  }
  
  for(Int_t ChannelID = 0; ChannelID<nChannelsPmt; ++ChannelID) {
    if (DgtzID[ChannelID] == 20) {
      BinContent = h2_Dgtz20->GetBinContent(xBin[ChannelID], yBin[ChannelID]);

      h2_Dgtz20->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]*1.0/Max20);
      h2_all->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]*1.0/Max20);

      h2_Dgtz20->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]/Max20);
      h2_all->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]/Max20);
    }
    
    else if(DgtzID[ChannelID] == 25) {
      BinContent = h2_Dgtz25->GetBinContent(xBin[ChannelID], yBin[ChannelID]);
      h2_Dgtz25->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]*1.0/Max25);
      h2_all->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]*1.0/Max25);
    }
    else if(DgtzID[ChannelID] == 31) {
      BinContent = h2_Dgtz31->GetBinContent(xBin[ChannelID], yBin[ChannelID]);
      h2_Dgtz31->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]*1.0/Max31);
      h2_all->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]*1.0/Max31);
      h2_Dgtz25->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]/Max25);
      h2_all->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]/Max25);
    }
    else if(DgtzID[ChannelID] == 31) {
      BinContent = h2_Dgtz31->GetBinContent(xBin[ChannelID], yBin[ChannelID]);
      h2_Dgtz31->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]/Max31);
      h2_all->SetBinContent(xBin[ChannelID], yBin[ChannelID], BinContent+PmtPulseHeight[ChannelID]/Max31);
    }
    
  }

  if (norm == 1) {
    cout << "   Max20 = " << Max20 << endl;
    cout << "   Max25 = " << Max25 << endl;
    cout << "   Max31 = " << Max31 << endl;
  }
  
  Max20 = 0;
  Max25 = 0;
  Max31 = 0;
 
  
  return;
  
}



/*****Plot the 2D histogram with the PMT active channel
 * mod = 0: Plot all the events
 * mod = 1: Plot only the selected events (from file)
 */

void CumPmtSignal(Int_t SiRunNumber, Int_t mod, Int_t ev) {

  TH2F* h2_Dgtz20 = new TH2F("h2_Dgtz20", "Signal Dgtz20", 8, 0, 8, 8, 0, 8);
  TH2F* h2_Dgtz25 = new TH2F("h2_Dgtz25", "Signal Dgtz25", 8, 0, 8, 8, 0, 8);
  TH2F* h2_Dgtz31 = new TH2F("h2_Dgtz31", "Signal Dgtz31", 8, 0, 8, 8, 0, 8);
  TH2F* h2_all    = new TH2F("h2_all", "Signal All", 8, 0, 8, 8, 0, 8);
  TH2F* h2_Dgtz20_norm = new TH2F("h2_Dgtz20_norm", "Signal Dgtz20 - Normalized", 8, 0, 8, 8, 0, 8);
  TH2F* h2_Dgtz25_norm = new TH2F("h2_Dgtz25_norm", "Signal Dgtz25 - Normalized", 8, 0, 8, 8, 0, 8);
  TH2F* h2_Dgtz31_norm = new TH2F("h2_Dgtz31_norm", "Signal Dgtz31 - Normalized", 8, 0, 8, 8, 0, 8);
  TH2F* h2_all_norm    = new TH2F("h2_all_norm   ", "Signal All    - Normalized", 8, 0, 8, 8, 0, 8);
  

  TString infile_name = Form("run%i.root",SiRunNumber);
  TFile* infile = new TFile(infile_name, "READ");
  if(infile->IsOpen()) printf("File opened successfully\n");
  
  TTree* intree = (TTree*)infile->Get("Cherenkov");
 
  if (mod == 0 /*choose from file*/) {   
    ifstream fin;
    fin.open("Selected.txt");
    
    while(fin >> ev) {
      cout << "Opening event: " <<  ev << endl;
      FillEvHisto(intree, ev, h2_Dgtz20,  h2_Dgtz25,  h2_Dgtz31,  h2_all, 0);
      FillEvHisto(intree, ev, h2_Dgtz20_norm,  h2_Dgtz25_norm,  h2_Dgtz31_norm,  h2_all_norm, 1);
    }
    
    
    fin.close();
  }
  
  else if (mod == 1 /*Choose all the events*/) {
    for(ev = 0; ev<intree->GetEntries(); ev++){
      cout << "Opening event: " <<  ev << endl;
      FillEvHisto(intree, ev,  h2_Dgtz20,  h2_Dgtz25,  h2_Dgtz31,  h2_all, 0);
      FillEvHisto(intree, ev, h2_Dgtz20_norm,  h2_Dgtz25_norm,  h2_Dgtz31_norm,  h2_all_norm, 1);
    }
  }

  else if (mod == 2 /*Choose a specific event*/) {
    cout << "Opening event: " <<  ev << endl;
    FillEvHisto(intree, ev,  h2_Dgtz20,  h2_Dgtz25,  h2_Dgtz31,  h2_all, 0);
    FillEvHisto(intree, ev, h2_Dgtz20_norm,  h2_Dgtz25_norm,  h2_Dgtz31_norm,  h2_all_norm, 1);
  }
  
 
  
  TCanvas* c1 = new TCanvas("c1", "c1", 750, 750);
  c1->Divide(2,2);
  c1->cd(1);
  h2_Dgtz20->Draw("Colz");
  c1->cd(2);
  h2_Dgtz25->Draw("Colz");
  c1->cd(3);
  h2_Dgtz31->Draw("Colz");
  c1->cd(4);
  h2_all->Draw("Colz");

  TCanvas* c2 = new TCanvas("c2", "c2", 750, 750);
  c2->Divide(2,2);
  c2->cd(1);
  h2_Dgtz20_norm->Draw("Colz");
  c2->cd(2);
  h2_Dgtz25_norm->Draw("Colz");
  c2->cd(3);
  h2_Dgtz31_norm->Draw("Colz");
  c2->cd(4);
  h2_all_norm->Draw("Colz");



  return;

}
