/**
   This program produces energy response plots from Monte-Carlo simulations
*/

#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TH2D.h>
#include <THStack.h>
#include <TProfile.h>
#include <iostream>
#include <fstream>
#include <TGraphAsymmErrors.h>

#define NTRACK_MAX (1U << 15)

#include <vector>
#include <math.h>
#include <set>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    exit(EXIT_FAILURE);
  }
  int dummyc = 1;
  char **dummyv = new char *[1];
    
  dummyv[0] = strdup("main");
  TApplication application("", &dummyc, dummyv);    
  std::cout <<" Number of arguments " << argc << std::endl; 
  for (int iarg = 1; iarg < argc; iarg++) {
    std::cout << "Opening: " << (TString)argv[iarg] << std::endl;
    TFile *file = TFile::Open((TString)argv[iarg]);
        
    if (file == NULL) {
      std::cout << " fail; could not open file" << std::endl;
      exit(EXIT_FAILURE);
    }
    file->Print();
        
    // Get all the TTree variables from the file to open, I guess
    TTree *_tree_event = NULL;
    std::cout << " About to try getting the ttree" << std::endl;
    _tree_event = dynamic_cast<TTree *> (file->Get("_tree_event"));
    if (_tree_event == NULL) {
      std::cout << "First try did not got trying again" << std::endl;
      _tree_event = dynamic_cast<TTree *> (dynamic_cast<TDirectoryFile *>   (file->Get("AliAnalysisTaskNTGJ"))->Get("_tree_event"));
      if (_tree_event == NULL) {
	std::cout << " fail; could not find _tree_event " << std::endl;
	exit(EXIT_FAILURE);
      }
    }

    if (_tree_event == NULL) {
      std::cout << " fail; the _tree_event is NULL " << std::endl;
      exit(EXIT_FAILURE);
    }
        
    //

    std::cout <<"_tree_event->GetEntries() " << _tree_event->GetEntries() << std::endl;
 
    const int xjbins = 20;
    const int phibins = 20;  
    const int etabins = 20;

    TH1D h_cutflow("h_cutflow","cut flow for photons", 10, -0.5,9.5);
    TH1D h_evtcutflow("h_evtcutflow","Event cut flow", 5, -0.5,4.5);
    TH1D h_evt_rho("h_evt_rho", "average UE density, rho", 100, 0, 10.0);
   
    TH1D h_reco("h_reco", "reco photons filled with pt reco", 50, 0, 50);    
    TH1D h_reco_truthpt("h_reco_truthpt", "reco photons filled with truthpt reco", 50, 0, 50); 
    TH1D h_truth("h_truth", "truth photons", 50, 0, 50);


    TH1D hSR_njet("hSR_njet", "Number of associated jets, signal region" , 5, -0.5, 4.5);
    TH1D hBR_njet("hBR_njet", "Number of associated jets, bkg region" , 5, -0.5, 4.5);
  

    TH1D hBR_clusterpt("hBR_clusterpt", "Isolated cluster pt [GeV], bkg region", 40, 10.0, 20.0);
    TH1D hSR_clusterpt("hSR_clusterpt", "Isolated cluster pt [GeV], signal region", 40, 10.0, 20.0);

    TH1D hBR_clustereta("hBR_clustereta", "Isolated cluster eta, bkg region", 20, -1.0, 1.0);
    TH1D hSR_clustereta("hSR_clustereta", "Isolated cluster eta, signal region", 20, -1.0, 1.0);
    TH1D hBR_clusterphi("hBR_clusterphi", "Isolated cluster phi, bkg region", 20, -1.0*TMath::Pi(), TMath::Pi());
    TH1D hSR_clusterphi("hSR_clusterphi", "Isolated cluster phi, signal region", 20, -1.0*TMath::Pi(), TMath::Pi());


    TH1D hBR_jetpt("hBR_jetpt",   "Associated jet pt spectrum (reco), bkg region", 30, 0, 30);
    TH1D hSR_jetpt("hSR_jetpt",   "Associated jet pt spectrum (reco), signal region", 30, 0, 30);
    TH1D hBR_jeteta("hBR_jeteta", "Associated jet eta spectrum (reco), bkg region", 20, -1.0, 1.0);
    TH1D hSR_jeteta("hSR_jeteta", "Associated jet eta spectrum (reco), signal region", 20, -1.0, 1.0); 
    TH1D hBR_jetphi("hBR_jetphi", "Associated jet phi spectrum (reco), bkg region", 20, -1.0*TMath::Pi(), TMath::Pi());
    TH1D hSR_jetphi("hSR_jetphi", "Associated jet phi spectrum (reco), signal region", 20, -1.0*TMath::Pi(), TMath::Pi());


    TH1D hBR_jetpt_truth("hBR_jetpt_truth",   "Associated jet pt spectrum (truth), bkg region", 30, 0, 30);
    TH1D hSR_jetpt_truth("hSR_jetpt_truth",   "Associated jet pt spectrum (truth), signal region", 30, 0, 30);
    TH1D hBR_jeteta_truth("hBR_jeteta_truth", "Associated jet eta spectrum (truth), bkg region", 20, -1.0, 1.0);
    TH1D hSR_jeteta_truth("hSR_jeteta_truth", "Associated jet eta spectrum (truth), signal region", 20, -1.0, 1.0);
    TH1D hBR_jetphi_truth("hBR_jetphi_truth", "Associated jet phi spectrum (truth), bkg region", 20, -1.0*TMath::Pi(), TMath::Pi());
    TH1D hSR_jetphi_truth("hSR_jetphi_truth", "Associated jet phi spectrum (truth), signal region", 20, -1.0*TMath::Pi(), TMath::Pi());

    TH1D h_jetpt_truth("h_jetpt_truth", "truth jet pt", 30, 0, 30);
    TH1D h_jetpt_truthreco("h_jetpt_truthreco", "reco jet truth pt (numerator of efficiency)", 30, 0, 30);
    TH1D h_jetpt_reco("h_jetpt_reco", "reco jet reco pt", 30, 0, 30);
    
 

    TH1D hSR_Xj("hSR_Xj", "Xj distribution, Signal region", xjbins, 0.0,2.0);
    TH1D hBR_Xj("hBR_Xj", "Xj distribution, BKG region", xjbins, 0.0,2.0);
    TH1D hSR_pTD("hSR_pTD", "pTD distribution, Signal region", 40, 0.0,1.0);
    TH1D hBR_pTD("hBR_pTD", "pTD distribution, BKG region", 40, 0.0,1.0);
    TH1D hSR_Multiplicity("hSR_Multiplicity", "Jet Multiplicity distribution, Signal region", 20, -0.5 , 19.5);
    TH1D hBR_Multiplicity("hBR_Multiplicity", "Jet Multiplicity distribution, BKG region", 20, -0.5, 19.5);
    TH1D hSR_jetwidth("hSR_jetwidth", "jet width distribution, Signal region", 20, -10, 0);
    TH1D hBR_jetwidth("hBR_jetwidth", "jet width distribution, BKG region", 20, -10, 0);


    TH1D hSR_Xj_truth("hSR_Xj_truth", "True Xj distribution, Signal region", xjbins, 0.0,2.0);
    TH1D hBR_Xj_truth("hBR_Xj_truth", "True Xj distribution, BKG region",xjbins, 0.0,2.0);
    TH2D h_Xj_Matrix("h_Xj_Matrix", "Truth/Reco matrix", xjbins, 0.0,2.0, xjbins, 0.0,2.0);
    TH1D h_Xj_truth("h_Xj_truth", "Xj truth distribution", xjbins, 0.0,2.0);    

    TH1D hSR_dPhi("hSR_dPhi", "delta phi gamma-jet signal region", phibins, 0, TMath::Pi());
    TH1D hBR_dPhi("hBR_dPhi", "delta phi gamma-jet background region", phibins, 0, TMath::Pi());
    TH1D hSR_dPhi_truth("hSR_dPhi_truth", "delta phi gamma-jet signal region, truth", phibins, 0, TMath::Pi());
    TH1D hBR_dPhi_truth("hBR_dPhi_truth", "delta phi gamma-jet background region, truth", phibins, 0, TMath::Pi());


    TH1D hSR_dEta("hSR_dEta", "delta eta gamma-jet signal region", etabins, -1.2, 1.2);
    TH1D hBR_dEta("hBR_dEta", "delta eta gamma-jet background region", etabins, -1.2, 1.2);
    TH1D hSR_dEta_truth("hSR_dEta_truth", "delta eta gamma-jet signal region, truth", etabins, -1.2, 1.2);
    TH1D hBR_dEta_truth("hBR_dEta_truth", "delta eta gamma-jet background region, truth", etabins, -1.2, 1.2);

    TH1D hSR_AvgEta("hSR_AvgEta", "Average eta gamma-jet signal region", 2*etabins, -1.2, 1.2);
    TH1D hBR_AvgEta("hBR_AvgEta", "Average eta gamma-jet background region", 2*etabins, -1.2, 1.2);
    TH1D hSR_AvgEta_truth("hSR_AvgEta_truth", "Average eta gamma-jet signal region, truth", 2*etabins, -1.2, 1.2);
    TH1D hBR_AvgEta_truth("hBR_AvgEta_truth", "Average eta gamma-jet background region, truth", 2*etabins, -1.2, 1.2);


    TH1D h_dPhi_truth("h_dPhi_truth", "delta phi gamma-jet truth MC", phibins, 0, TMath::Pi());


    hSR_pTD.Sumw2();
    hBR_pTD.Sumw2();
    hSR_Multiplicity.Sumw2();
    hBR_Multiplicity.Sumw2();
    hSR_jetwidth.Sumw2();
    hBR_jetwidth.Sumw2();

    h_evt_rho.Sumw2();
    hSR_Xj.Sumw2();
    hBR_Xj.Sumw2();
    hSR_Xj_truth.Sumw2();
    hBR_Xj_truth.Sumw2();
    h_Xj_truth.Sumw2();     

    hSR_njet.Sumw2();
    hBR_njet.Sumw2();

    hSR_dPhi.Sumw2();
    hBR_dPhi.Sumw2();
    hSR_dPhi_truth.Sumw2();
    hBR_dPhi_truth.Sumw2();

    hSR_dEta.Sumw2();
    hBR_dEta.Sumw2();
    hSR_dEta_truth.Sumw2();
    hBR_dEta_truth.Sumw2();

    hSR_AvgEta.Sumw2();
    hBR_AvgEta.Sumw2();
    hSR_AvgEta_truth.Sumw2();
    hBR_AvgEta_truth.Sumw2();


    h_jetpt_truth.Sumw2();
    h_jetpt_truthreco.Sumw2();
    h_jetpt_reco.Sumw2();

    h_dPhi_truth.Sumw2();

    hSR_clusterpt.Sumw2();
    hBR_clusterpt.Sumw2();
    hSR_clustereta.Sumw2();
    hBR_clustereta.Sumw2();
    hSR_clusterphi.Sumw2();
    hBR_clusterphi.Sumw2();

    hBR_jetpt.Sumw2();
    hSR_jetpt.Sumw2();
    hBR_jeteta.Sumw2();
    hSR_jeteta.Sumw2();
    hBR_jetphi.Sumw2();
    hSR_jetphi.Sumw2();
    
    hBR_jetpt_truth.Sumw2();
    hSR_jetpt_truth.Sumw2();
    hBR_jeteta_truth.Sumw2();
    hSR_jeteta_truth.Sumw2();
    hBR_jetphi_truth.Sumw2();
    hSR_jetphi_truth.Sumw2();


    h_Xj_Matrix.Sumw2();

    hSR_Xj.SetTitle("; X_{j} ; 1/N_{#gamma} dN_{J#gamma}/dX_{j}");
    hBR_Xj.SetTitle("; X_{j} ; 1/N_{#gamma} dN_{J#gamma}/dX_{j}");   
    h_Xj_truth.SetTitle("; X_{j}^{true} ; counts");

    
    TCanvas* canvas = new TCanvas();
        
    //you define variables
    Double_t primary_vertex[3];
    Bool_t is_pileup_from_spd_5_08;
    Bool_t is_pileup_from_spd_3_08;
    Float_t ue_estimate_its_const;
   
    UInt_t ntrack;
    Float_t track_e[NTRACK_MAX];
    Float_t track_pt[NTRACK_MAX];
    Float_t track_eta[NTRACK_MAX];
    Float_t track_phi[NTRACK_MAX];
    UChar_t track_quality[NTRACK_MAX];
        
    UInt_t ncluster;
    Float_t cluster_e[NTRACK_MAX];
    Float_t cluster_e_cross[NTRACK_MAX];
    Float_t cluster_pt[NTRACK_MAX];
    Float_t cluster_eta[NTRACK_MAX];
    Float_t cluster_phi[NTRACK_MAX];
    Float_t cluster_iso_tpc_04[NTRACK_MAX];
    Float_t cluster_iso_its_04[NTRACK_MAX];
    Float_t cluster_frixione_tpc_04_02[NTRACK_MAX];
    Float_t cluster_frixione_its_04_02[NTRACK_MAX];
    Float_t cluster_s_nphoton[NTRACK_MAX][4];
    UChar_t cluster_nlocal_maxima[NTRACK_MAX];
    Float_t cluster_distance_to_bad_channel[NTRACK_MAX];   
 
    unsigned short cluster_mc_truth_index[NTRACK_MAX][32];
    Int_t cluster_ncell[NTRACK_MAX];
    UShort_t  cluster_cell_id_max[NTRACK_MAX];
    Float_t cluster_lambda_square[NTRACK_MAX][2];
    Float_t cell_e[17664];

    //Jets reco 
    UInt_t njet_ak04its;
    Float_t jet_ak04its_pt_raw[NTRACK_MAX];
    Float_t jet_ak04its_eta_raw[NTRACK_MAX];
    Float_t jet_ak04its_phi[NTRACK_MAX];
    
    Float_t jet_ak04its_pt_truth[NTRACK_MAX];
    Float_t jet_ak04its_eta_truth[NTRACK_MAX];
    Float_t jet_ak04its_phi_truth[NTRACK_MAX];
  
    //The z_reco is defined as the fraction of the true jet that ended up in this reco jet 
    //There are two entries and indices, the first is the best. 

    Int_t   jet_ak04its_truth_index_z_reco[NTRACK_MAX][2];
    Float_t jet_ak04its_truth_z_reco[NTRACK_MAX][2];

    Float_t jet_ak04its_ptd_raw[NTRACK_MAX];
    Float_t jet_ak04its_width_sigma[NTRACK_MAX];
    UShort_t jet_ak04its_multiplicity[NTRACK_MAX];

    //Truth Jets
    UInt_t njet_truth_ak04;
    Float_t jet_truth_ak04_pt[NTRACK_MAX];
    Float_t jet_truth_ak04_eta[NTRACK_MAX];
    Float_t jet_truth_ak04_phi[NTRACK_MAX];
   
    //Int_t eg_ntrial;

    Float_t eg_cross_section;
    Int_t   eg_ntrial;
    
    //MC
    unsigned int nmc_truth;
    Float_t mc_truth_pt[NTRACK_MAX];
    Float_t mc_truth_eta[NTRACK_MAX];
    Float_t mc_truth_phi[NTRACK_MAX];
    short mc_truth_pdg_code[NTRACK_MAX];
    short mc_truth_first_parent_pdg_code[NTRACK_MAX];
    char mc_truth_charge[NTRACK_MAX];
    UChar_t mc_truth_status[NTRACK_MAX];
        
    Float_t mc_truth_first_parent_e[NTRACK_MAX];
    Float_t mc_truth_first_parent_pt[NTRACK_MAX];
    Float_t mc_truth_first_parent_eta[NTRACK_MAX];
    Float_t mc_truth_first_parent_phi[NTRACK_MAX];
    
    ULong64_t trigger_mask[2];
        
    // Set the branch addresses of the branches in the TTrees
    //    _tree_event->SetBranchAddress("eg_ntrial",&eg_ntrial);
    _tree_event->SetBranchAddress("primary_vertex", primary_vertex);
    _tree_event->SetBranchAddress("is_pileup_from_spd_5_08", &is_pileup_from_spd_5_08);
    _tree_event->SetBranchAddress("is_pileup_from_spd_3_08", &is_pileup_from_spd_3_08);
    _tree_event->SetBranchAddress("ue_estimate_its_const", &ue_estimate_its_const);
    _tree_event->SetBranchAddress("trigger_mask", &trigger_mask);


    _tree_event->SetBranchAddress("ntrack", &ntrack);
    _tree_event->SetBranchAddress("track_e", track_e);
    _tree_event->SetBranchAddress("track_pt", track_pt);
    _tree_event->SetBranchAddress("track_eta", track_eta);
    _tree_event->SetBranchAddress("track_phi", track_phi);
    _tree_event->SetBranchAddress("track_quality", track_quality);
        
    _tree_event->SetBranchAddress("ncluster", &ncluster);
    _tree_event->SetBranchAddress("cluster_e", cluster_e);
    _tree_event->SetBranchAddress("cluster_e_cross", cluster_e_cross);
    _tree_event->SetBranchAddress("cluster_pt", cluster_pt); // here
    _tree_event->SetBranchAddress("cluster_eta", cluster_eta);
    _tree_event->SetBranchAddress("cluster_phi", cluster_phi);
    _tree_event->SetBranchAddress("cluster_s_nphoton", cluster_s_nphoton); // here
    _tree_event->SetBranchAddress("cluster_mc_truth_index", cluster_mc_truth_index);
    _tree_event->SetBranchAddress("cluster_lambda_square", cluster_lambda_square);
    _tree_event->SetBranchAddress("cluster_iso_tpc_04",cluster_iso_tpc_04);
    _tree_event->SetBranchAddress("cluster_iso_its_04",cluster_iso_its_04);
    _tree_event->SetBranchAddress("cluster_frixione_tpc_04_02",cluster_frixione_tpc_04_02);
    _tree_event->SetBranchAddress("cluster_frixione_its_04_02",cluster_frixione_its_04_02);
    _tree_event->SetBranchAddress("cluster_nlocal_maxima", cluster_nlocal_maxima);        
    _tree_event->SetBranchAddress("cluster_distance_to_bad_channel", cluster_distance_to_bad_channel);

    _tree_event->SetBranchAddress("cluster_ncell", cluster_ncell);
    _tree_event->SetBranchAddress("cluster_cell_id_max", cluster_cell_id_max);
    _tree_event->SetBranchAddress("cell_e", cell_e);
        
    _tree_event->SetBranchAddress("nmc_truth", &nmc_truth);
    _tree_event->SetBranchAddress("mc_truth_pdg_code", mc_truth_pdg_code);
    _tree_event->SetBranchAddress("mc_truth_pt", mc_truth_pt);
    _tree_event->SetBranchAddress("mc_truth_phi", mc_truth_phi);
    _tree_event->SetBranchAddress("mc_truth_eta", mc_truth_eta);
    _tree_event->SetBranchAddress("mc_truth_status", mc_truth_status);        
    _tree_event->SetBranchAddress("mc_truth_first_parent_pdg_code",mc_truth_first_parent_pdg_code);

    _tree_event->SetBranchAddress("eg_cross_section",&eg_cross_section);
    _tree_event->SetBranchAddress("eg_ntrial",&eg_ntrial);
    

    //jets
    _tree_event->SetBranchAddress("njet_ak04its", &njet_ak04its);
    _tree_event->SetBranchAddress("jet_ak04its_pt_raw", jet_ak04its_pt_raw);
    _tree_event->SetBranchAddress("jet_ak04its_eta_raw", jet_ak04its_eta_raw);
    _tree_event->SetBranchAddress("jet_ak04its_phi", jet_ak04its_phi);
    _tree_event->SetBranchAddress("jet_ak04its_pt_truth", jet_ak04its_pt_truth);
    _tree_event->SetBranchAddress("jet_ak04its_eta_truth", jet_ak04its_eta_truth);
    _tree_event->SetBranchAddress("jet_ak04its_phi_truth", jet_ak04its_phi_truth);

    //quark-gluon discriminator variables
    _tree_event->SetBranchAddress("jet_ak04its_ptd_raw", jet_ak04its_ptd_raw);
    _tree_event->SetBranchAddress("jet_ak04its_width_sigma", jet_ak04its_width_sigma);
    _tree_event->SetBranchAddress("jet_ak04its_multiplicity_raw", jet_ak04its_multiplicity);



    _tree_event->SetBranchAddress("jet_ak04its_truth_index_z_reco",     jet_ak04its_truth_index_z_reco);
    _tree_event->SetBranchAddress("jet_ak04its_truth_z_reco", jet_ak04its_truth_z_reco);    

    //truth jets
    _tree_event->SetBranchAddress("njet_truth_ak04", &njet_truth_ak04);
    _tree_event->SetBranchAddress("jet_truth_ak04_pt", jet_truth_ak04_pt);
    _tree_event->SetBranchAddress("jet_truth_ak04_phi", jet_truth_ak04_phi);
    _tree_event->SetBranchAddress("jet_truth_ak04_eta", jet_truth_ak04_eta);    
    // Loop over events

    Bool_t isRealData = true;
    _tree_event->GetEntry(1);
    if(nmc_truth>0) isRealData= false;
    Float_t N_SR = 0; //float because it might be weighted in MC
    Float_t N_BR = 0;
    Float_t N_eventpassed = 0;
    Float_t N_truth = 0;
     
 
    std::cout<<" About to start looping over events to get weights" << std::endl;

    TH1D hBR("hBR", "Isolated cluster, bkg region", 40, 10.0, 20.0);
    TH1D hweight("hweight", "Isolated cluster, signal region", 40, 10.0, 20.0);

    if(isRealData){
      for(Long64_t ievent = 0; ievent < _tree_event->GetEntries() ; ievent++){
	    if (ievent % 100000 == 0) std::cout << " event " << ievent << std::endl;

            _tree_event->GetEntry(ievent);
            if(not( TMath::Abs(primary_vertex[2])<10.0)) continue; //vertex z position
            if(is_pileup_from_spd_5_08) continue; //removes pileup
            ULong64_t one1 = 1;
            ULong64_t triggerMask_13data = (one1 << 17) | (one1 << 18) | (one1 << 19) | (one1 << 20); //EG1 or EG2 or EJ1 or EJ2
            if(triggerMask_13data & trigger_mask[0] == 0) continue; //trigger selection

            for (ULong64_t n = 0; n < ncluster; n++) {
                if( not(cluster_pt[n]>10.0)) continue; //select pt of photons
                if( not(cluster_pt[n]<16.0)) continue;
                if( not(cluster_ncell[n]>2)) continue;   //removes clusters with 1 or 2 cells
                if( not(cluster_e_cross[n]/cluster_e[n]>0.05)) continue; //removes "spiky" clusters
                if( not(cluster_nlocal_maxima[n]<= 2)) continue; //require to have at most 2 local maxima.
                if( not(cluster_distance_to_bad_channel[n]>=2.0)) continue;
                if( not(cluster_iso_its_04[n] < 1.0)) continue;

                if(cluster_s_nphoton[n][1] > 0.55 and cluster_s_nphoton[n][1]<0.85){
		    hweight.Fill(cluster_pt[n]);
		}
                else if(cluster_s_nphoton[n][1]<0.30){
		    hBR.Fill(cluster_pt[n]);
		}
	    } //cluster
	}//events
	hweight.Divide(&hBR);
	std::cout << " Weights " << std::endl;
        for(int i=0 ; i< hweight.GetNbinsX() ; i++) std::cout <<" i" << i << " weight= " << hweight.GetBinContent(i) << std::endl;
    }//end loop over events to get weights for background region
    

    std::cout<<" About to start looping over events" << std::endl;

    h_cutflow.GetXaxis()->SetBinLabel(1, "All");
    h_cutflow.GetXaxis()->SetBinLabel(2, "pt<16 GeV");
    h_cutflow.GetXaxis()->SetBinLabel(3, "N_{cell}>2");
    h_cutflow.GetXaxis()->SetBinLabel(4, "E_{cross}/E_{cell}");
    h_cutflow.GetXaxis()->SetBinLabel(5, "NLM <3");
    h_cutflow.GetXaxis()->SetBinLabel(6, "Distance-to-bad chanel>=2");
    h_cutflow.GetXaxis()->SetBinLabel(7, "Isolation < 1 GeV");

    h_evtcutflow.GetXaxis()->SetBinLabel(1, "All");
    h_evtcutflow.GetXaxis()->SetBinLabel(2, "Vertex |z| < 10 cm");
    h_evtcutflow.GetXaxis()->SetBinLabel(3, "Pileup rejection");

    for(Long64_t ievent = 0; ievent < _tree_event->GetEntries() ; ievent++){

      // std::cout << ievent << std::endl;
    //for(Long64_t ievent = 0; ievent < 500000 ; ievent++){
      _tree_event->GetEntry(ievent);
      h_evtcutflow.Fill(0);
      //Eevent Selection: 
      if(not( TMath::Abs(primary_vertex[2])<10.0)) continue; //vertex z position    
      h_evtcutflow.Fill(1);
      if(is_pileup_from_spd_5_08) continue; //removes pileup
      h_evtcutflow.Fill(2);     
      ULong64_t one1 = 1;
      ULong64_t triggerMask_13data = (one1 << 17) | (one1 << 18) | (one1 << 19) | (one1 << 20); //EG1 or EG2 or EJ1 or EJ2
      if(isRealData and (triggerMask_13data & trigger_mask[0]) == 0) continue; //trigger selection
      h_evtcutflow.Fill(3);

      N_eventpassed +=1;


      double weight = 1.0;
      if(not isRealData) weight = eg_cross_section/(double)eg_ntrial;

      h_evt_rho.Fill(ue_estimate_its_const, weight);


      //loop over clusters
      for (ULong64_t n = 0; n < ncluster; n++) {
        //Photon Selection
       
        if( not(cluster_pt[n]>10.0)) continue; //select pt of photons
	h_cutflow.Fill(0);
        if( not(cluster_pt[n]<16.0)) continue;
        h_cutflow.Fill(1);
        if( not(cluster_ncell[n]>2)) continue;   //removes clusters with 1 or 2 cells
        h_cutflow.Fill(2);
	if( not(cluster_e_cross[n]/cluster_e[n]>0.05)) continue; //removes "spiky" clusters
        h_cutflow.Fill(3);
        if( not(cluster_nlocal_maxima[n]< 3)) continue; //require to have at most 2 local maxima.
        h_cutflow.Fill(4);
	if( not(cluster_distance_to_bad_channel[n]>=2.0)) continue;
        h_cutflow.Fill(5);
        if( not(cluster_iso_its_04[n] < 1.0)) continue;
        h_cutflow.Fill(6);
       
	Bool_t isTruePhoton = false;
        Float_t truth_pt = -999.0;
        Float_t truth_eta = -999.0;
        Float_t truth_phi = -999.0;

	for(int counter = 0 ; counter<32; counter++){
	  unsigned short index = cluster_mc_truth_index[n][counter];                   
          if(isTruePhoton) break;
          if(index==65535) continue;
          if(mc_truth_pdg_code[index]!=22) continue;
          if(mc_truth_first_parent_pdg_code[index]!=22) continue;
          if( not (mc_truth_status[index] >0)) continue;        
          isTruePhoton = true;
          truth_pt     = mc_truth_pt[index];
          truth_phi    =  mc_truth_phi[index];
          truth_eta    =  mc_truth_eta[index];
	}//end loop over indices
	
        if((not isRealData) and (not isTruePhoton)) continue;
	
	//start jet loop 

        Bool_t inSignalRegion = false;
        Bool_t inBkgRegion    = false;                
	//if( cluster_lambda_square[n][0]<0.30){
        if(cluster_s_nphoton[n][1] > 0.55 and cluster_s_nphoton[n][1]<0.85){
           N_SR +=weight;
           inSignalRegion = true;
	}
        else if(cluster_s_nphoton[n][1]<0.30){
            double bkg_weight = hweight.GetBinContent(hweight.FindBin(cluster_pt[n]));
	    //std::cout << bkg_weight << " " << cluster_pt[n] << std::endl;
	    weight = weight*bkg_weight; //pt-dependent weight for background;					      
            N_BR +=weight;
            inBkgRegion = true;
	}

        Int_t njets_SR = 0; 
        Int_t njets_BR = 0;
	for (ULong64_t ijet = 0; ijet < njet_ak04its; ijet++) { //start loop over jets
          if(not (jet_ak04its_pt_raw[ijet]>5)) continue; 
          if(not (TMath::Abs(jet_ak04its_eta_raw[ijet]) <0.5)) continue;
          Float_t dphi = TMath::Abs(TVector2::Phi_mpi_pi(jet_ak04its_phi[ijet] - cluster_phi[n]));
	  Float_t deta = jet_ak04its_eta_raw[ijet] - cluster_eta[n];
	  Float_t dphi_truth = 0;
          Float_t deta_truth = 0;

          if(eg_ntrial>0){
              dphi_truth = TMath::Abs(TVector2::Phi_mpi_pi(jet_ak04its_phi_truth[ijet] - truth_phi));
	      deta_truth = jet_ak04its_eta_truth[ijet] -  truth_eta;
	  }
          if(inSignalRegion){
	    hSR_dPhi.Fill(dphi,weight);
            hSR_dPhi_truth.Fill(dphi_truth,weight);
	 
	  }
	  else if(inBkgRegion){
	    hBR_dPhi.Fill(dphi,weight);
            hBR_dPhi_truth.Fill(dphi_truth,weight);
         
	  }
 	  if(not (dphi>0.4)) continue; 

          //counts jets associated with clusters   
          if(inSignalRegion){
	    njets_SR =+1 ;
	  }
          else if(inBkgRegion){
            njets_BR =+1; 
	  }
          

	  Float_t xj = jet_ak04its_pt_raw[ijet]/cluster_pt[n];
	  //std::cout <<"truthptjet: " << jet_ak04its_pt_truth[ijet] << "reco pt jet" << jet_ak04its_pt_raw[ijet] << std::endl;           
          Float_t xj_truth = jet_ak04its_pt_truth[ijet]/truth_pt; 
                     
	  if( inSignalRegion){
            hSR_Xj.Fill(xj, weight);
	    hSR_dEta.Fill(deta, weight);
            hSR_AvgEta.Fill(0.5*(jet_ak04its_eta_raw[ijet] + cluster_eta[n]), weight);

            hSR_pTD.Fill(jet_ak04its_ptd_raw[ijet],weight);
	    //	    std::cout << jet_ak04its_multiplicity[ijet] << " " << jet_ak04its_width_sigma[ijet] << " " << jet_ak04its_ptd_raw[ijet] << std::endl;
            hSR_Multiplicity.Fill(jet_ak04its_multiplicity[ijet],weight);
            hSR_jetwidth.Fill(TMath::Log(jet_ak04its_width_sigma[ijet]), weight);

            //associated jet rate
	    hSR_jetpt.Fill(jet_ak04its_pt_raw[ijet], weight);
	    hSR_jeteta.Fill(jet_ak04its_eta_raw[ijet], weight);
	    hSR_jetphi.Fill(jet_ak04its_phi[ijet],weight);

	    hSR_jetpt_truth.Fill(jet_ak04its_pt_truth[ijet], weight);
            hSR_jeteta_truth.Fill(jet_ak04its_eta_truth[ijet], weight);
            hSR_jetphi_truth.Fill(jet_ak04its_phi_truth[ijet],weight);

            hSR_Xj_truth.Fill(xj_truth, weight);
            hSR_dEta_truth.Fill(deta_truth,weight);
            hSR_AvgEta_truth.Fill(0.5*(jet_ak04its_eta_truth[ijet] +  truth_eta), weight);

            h_Xj_Matrix.Fill(xj_truth, xj, weight);
	    //std::cout<<" xj " << xj << " " << " xj_truth "<< xj_truth << std::endl;

	  }
          else if(inBkgRegion){
	    hBR_Xj.Fill(xj,weight);
	    hBR_dEta.Fill(deta,weight);
	    hBR_AvgEta.Fill(0.5*(jet_ak04its_eta_raw[ijet] + cluster_eta[n]), weight);

	    hBR_pTD.Fill(jet_ak04its_ptd_raw[ijet],weight);
	    hBR_Multiplicity.Fill(jet_ak04its_multiplicity[ijet],weight);
            hBR_jetwidth.Fill(TMath::Log(jet_ak04its_width_sigma[ijet]), weight);

	    //Associated jet rate
	    hBR_jetpt.Fill(jet_ak04its_pt_raw[ijet], weight);
	    hBR_jeteta.Fill(jet_ak04its_eta_raw[ijet], weight);
            hBR_jetphi.Fill(jet_ak04its_phi[ijet],weight);

	    hBR_jetpt_truth.Fill(jet_ak04its_pt_truth[ijet], weight);
            hBR_jeteta_truth.Fill(jet_ak04its_eta_truth[ijet], weight);
            hBR_jetphi_truth.Fill(jet_ak04its_phi_truth[ijet],weight);

            hBR_Xj_truth.Fill(xj_truth,weight);
	    hBR_dEta_truth.Fill(deta_truth,weight);
	    hSR_AvgEta_truth.Fill(0.5*(jet_ak04its_eta_truth[ijet] +  truth_eta), weight);
	  }



	}//end loop over jets

        if(inSignalRegion){
	  hSR_clusterpt.Fill(cluster_pt[n], weight);
	  hSR_clustereta.Fill(cluster_eta[n], weight);
	  hSR_clusterphi.Fill(cluster_phi[n], weight);
          hSR_njet.Fill(njets_SR, weight); 
	}
        else if(inBkgRegion){
	  hBR_clusterpt.Fill(cluster_pt[n], weight);
          hBR_clustereta.Fill(cluster_eta[n], weight);
          hBR_clusterphi.Fill(cluster_phi[n], weight);
	  hBR_njet.Fill(njets_BR, weight);
	}
	//fill in this histogram only photons that can be traced to a generated non-decay photon.	
        h_reco_truthpt.Fill(truth_pt,weight);
        h_reco.Fill(cluster_pt[n],weight); 

      }//end loop on clusters


      //** Study of jet reconstruction efficiency:

      //loop over truth jets
      for (ULong64_t ijet = 0; ijet < njet_truth_ak04; ijet++) {
	if(not(TMath::Abs(jet_truth_ak04_eta[ijet])<0.5)) continue;
         h_jetpt_truth.Fill(jet_truth_ak04_pt[ijet], weight);
      }
    
      std::set<int> temp; //to store truth indices associated with reco jets
      for (ULong64_t ijet = 0; ijet < njet_ak04its; ijet++) { 
	if(not (jet_ak04its_pt_raw[ijet]>5)) continue;
	if(not (TMath::Abs(jet_ak04its_eta_raw[ijet])  <0.5 ) ) continue;
	h_jetpt_reco.Fill(jet_ak04its_pt_raw[ijet], weight);
	temp.insert(jet_ak04its_truth_index_z_reco[ijet][0]);
      } //end loop over reco jets
      for(auto& index: temp){
	if(index>0){
	  if(not(TMath::Abs(jet_truth_ak04_eta[index])<0.5)) continue;
          h_jetpt_truthreco.Fill(jet_truth_ak04_pt[index],weight);
	}
      }//end loop over indices of reco jets

      for (ULong64_t nmc = 0; nmc < nmc_truth; nmc++) {
        if(not(mc_truth_pt[nmc]>10.0)) continue;
	if(not(mc_truth_pt[nmc]<16.0)) continue;
	if(mc_truth_pdg_code[nmc]==22 && int(mc_truth_status[nmc])>0 &&  mc_truth_first_parent_pdg_code[nmc]==22){
	  //std::cout << mc_truth_pt[nmc] << "phi " << mc_truth_phi[nmc] << " eta " << mc_truth_eta[nmc] << 
	  //  " code: " << mc_truth_pdg_code[nmc] << " status " << int(mc_truth_status[nmc]) << " parentpdg " << mc_truth_first_parent_pdg_code[nmc] << std::endl;    
	  h_truth.Fill(mc_truth_pt[nmc],weight);
          N_truth +=1;
	  //std::cout << " number of truth jets " << njet_truth_ak04 << std::endl;
	  for (ULong64_t ijet = 0; ijet < njet_truth_ak04; ijet++) {
            if(not(jet_truth_ak04_pt[ijet]>5.0)) continue;
	    if(not(TMath::Abs(jet_truth_ak04_eta[ijet])<0.5)) continue;
	    Float_t dphi_truth = TMath::Abs(TVector2::Phi_mpi_pi(jet_truth_ak04_phi[ijet] - mc_truth_phi[nmc]));
	    //std::cout<< dphi_truth << std::endl;
            
	    h_dPhi_truth.Fill(dphi_truth,weight);
            if( not(dphi_truth>0.4)) continue;
	    Float_t xj_truth = jet_truth_ak04_pt[ijet]/mc_truth_pt[nmc];
            h_Xj_truth.Fill(xj_truth,weight);
	  }//end loop over truth jets
	}
      }//end loop over mc particles

     // Create the file label, to be used within the filenames, to represent the source file
     

      h_truth.SetLineColor(2);
      THStack* hs = new THStack("hs","stack histo for plotting");
      hs->Add(&h_reco);
      hs->Add(&h_truth);

      if (ievent % 10000 == 0) {
	//SR_Xj.Draw("e1x0nostack");
        hSR_dPhi.Draw("e1x0nostack");
	std::cout << ievent << " " << _tree_event->GetEntries() << std::endl;
        canvas->Update();
      } 

     }//end over events

    std::cout << " Numbers of events passing selection " << N_eventpassed << std::endl;
    std::cout << " Number of clusters in signal region " << N_SR << std::endl;
    std::cout << " Number of clusters in background region " << N_BR << std::endl;
    std::cout << " Number of truth photons " << N_truth << std::endl;

    std::string opened_files = "";
    //  for (int iarg = 1; iarg < argc; iarg++) {
    std::string filepath = argv[iarg];    
    opened_files = "_" + filepath.substr(filepath.find_last_of("/")+1, filepath.find_last_of(".")-filepath.find_last_of("/")-1);
    
    TFile* fout = new TFile(Form("GammaJet_config%s.root", opened_files.c_str()),"RECREATE");
    fout->Print();
    hSR_njet.Scale(1.0/N_SR);
    hBR_njet.Scale(1.0/N_BR);

    hSR_Xj.Scale(1.0/N_SR);
    hBR_Xj.Scale(1.0/N_BR);
    hSR_dPhi.Scale(1.0/N_SR);
    hBR_dPhi.Scale(1.0/N_BR);
    hSR_dEta.Scale(1.0/N_SR);
    hBR_dEta.Scale(1.0/N_BR);

    hSR_AvgEta.Scale(1.0/N_SR);
    hBR_AvgEta.Scale(1.0/N_BR);

    hSR_jetpt.Scale(1.0/N_SR);
    hBR_jetpt.Scale(1.0/N_BR);
    hSR_jeteta.Scale(1.0/N_SR);
    hBR_jeteta.Scale(1.0/N_BR);
    hSR_jetphi.Scale(1.0/N_SR);
    hBR_jetphi.Scale(1.0/N_BR);


    hSR_jetpt_truth.Scale(1.0/N_SR);
    hBR_jetpt_truth.Scale(1.0/N_BR);
    hSR_jeteta_truth.Scale(1.0/N_SR);
    hBR_jeteta_truth.Scale(1.0/N_BR);
    hSR_jetphi_truth.Scale(1.0/N_SR);
    hBR_jetphi_truth.Scale(1.0/N_BR);


    hSR_Xj_truth.Scale(1.0/N_SR);
    hBR_Xj_truth.Scale(1.0/N_BR);
    hSR_dPhi_truth.Scale(1.0/N_SR);
    hBR_dPhi_truth.Scale(1.0/N_BR);
    hSR_dEta_truth.Scale(1.0/N_SR);
    hBR_dEta_truth.Scale(1.0/N_BR);
    hSR_AvgEta_truth.Scale(1.0/N_SR);
    hBR_AvgEta_truth.Scale(1.0/N_BR);


    hSR_pTD.Scale(1.0/N_SR);
    hBR_pTD.Scale(1.0/N_BR);
    hSR_Multiplicity.Scale(1.0/N_SR);
    hBR_Multiplicity.Scale(1.0/N_BR);
    hSR_jetwidth.Scale(1.0/N_SR);
    hBR_jetwidth.Scale(1.0/N_BR);


    h_dPhi_truth.Scale(1.0/N_truth);
    h_Xj_truth.Scale(1.0/N_truth);

    h_evtcutflow.Write("EventCutFlow");
    h_cutflow.Write("ClusterCutFlow");
    h_evt_rho.Write("h_evt_rho");

    //number of jets
    hSR_njet.Write("hSR_njet");
    hBR_njet.Write("hBR_njet");

    //Xj
    hSR_Xj.Write("hSR_Xj");
    hBR_Xj.Write("hBR_Xj");
    hSR_Xj_truth.Write("hSR_Xj_truth");
    hBR_Xj_truth.Write("hBR_Xj_truth");
    //dPhi
    hSR_dPhi.Write("hSR_dPhi");
    hBR_dPhi.Write("hBR_dPhi");
    hSR_dPhi_truth.Write("hSR_dPhi_truth");
    hBR_dPhi_truth.Write("hBR_dPhi_truth");
    //dEta
    hBR_dEta.Write("hBR_dEta"); 
    hSR_dEta.Write("hSR_dEta");
    hBR_dEta_truth.Write("hBR_dEta_truth");
    hSR_dEta_truth.Write("hSR_dEta_truth");
    //Average Eta  
    hBR_AvgEta.Write("hBR_AvgEta");
    hSR_AvgEta.Write("hSR_AvgEta");
    hBR_AvgEta_truth.Write("hBR_AvgEta_truth");
    hSR_AvgEta_truth.Write("hSR_AvgEta_truth");

    //Flavor variables
    hSR_pTD.Write("hSR_pTD");
    hBR_pTD.Write("hBR_pTD");
    hSR_Multiplicity.Write("hSR_Multiplicity");
    hBR_Multiplicity.Write("hBR_Multiplicity");
    hSR_jetwidth.Write("hSR_jetwidth");
    hBR_jetwidth.Write("hBR_jetwidth");
    //Associated jet histograms    
    hSR_jetpt.Write("hSR_jetpt");
    hBR_jetpt.Write("hBR_jetpt");
    hSR_jetpt_truth.Write("hSR_jetpt_truth");
    hBR_jetpt_truth.Write("hBR_jetpt_truth");

    hSR_jeteta.Write("hSR_jeteta");
    hBR_jeteta.Write("hBR_jeteta");
    hSR_jeteta_truth.Write("hSR_jeteta_truth");
    hBR_jeteta_truth.Write("hBR_jeteta_truth");

    hSR_jetphi.Write("hSR_jetphi");
    hBR_jetphi.Write("hBR_jetphi");
    hSR_jetphi_truth.Write("hSR_jetphi_truth");
    hBR_jetphi_truth.Write("hBR_jetphi_truth");


    //Cluster pt
    hSR_clusterpt.Write("hSR_clusterpt");
    hBR_clusterpt.Write("hBR_clusterpt");
    hSR_clustereta.Write("hSR_clustereta");
    hBR_clustereta.Write("hBR_clustereta");
    hSR_clusterphi.Write("hSR_clusterphi");
    hBR_clusterphi.Write("hBR_clusterphi");

    //MC truth 
    h_dPhi_truth.Write("h_dPhi_truth");
    h_Xj_Matrix.Write("xj_matrix");    
    h_Xj_truth.Write("h_Xj_truth");
    h_jetpt_truth.Write("h_jetpt_truth");
    h_jetpt_truthreco.Write("h_jetpt_truthreco");
    h_jetpt_reco.Write("h_jetpt");
   
    TH1D* jet_eff = (TH1D*)h_jetpt_truthreco.Clone();
    jet_eff->Divide(&h_jetpt_truth);
    jet_eff->Write("jetefficiency");
 
    std::cout << " ending " << std::endl;
    fout->Close();
  }//end of arguments
  return EXIT_SUCCESS;
 
}
