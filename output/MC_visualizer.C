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
    
    TH1F* phOut = new TH1F("phOut"," bottom = 1 / walls = 0 /top = -1",3,-1.5,1.5);
    
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
    
}
