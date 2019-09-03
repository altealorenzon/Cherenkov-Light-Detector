/************************************************************************
*			PM_plane_visualizer.C                    	*
*************************************************************************
* -> To run the macto:							*
*    > root								*
*    > .L PM_plane_visualizer.C						*
*									*
* -> To see the tracks in one event:				        *
*    > event_display("filename with path", event number, true/false)    *
*       -> choose true if the radiator is a parallelepiped		*
*	-> choose false if it is a cylinder				*
*									*
* -> To see the number of photons produced per event:			*   
*    > photon_statistics("filename with path")				*
*									*
* -> To see the signals in the PM plane and the ratio of photons	*
*    that exit from the top and from the bottom of the radiator:	*
*    > PM_plane_visualizer("filename with path", event number)		*
*									*
*************************************************************************/


void PM_plane_visualizer( TString file_name, Int_t event_number ) {
    
    TFile *file = new TFile(file_name);
    TTree *tree = (TTree*)file->Get("Cherenkov");
    
    Int_t evNumber;     tree->SetBranchAddress("evNumber",&evNumber);
    Int_t id;           tree->SetBranchAddress("id",&id); 
    Int_t position_out; tree->SetBranchAddress("position_out",&position_out);
    Double_t x_PM;      tree->SetBranchAddress("x_PM",&x_PM);
    Double_t y_PM;      tree->SetBranchAddress("y_PM",&y_PM);
    
    TH1F* phPosOut = new TH1F("phPosOut","",3,-1.5,1.5);
    TH2F* PMphXY   = new TH2F("PMphXY","",16,-4.9,4.9,16,-4.9,4.9);
    TMarker* mu;
    Int_t nEntries = tree->GetEntries();
    
    for(Int_t iEntry=0; iEntry<nEntries; ++iEntry) {
        tree->GetEntry(iEntry); //each entry is a particle
        if(id==22 && evNumber == event_number){
            phPosOut->Fill(position_out);
        }
        if(id==22 && position_out==1 && evNumber == event_number){
            PMphXY->Fill(x_PM,y_PM);
        }
        if(id==13 && evNumber == event_number){
            mu = new TMarker(x_PM, y_PM, 34);
        }
    }
    
    Double_t Top_over_Bottom = phPosOut->GetBinContent(1)/phPosOut->GetBinContent(3);
    Double_t Walls_over_Bottom = phPosOut->GetBinContent(2)/phPosOut->GetBinContent(3);
    TPaveText* pt = new TPaveText(0.1260745,0.8463158,0.491404,0.9410526,"NBNDC");
    pt->SetTextFont(42);
    pt->SetTextSize(0.07);
    pt->AddText(Form("Top/Bottom   = %f %%", Top_over_Bottom*100));
    pt->AddText(Form("Walls/Bottom = %f %%", Walls_over_Bottom*100));
    
    TCanvas* c = new TCanvas();
    phPosOut->SetStats(0);
    phPosOut->SetFillColor(kAzure-8);
    phPosOut->SetLineColor(kAzure-8);
    TAxis* xaxis = (TAxis*)phPosOut->GetXaxis();
    xaxis->SetBinLabel(1,"Top");
    xaxis->SetBinLabel(2,"Walls");
    xaxis->SetBinLabel(3,"Bottom");
    xaxis->SetTickLength(0.0);
    xaxis->SetLabelFont(42);
    xaxis->SetLabelSize(0.07);
    phPosOut->Draw();
    pt->Draw();
    //c->SaveAs("top-bottom.pdf");
    
    TLine* left  = new TLine( -2.45, 2.45,-2.45,-2.45 );
    TLine* right = new TLine(  2.45, 2.45, 2.45,-2.45 );
    TLine* up    = new TLine( -2.45, 2.45, 2.45, 2.45 );
    TLine* down  = new TLine( -2.45,-2.45, 2.45,-2.45 );
    //TEllipse* circle = new TEllipse( 0.0,0.0,1.0,1.0 );
    //circle->SetFillColorAlpha(0,0.0);
    
    TCanvas* cc = new TCanvas("cc","cc",750,750);
    TAxis* Xaxis = PMphXY->GetXaxis();
    Xaxis->SetTitle("x [cm]");
    Xaxis->CenterTitle();
    TAxis* Yaxis = PMphXY->GetYaxis();
    Yaxis->SetTitle("y [cm]");
    Yaxis->CenterTitle();
    PMphXY->SetStats(0);
    PMphXY->Draw("colz");    
    mu->Draw("SAME");
    left->Draw("SAME");
    right->Draw("SAME");
    up->Draw("SAME");
    down->Draw("SAME");
    //circle->Draw("SAME");
}

void photon_statistics( TString file_name ) {
    TFile *file = new TFile(file_name);
    TTree *tree = (TTree*)file->Get("Cherenkov");
    
    Int_t evNumber;     tree->SetBranchAddress("evNumber",&evNumber);
    Int_t id;           tree->SetBranchAddress("id",&id); 
    Int_t phNumber;     tree->SetBranchAddress("phNumber", &phNumber);
    
    Int_t nEntries = tree->GetEntries();
    Int_t nEvents = 100;
    Int_t maxPh[nEvents];
    
    for( Int_t iEvent=0; iEvent<nEvents; ++iEvent ) {
        maxPh[iEvent] = -1;
    }
    
    for(Int_t iEntry=0; iEntry<nEntries; ++iEntry){
        tree->GetEntry(iEntry); //each entry is a particle
        if ( id==22 ) {
            if( maxPh[evNumber-1] < phNumber ) maxPh[evNumber-1] = phNumber;
        }
    }
    
    TH1F* phDistribution = new TH1F("Photon distribution", "", nEvents, 0.5, 0.5+nEvents);
    Int_t sum = 0;
    
    for( Int_t iEvent=0; iEvent<nEvents; ++iEvent ) {
        phDistribution->SetBinContent( iEvent+1, maxPh[iEvent] );
        sum += maxPh[iEvent];
    }
    
    TCanvas* c = new TCanvas("c","",1500,750);
    TAxis* Xaxis = phDistribution->GetXaxis();
    Xaxis->SetTitle("Event Number");
    Xaxis->SetTitleSize(0.05);
    Xaxis->CenterTitle();
    TAxis* Yaxis = phDistribution->GetYaxis();
    Yaxis->SetTitle("Number of photons");
    Yaxis->SetTitleSize(0.05);
    Yaxis->CenterTitle();
    
    TPaveText* pt = new TPaveText(0.3171806,0.8537666,0.6819383,0.9542097,"NB" "NDC");
    pt->SetTextFont(42);
    pt->SetTextSize(0.07);
    pt->AddText(Form("Mean = %f ", sum*1.0/nEvents));
    
    phDistribution->SetStats(0);
    phDistribution->SetLineColor(kRed-7);
    phDistribution->SetFillColor(kRed-7);
    phDistribution->Draw();
    pt->Draw();
}

void event_display( TString file_name, Int_t evNumber, bool Is_Parallelepyped=false ) {
    TFile *file = new TFile(file_name);
    TTree *tree = (TTree*)file->Get("Cherenkov");
    
    TCanvas* c = new TCanvas("c","",1500,750);
    
    if( Is_Parallelepyped ) {
    	TPolyLine3D* f1 = new TPolyLine3D(5);
    	f1->SetPoint(0,-3,-3,0);
   	f1->SetPoint(1,-3,-3,-1);
    	f1->SetPoint(2,-3,3,-1);
    	f1->SetPoint(3,-3,3,0);
    	f1->SetPoint(4,-3,-3,0);
        f1->Draw();
    
	TPolyLine3D* f2 = new TPolyLine3D(5);
	f2->SetPoint(0,-3,3,0);
	f2->SetPoint(1,-3,3,-1);
	f2->SetPoint(2,3,3,-1);
	f2->SetPoint(3,3,3,0);
	f2->SetPoint(4,-3,3,0);
	f2->Draw("SAME");
    
	TPolyLine3D* f3 = new TPolyLine3D(5);
	f3->SetPoint(0,3,3,0);
	f3->SetPoint(1,3,3,-1);
	f3->SetPoint(2,3,-3,-1);
	f3->SetPoint(3,3,-3,0);
	f3->SetPoint(4,3,3,0);
	f3->Draw("SAME");
    
	TPolyLine3D* f4 = new TPolyLine3D(5);
	f4->SetPoint(0,3,-3,0);
	f4->SetPoint(1,3,-3,-1);
	f4->SetPoint(2,-3,-3,-1);
	f4->SetPoint(3,-3,-3,0);
	f4->SetPoint(4,3,-3,0);
	f4->Draw("SAME");
	tree->Draw("-z:x:y", "id==22&&x>-999&&z<=1", "SAME");
    }
    
    tree->Draw("-z:x:y", Form("id==22&&x>-999&&z<=1&&evNumber==%i",evNumber));
    tree->Draw("-z:x:y", Form("id==13&&x>-999&&z<=1&&evNumber==%i",evNumber),"SAME");
    TEllipse* circle = new TEllipse( 0.0,0.0,1.0,1.0 );
    circle->SetFillColorAlpha(kBlue,0.4);
    circle->Draw("SAME");
}
