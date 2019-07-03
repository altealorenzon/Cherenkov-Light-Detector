#include <fstream>

void ProduceArrays(TString file_name, Int_t event_number) {
	TFile *file = new TFile(file_name);
    	TTree *tree = (TTree*)file->Get("Cherenkov");
    	
    	Int_t evNumber;     tree->SetBranchAddress("evNumber",&evNumber);
    	Int_t id;           tree->SetBranchAddress("id",&id);
    	Int_t phNumber;	    tree->SetBranchAddress("phNumber",&phNumber); 
	Double_t x[168];    tree->SetBranchAddress("x",x);
	Double_t y[168];    tree->SetBranchAddress("y",y);
	Double_t z[168];    tree->SetBranchAddress("z",z);
	
	Int_t nEntries = tree->GetEntries();
    	
    	ofstream fx_mu; fx_mu.open("mu_x.txt");
    	ofstream fy_mu; fy_mu.open("mu_y.txt");
    	ofstream fz_mu; fz_mu.open("mu_z.txt");
    	ofstream fx_ph; fx_ph.open("ph_x.txt");
    	ofstream fy_ph; fy_ph.open("ph_y.txt");
    	ofstream fz_ph; fz_ph.open("ph_z.txt");
    	
    	for(Int_t iEntry=0; iEntry<nEntries; ++iEntry) {
        	tree->GetEntry(iEntry); //each entry is a particle
        	for(Int_t i=0; i<168; ++i) {
        		if(id==22 && evNumber==event_number && x[i]>-999){
        			fx_ph << x[i] << endl;
        			fy_ph << y[i] << endl;
        			fz_ph << -z[i]+1. << endl;
        		}
        		if(id==13 && evNumber==event_number && x[i]>-999 && i!=0){
            			fx_mu << x[i] << endl;
        			fy_mu << y[i] << endl;
        			fz_mu << -z[i]+1. << endl;
        		}
        	}
    	}
    	fx_mu.close();
    	fy_mu.close();
    	fz_mu.close();
    	fx_ph.close();
    	fy_ph.close();
    	fz_ph.close();
	return;
}
