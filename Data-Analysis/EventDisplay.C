/*******************************************************
* 			EventDisplay.C                 *
********************************************************
* OBJECT
* This macro reads the root file (produced by Reader.C) 
* with all the important info from the detectors.
*
* OUTPUT
* The macro will produce histograms of the PMT recorded 
* times and pulse heights: it gives the possibility to 
* select events that belong to a particular time window
*
* TO COMPILE/RUN
* root -l -q EventDisplay.C+(RunNumber)
*/

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

void EventDisplay(Int_t RunNumber) {

	TFile* file = new TFile(Form("run%i.root",RunNumber));
	TTree* tree = (TTree*)file->Get("Cherenkov");
	
	TH1F* PmtTime_Histo[nChannelsPmt];
	TH1F* PmtPulseHeight_Histo[nChannelsPmt];
	TH1F* PmtPulseHeight_HistoInTime[nChannelsPmt];
	
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		PmtTime_Histo[iChannel] = new TH1F(Form("PmtTime_%i",iChannel),Form("ch %i",activeChannels[iChannel]),100,0,300);
		PmtPulseHeight_Histo[iChannel] = new TH1F(Form("PmtPulseHeight_Histo_%i",iChannel),Form("ch %i",activeChannels[iChannel]),100,0,500);
		PmtPulseHeight_HistoInTime[iChannel] = new TH1F(Form("PmtPulseHeight_HistoInTime_%i",iChannel),Form("ch %i",activeChannels[iChannel]),100,0,500);
		PmtPulseHeight_HistoInTime[iChannel]->SetLineColor(kRed);
	}
	
	Ev_t Ev;
	Int_t evNumber;     				
	tree->SetBranchAddress("evNumber",&evNumber);
	tree->SetBranchAddress("PmtTime",Ev.PmtSignal.Time);
	tree->SetBranchAddress("PmtPulseHeight",Ev.PmtSignal.PulseHeight);
	tree->SetBranchAddress("PmtChannelID",Ev.PmtSignal.ChannelID);
	
	Int_t nEntries = tree->GetEntries();
	
	for(Int_t iEntry=0; iEntry<nEntries; ++iEntry) {
		tree->GetEntry(iEntry);
		for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
			PmtTime_Histo[iChannel]->Fill(Ev.PmtSignal.Time[iChannel]);
			PmtPulseHeight_Histo[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]);
			if(Ev.PmtSignal.Time[iChannel]>=150 && Ev.PmtSignal.Time[iChannel]<=200 ) {
				if(Ev.PmtSignal.PulseHeight[iChannel]>=200) {
					cout << "ch " << Ev.PmtSignal.ChannelID[iChannel] << " evNumber " << evNumber << endl;
				}
				PmtPulseHeight_HistoInTime[iChannel]->Fill(Ev.PmtSignal.PulseHeight[iChannel]);
			}
		}
	}
	
	TCanvas* c_time = new TCanvas("c_time","",1500,750);
	c_time->Divide(nChannelsPmt/2,2);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		c_time->cd(iChannel+1);
		PmtTime_Histo[iChannel]->SetStats(0);
		PmtTime_Histo[iChannel]->GetXaxis()->SetTitle("Time");
		PmtTime_Histo[iChannel]->Draw();
	}
	
	TCanvas* c_ph = new TCanvas("c_ph","",1500,750);
	c_ph->Divide(nChannelsPmt/2,2);
	for(Int_t iChannel=0; iChannel<nChannelsPmt; ++iChannel) {
		c_ph->cd(iChannel+1);
		c_ph->cd(iChannel+1)->SetLogy();
		PmtPulseHeight_Histo[iChannel]->SetStats(0);
		PmtPulseHeight_Histo[iChannel]->GetXaxis()->SetTitle("PH");
		PmtPulseHeight_Histo[iChannel]->Draw();
		PmtPulseHeight_HistoInTime[iChannel]->SetStats(0);
		PmtPulseHeight_HistoInTime[iChannel]->Draw("SAME");
	}
	return;

}
