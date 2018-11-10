/**************************************
 *                                    *
 *       ROOT MACRO TO VISUALIZE      *
 *       THE CHERENKOV MC RESULTS     *
 *                                    *
 **************************************/

void MC_visualizer() {
    
    TFile *file = new TFile("Cherenkov_MC.root");
    TTree *tree = (TTree*)file->Get("Cherenkov");
    
    Int_t position_out; tree->SetBranchAddress("position_out",&position_out);
    Int_t id;           tree->SetBranchAddress("id",&id);
    
    TH1F* phOut = new TH1F("phOut","",3,-1.5,1.5);
    
    Int_t nEntries = tree->GetEntries();
    
    for(Int_t iEntry=0; iEntry<nEntries; ++iEntry) {
        tree->GetEntry(iEntry); //each entry is a particle
        if(id==22) phOut->Fill(position_out);
    }
    
    TCanvas* c = new TCanvas();
    phOut->SetStats(0);
    phOut->SetFillColor(kAzure-8);
    phOut->SetLineColor(kAzure-8);
    phOut->Draw();
    
/*    TPaveText *pt_top = new TPaveText(0.7862254,871.7349,1.430233,1447.56,"br");
    pt_top->SetLineColor(0);
    pt_top->SetFillColor(0);
    pt_top->SetShadowColor(0);
    TText *pt_top_text = pt_top->AddText("top");
    pt_top->Draw();
    c->Modified();*/
    
}
