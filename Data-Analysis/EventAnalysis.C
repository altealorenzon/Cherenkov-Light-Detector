/*********************EventAnalysis.C******************
 *
 * To compile: $ root -l 
 *             .L EventAnalysis.C
 *             xyrad_histo(thr_theta, thr_xRadiator, thr_yRadiator, thr_x0, thr_y0)
 *
 *
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
 *
 * + A canvas of three histograms with the hits on the UPPER and LOWER Si and on the Radiator.
 *
 * Example: xyrad_histo(0.99, 2.0, 2.0, 10.0, 10.0).
 * --> I'm filtering only on the radiator since the thr on the upper Si is higher than the Si's dimensions.
 *
 ********************************************************/



#include <fstream>
#include "TH1F.h"
#include "TGraph.h"
#include "TString.h"
#include <iostream>
#include "TFile.h"
#include "TTree.h"

using namespace std;

void xyrad_histo();

void xyrad_histo(Double_t thr_theta,
		 Double_t thr_xRadiator,
		 Double_t thr_yRadiator,
		 Double_t thr_x0,
		 Double_t thr_y0) {

  TFile* infile = new TFile("run300126.root", "READ");
  if (infile->IsOpen() ) printf("File opened successfully\n");
  
  TTree* intree = (TTree*)infile->Get("Cherenkov");
  Double_t xRadiator, yRadiator, PmtTime, theta;
  Double_t xHit[2], yHit[2];
    
  intree->SetBranchAddress("xRadiator", &xRadiator);  TH1F* h_xRadiator = new TH1F("h_xRadiator", "xRadiator", 50, 0, 10);
  intree->SetBranchAddress("yRadiator", &yRadiator);  TH1F* h_yRadiator = new TH1F("h_yRadiator", "yRadiator", 50, 0, 10);
                                                      TH2F* h_xyRadiator = new TH2F("h_xyRadiator", "xyRadiator", 50, 0, 10, 50, 0, 10);

  intree->SetBranchAddress("xHit", &xHit);            TH1F* h_x0Hit = new TH1F("h_x0Hit", "x0Hit", 50, 0, 10);
                                                      TH1F* h_x1Hit = new TH1F("h_x1Hit", "x1Hit", 50, 0, 10);

  intree->SetBranchAddress("yHit", &yHit);            TH1F* h_y0Hit = new TH1F("h_y0Hit", "y0Hit", 50, 0, 10);
                                                      TH1F* h_y1Hit = new TH1F("h_y1Hit", "y1Hit", 50, 0, 10);

  intree->SetBranchAddress("theta", &theta);          TH1F* h_theta = new TH1F("h_theta", "theta", 10, thr_theta, 1);


  for (Int_t i = 0; i < intree->GetEntries(); ++i) {

    intree->GetEntry(i);
    h_x0Hit->Fill(xHit[0]);
    h_x1Hit->Fill(xHit[1]);
    h_y0Hit->Fill(yHit[0]);
    h_y1Hit->Fill(yHit[1]);
    
    if (theta > thr_theta) {
   
      if ( abs(5 - xRadiator) < thr_xRadiator && abs(5.96 - yRadiator) < thr_yRadiator &&
	   abs(5 - xHit[0]) < thr_x0 && abs(5 - yHit[0]) < thr_y0 ) {

	h_xyRadiator->Fill(yRadiator, xRadiator);
	h_xRadiator->Fill(xRadiator);
	h_yRadiator->Fill(yRadiator);
	h_theta->Fill(theta);
	
      }
    }
      
    
  };

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
