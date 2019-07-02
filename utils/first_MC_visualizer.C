/**************************************
 *                                    *
 *       ROOT MACRO TO VISUALIZE      *
 *       THE CHERENKOV MC RESULTS     *
 *                                    *
 **************************************/

void MC_visualizer( TString file_name ) {
    
    TFile *file = new TFile(file_name);
    TTree *tree = (TTree*)file->Get("Cherenkov");
    
    const Int_t length = 100000;
    
    Int_t evNumber;     tree->SetBranchAddress("evNumber",&evNumber);
    Int_t id;           tree->SetBranchAddress("id",&id);
    Int_t position_out; tree->SetBranchAddress("position_out",&position_out);
    Double_t x_out;     tree->SetBranchAddress("x_out",&x_out);
    Double_t y_out;     tree->SetBranchAddress("y_out",&y_out);
    Double_t z_out;     tree->SetBranchAddress("z_out",&z_out);
    
    Double_t bound = 5.0/sqrt(2)/2;

    TH1F* phPosOut   = new TH1F("phPosOut"," bottom = 1 / walls = 0 /top = -1",3,-1.5,1.5);
    
    Int_t nHistos = 1;
    
    TH2F* phXYOut_16[nHistos]; 
    TH2F* phXYOut_64[nHistos];
    
    for(Int_t i=0; i<nHistos; ++i) {
        phXYOut_16[i] = new TH2F(Form("phXYOut_16_%i",i),"",4,-bound,bound,4,-bound,bound);
        phXYOut_64[i] = new TH2F(Form("phXYOut_64_%i",i),"",8,-2.5,2.5,8,-2.5,2.5);
    }
    
    Int_t nEntries = tree->GetEntries();
    Int_t iHisto = 0;
    Int_t ev[nHistos];
    Double_t xx[nHistos];
    Double_t yy[nHistos];
    
    for(Int_t i=0; i<nHistos; ++i){
        ev[i]=-1;
    }
    
    for(Int_t iEntry=0; iEntry<nEntries; ++iEntry) {
        tree->GetEntry(iEntry); //each entry is a particle
        if(id==22){
            phPosOut->Fill(position_out);
        }
//        if(id==13 && z_out>=8.0 /*&& iHisto<nHistos*/) {
//            //ev[iHisto] = evNumber;
//            cout << "Event Number = " << evNumber << endl;
//            cout << "(" << x_out << ", " << y_out << ", " << z_out << ")" << endl;
//            xx[iHisto] = x_out;
//            yy[iHisto] = y_out;
//            ++iHisto;
//        }
//        if(id==22 && /*evNumber==ev[iHisto-1] && iHisto-1<nHistos &&*/ position_out==1) {
//            phXYOut_16[iHisto-1]->Fill(x_out, y_out);
//            phXYOut_64[iHisto-1]->Fill(x_out, y_out);
//        } 
    }
    
    Double_t Top_over_Bottom = phPosOut->GetBinContent(1)/phPosOut->GetBinContent(3);
    
    TPaveText* pt = new TPaveText(0.05,0.93,0.35,0.85,"NB" "NDC");
    pt->AddText(Form("Top/Bottom = %f %%", Top_over_Bottom*100));
    
    TCanvas* c = new TCanvas();
    phPosOut->SetStats(0);
    phPosOut->SetFillColor(kAzure-8);
    phPosOut->SetLineColor(kAzure-8);
    phPosOut->Draw();
    pt->Draw();
    c->SaveAs("top-bottom.pdf");    

//    TCanvas* cc[iHisto];
//    TAxis* Xaxis1[iHisto];
//    TAxis* Yaxis1[iHisto];
//    TAxis* Xaxis2[iHisto];
//    TEllipse* circle[iHisto];
//    TMarker* mu[iHisto];
//    
//    for(Int_t i=0; i<iHisto; ++i) {
//        
//        cout << "Saving histogram " << i+1 << endl;
//        mu[i] = new TMarker( xx[i], yy[i], 34 );
//        mu[i]->SetMarkerSize(2);
//        
//        cc[i] = new TCanvas(Form("cc_%i",i),"",1500,750);
//        cc[i]->Divide(2,1);
//        cc[i]->cd(1);
//        
//        Xaxis1[i] = phXYOut_16[i]->GetXaxis();
//        Xaxis1[i]->SetTitle("x [cm]");
//        Xaxis1[i]->CenterTitle();
//        Yaxis1[i] = phXYOut_16[i]->GetYaxis();
//        Yaxis1[i]->SetTitle("y [cm]");
//        Yaxis1[i]->CenterTitle();
//        phXYOut_16[i]->SetStats(0);
//        phXYOut_16[i]->Draw("colz");
//        mu[i]->Draw("SAME");
//        
//        cc[i]->cd(2);
//        circle[i] = new TEllipse(0.0,0.0,2.51,2.51);
//        Xaxis2[i] = phXYOut_64[i]->GetXaxis();
//        Xaxis2[i]->SetTitle("x [cm]");
//        Xaxis2[i]->CenterTitle();
//        phXYOut_64[i]->SetStats(0);
//        phXYOut_64[i]->Draw("colz");
//        circle[i]->SetFillColorAlpha(0,0.0);
//        circle[i]->Draw("SAME");
//        mu[i]->Draw("SAME");
//        
//        cc[i]->SaveAs(Form("cc_%i.pdf",i+1));
//        delete cc[i];
//    }
}
