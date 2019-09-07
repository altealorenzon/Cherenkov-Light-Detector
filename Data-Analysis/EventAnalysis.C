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
const Int_t nChannelsPmt = 8; // or 26
const Int_t activeChannels[26] = {21,22,29,30,37,38,45,46,5,6,13,14,53,54,61,62,40,39,32,31,24,23,16,15,7,8};
//(x,y) bin numbers corresponding to the active channels of the Pmt
//			21 22 29 30 37 38 45 46  5  6 13 14 53 54 61 62 40 39 32 31 24 23 16 15  7  8
const Int_t xBin[26] = { 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 5, 6, 8, 7, 8, 7, 8, 7, 8, 7, 7, 8};
const Int_t yBin[26] = { 6, 6, 5, 5, 4, 4, 3, 3, 8, 8, 7, 7, 2, 2, 1, 1, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8};

//here you can set the time window
const Int_t timeWindow_lowerBound = 80;
const Int_t timeWindow_upperBound = 120;

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

void RunStatsPmt();
void xyrad_histo();
void ShowPmtSignal();
//\\//\\//\\//\\// RUNSTATSPMT //\\//\\//\\//\\//\\//\\//
//See the time spectrum and PH spectrum of PMT active channels and select events in a particular time window
void RunStatsPmt(Int_t SiRunNumber) {
	
	TString infile_name = Form("run%i.root",SiRunNumber);
	TFile* infile = new TFile(infile_name,"READ");
  	if(infile->IsOpen()) printf("File opened successfully\n");
	TTree* intree = (TTree*)infile->Get("Cherenkov");
	
	TH1F* PmtTime_Histo[nChannelsPmt];
	TH1F* PmtPulseHeight_Histo[nChannelsPmt];
	TH1F* PmtPulseHeight_HistoInTime[nChannelsPmt];
	
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		PmtTime_Histo[iChannel] = new TH1F(Form("PmtTime_%i",iChannel),Form("ch %i Time [a.u.]",activeChannels[iChannel]),50,50,300);
		PmtTime_Histo[iChannel]->SetFillColor(kAzure-8);
		PmtTime_Histo[iChannel]->SetLineColor(kAzure-8);
		PmtPulseHeight_Histo[iChannel] = new TH1F(Form("PmtPulseHeight_Histo_%i",iChannel),Form("ch %i Pulse Height [a.u.]",activeChannels[iChannel]),50,0,500);
		PmtPulseHeight_Histo[iChannel]->SetLineColor(kBlue+2);
		PmtPulseHeight_Histo[iChannel]->SetFillColor(kBlue+2);
		PmtPulseHeight_HistoInTime[iChannel] = new TH1F(Form("PmtPulseHeight_HistoInTime_%i",iChannel),Form("ch %i",activeChannels[iChannel]),50,0,500);
		PmtPulseHeight_HistoInTime[iChannel]->SetLineColor(kRed-7);
		PmtPulseHeight_HistoInTime[iChannel]->SetFillColor(kRed-7);
	}
	
	Ev_t Ev;
	Int_t evNumber;     				
	intree->SetBranchAddress("evNumber",&evNumber);
	intree->SetBranchAddress("PmtTime",Ev.PmtSignal.Time);
	intree->SetBranchAddress("PmtPulseHeight",Ev.PmtSignal.PulseHeight);
	intree->SetBranchAddress("PmtChannelID",Ev.PmtSignal.ChannelID);
	
	Int_t nEntries = intree->GetEntries();
	
	for(Int_t iEntry=0; iEntry<nEntries; ++iEntry) {
		intree->GetEntry(iEntry);
		Int_t chCounter=0;
		for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
			PmtTime_Histo[iChannel]->Fill(Ev.PmtSignal.Time[iChannel]);
			PmtPulseHeight_Histo[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]);
			if(Ev.PmtSignal.Time[iChannel]>=timeWindow_lowerBound && Ev.PmtSignal.Time[iChannel]<=timeWindow_upperBound ) {
				PmtPulseHeight_HistoInTime[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]);
				if(Ev.PmtSignal.PulseHeight[iChannel]>=150) ++chCounter;
			}
		}
		if(chCounter>=4) cout << "High PH in " << chCounter << " channels. Event number = " << evNumber << endl;
	}
	
	TLine* line_lowerBound[nChannelsPmt]; 
	TLine* line_upperBound[nChannelsPmt];
	
	TCanvas* c_time = new TCanvas("c_time","",1500,750);
	Int_t nLines=2;
	if(nChannelsPmt==26) nLines=4;
	c_time->Divide(nChannelsPmt/2,nLines);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		c_time->cd(iChannel+1);
		PmtTime_Histo[iChannel]->SetStats(0);
		//PmtTime_Histo[iChannel]->GetXaxis()->SetTitle("Time [a.u.]");
		PmtTime_Histo[iChannel]->Draw();
		line_lowerBound[iChannel] = new TLine(timeWindow_lowerBound,0,timeWindow_lowerBound,PmtTime_Histo[iChannel]->GetBinContent(PmtTime_Histo[iChannel]->GetMaximumBin()));
		line_lowerBound[iChannel]->SetLineColor(kBlack);
		line_lowerBound[iChannel]->Draw("SAME");
		line_upperBound[iChannel] = new TLine(timeWindow_upperBound,0,timeWindow_upperBound,PmtTime_Histo[iChannel]->GetBinContent(PmtTime_Histo[iChannel]->GetMaximumBin()));
		line_upperBound[iChannel]->SetLineColor(kBlack);
		line_upperBound[iChannel]->Draw("SAME");
	}
	
	TCanvas* c_ph = new TCanvas("c_ph","",1500,750);
	TPaveText* pt[nChannelsPmt];
	c_ph->Divide(nChannelsPmt/2,nLines);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		c_ph->cd(iChannel+1);
		//c_ph->cd(iChannel+1)->SetLogy();
		PmtPulseHeight_Histo[iChannel]->SetStats(0);
		//PmtPulseHeight_Histo[iChannel]->GetXaxis()->SetTitle("PH [a.u.]");
		PmtPulseHeight_Histo[iChannel]->SetTitleSize(21);
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
		 Double_t thr_xRadiator,
		 Double_t thr_yRadiator,
		 Double_t thr_x0,
		 Double_t thr_y0) {

	TString infile_name = Form("run%i.root",SiRunNumber);
	TFile* infile = new TFile(infile_name, "READ");
  	if(infile->IsOpen()) printf("File opened successfully\n");
  
	TTree* intree = (TTree*)infile->Get("Cherenkov");
	Double_t xRadiator, yRadiator, theta;
	Double_t xHit[nSiLayers], yHit[nSiLayers];
	Double_t PmtTime[nChannelsPmt];
	 
	intree->SetBranchAddress("PmtTime",PmtTime);
	intree->SetBranchAddress("xRadiator", &xRadiator);  TH1F* h_xRadiator = new TH1F("h_xRadiator", "xRadiator", 50, 0, 10);
  	intree->SetBranchAddress("yRadiator", &yRadiator);  TH1F* h_yRadiator = new TH1F("h_yRadiator", "yRadiator", 50, 0, 10);
                                                      	    TH2F* h_xyRadiator = new TH2F("h_xyRadiator", "xyRadiator", 50, 0, 10, 50, 0, 10);

	intree->SetBranchAddress("xHit", xHit);             TH1F* h_x0Hit = new TH1F("h_x0Hit", "x0Hit", 50, 0, 10);
                          	                            TH1F* h_x1Hit = new TH1F("h_x1Hit", "x1Hit", 50, 0, 10);

  	intree->SetBranchAddress("yHit", yHit);             TH1F* h_y0Hit = new TH1F("h_y0Hit", "y0Hit", 50, 0, 10);
        	                                            TH1F* h_y1Hit = new TH1F("h_y1Hit", "y1Hit", 50, 0, 10);

  	intree->SetBranchAddress("theta", &theta);          TH1F* h_theta = new TH1F("h_theta", "theta", 10, thr_theta, 1);
  	
  	vector<Int_t> selectedEvents;
  	
  	for (Int_t i = 0; i < intree->GetEntries(); ++i) {
		intree->GetEntry(i);
    		h_x0Hit->Fill(xHit[0]);
    		h_x1Hit->Fill(xHit[1]);
    		h_y0Hit->Fill(yHit[0]);
    		h_y1Hit->Fill(yHit[1]);
    		
    		Int_t nChannelsInTime = 0;
    		
		for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
			if( PmtTime[iChannel]>=timeWindow_lowerBound &&PmtTime[iChannel]<=timeWindow_upperBound ) nChannelsInTime += 1;
			//If all signals are in time then AllSignalsInTime is = number of PMT channels
		}
		// CUT on time of PMT signal 
    		if( nChannelsInTime==nChannelsPmt ) {
    			// CUT on tracks direction
    			if (theta > thr_theta) {
    				// CUT on spacial distribution of hits
    				if ( abs(5 - xRadiator) < thr_xRadiator && abs(5.96 - yRadiator) < thr_yRadiator &&
	   		             abs(5 - xHit[0]) < thr_x0 && abs(5 - yHit[0]) < thr_y0 ) {
	   				h_xyRadiator->Fill(yRadiator, xRadiator);
	   				h_xRadiator->Fill(xRadiator);
	   				h_yRadiator->Fill(yRadiator);
	   				h_theta->Fill(theta);
	   				selectedEvents.push_back(i+1);
				}
			}
		}
	}
	
	cout << "Selected events = " << selectedEvents.size() << endl;
	ofstream selectedEvents_fout(Form("run%i_selectedEvents_thetaThr%1.1f_xRadiatorThr%1.1f_xUpperSiThr%1.1f.txt",
					   SiRunNumber,thr_theta,thr_xRadiator,thr_x0));
	
	for(Int_t iEv=0; iEv<selectedEvents.size(); ++iEv) {
		selectedEvents_fout << selectedEvents[iEv] << endl;
	}
		
	//In this Canvas I plot the distribution of the muons on the xy plane of the radiator provided some filters
  	TCanvas* c = new TCanvas();
  	c->Divide(2,2);
	c->cd(1);
  	intree->Draw("xRadiator");
  	h_xRadiator->SetLineColor(kRed);
  	h_xRadiator->Draw("same");
	c->cd(2);
  	intree->Draw("yRadiator");
  	h_yRadiator->SetLineColor(kRed);
  	h_yRadiator->Draw("same");
	c->cd(3);
  	h_xyRadiator->Draw("colz");
	c->cd(4);
  	intree->Draw("theta");
  	h_theta->SetLineColor(kRed);
  	h_theta->Draw("same");
  
  	//In this canvas I plot all the hit on the UPPER and LOWER SI and on the radiator
  	TCanvas* c1 = new TCanvas();
  	c1->Divide(2,2);
	c1->cd(1);
  	intree->Draw("yHit[0]:xHit[0]");
	c1->cd(2);
	intree->Draw("yHit[1]:xHit[1]");
	c1->cd(3);
	intree->Draw("yRadiator:xRadiator");
	
	return;
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
