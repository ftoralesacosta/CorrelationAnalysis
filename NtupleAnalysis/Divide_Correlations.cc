#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TH2D.h>
#include <TLine.h>
#include <TLegend.h>
#include <THStack.h>
#include <TProfile.h>
#include <iostream>
#include <fstream>

//int main(int argc, char *argv[])
int main()
{
//    if (argc < 3){
//     std::cout<<"Syntax is [Command] [Same_Event_root_file] [Mixed_Event_root_file]"<<std::endl;
//     exit(EXIT_FAILURE);
//   }

  //TFile *corr = TFile::Open((TString)argv[1]);
  TFile *corr = TFile::Open("fout_lowDNN.root");
  if (corr == NULL) {
    std::cout << "file 1 fail" << std::endl;
    exit(EXIT_FAILURE);
    }

  TFile *mix = TFile::Open("fout_lowDNN_Mix.root");
  if (mix == NULL) {
    std::cout << " file 2 fail" << std::endl;
    exit(EXIT_FAILURE);
  }

  int nztbins = 7;
  float* ztbins;
  ztbins = new float[nztbins+1];
  ztbins[0] = 0.05; ztbins[1] = 0.1; ztbins[2] = 0.2; ztbins[3] = 0.4; ztbins[4] = 0.6; ztbins[5] = 0.8; ztbins[6] = 1.0; ztbins[7] = 1.2;

  int nptbins = 3;
  float* ptbins;
  ptbins = new float[nptbins+1];
  ptbins[0] = 10.0; ptbins[1] = 11; ptbins[2] = 12.5; ptbins[3] = 16;

  TH2F* Corr_Iso_same[nztbins*nptbins]; 
  TH2F* Corr_Iso_mix[nztbins*nptbins];

  TH2F* Corr_AntiIso_same[nztbins*nptbins];
  TH2F* Corr_AntiIso_mix[nztbins*nptbins];

  TH1D* IsoPhiProj[nztbins*nptbins];
  TH1D* AntiIsoPhiProj[nztbins*nptbins];

  TH1D* IsoPhiProj_LargeEta[nztbins*nptbins];
  TH1D* AntiIsoPhiProj_LargeEta[nztbins*nptbins];

  TH1D* N_Iso_Triggers[nptbins];
  TH1D* N_BKGD_Triggers[nptbins];
  //Grab Histos, correct with mixed, project, get pedestal

  for(int ipt = 0; ipt < nptbins; ipt++){
    for (int izt = 0; izt<nztbins; izt++){

    //Corr_Iso_same[izt] = (TH2F*)corr->Get(Form("IsoCorrelation_ztmin%1.0f_ztmax%1.0f",10*ztbins[izt],10*ztbins[izt+1]));
      Corr_Iso_same[izt+ipt*nztbins] = (TH2F*)corr->Get(Form("IsoCorrelation_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
							     ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));

      if (Corr_Iso_same[izt+ipt*nztbins] == NULL) {
	std::cout << "tree 1 fail" << std::endl;
	exit(EXIT_FAILURE);}
      
      Corr_Iso_mix[izt+ipt*nztbins] = (TH2F*)mix->Get(Form("Correlation_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
					       ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));   

      if (Corr_Iso_mix[izt+ipt*nztbins] == NULL) {
	std::cout << "tree 2 fail" << std::endl;
	exit(EXIT_FAILURE);
      }
      
      Corr_AntiIso_same[izt+ipt*nztbins] = (TH2F*)corr->Get(Form("AntiIsoCorrelation_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
						     ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
      if (Corr_AntiIso_same[izt+ipt*nztbins] == NULL) {
	std::cout << "tree 1 fail" << std::endl;
	exit(EXIT_FAILURE);}

      //FIXME:Just set one mixed correlation function to be all clusters withouth iosalation or DNN      
      Corr_AntiIso_mix[izt+ipt*nztbins] = (TH2F*)mix->Get(Form("Correlation_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
						   ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
      if (Corr_AntiIso_mix[izt+ipt*nztbins] == NULL) {
	std::cout << "tree 2 fail" << std::endl;
	exit(EXIT_FAILURE);
      }

    //Normalize Mixing to 1 at ∆eta∆phi = 0,0
    TAxis *mix_xaxis = Corr_Iso_mix[izt+ipt*nztbins]->GetXaxis();
    Int_t mix_bin_phi = mix_xaxis->FindBin(0.0);
    TAxis *mix_yaxis = Corr_Iso_mix[izt+ipt*nztbins]->GetYaxis();
    Int_t mix_bin_eta = mix_yaxis->FindBin(0.0);
    Double_t mix_Iso_integ = Corr_Iso_mix[izt+ipt*nztbins]->Integral(mix_bin_phi,mix_bin_phi,mix_bin_eta,mix_bin_eta);
    //Double_t mix_Iso_integ = Corr_Iso_mix[izt+ipt*nztbins]->GetBinContent(mix_bin_phi,mix_bin_eta,mix_bin_eta);
    std::cout<<"Iso Mix Integ: "<<mix_Iso_integ<<std::endl;
    Double_t Iso_norm = 1.0/mix_Iso_integ;
    Corr_Iso_mix[izt+ipt*nztbins]->Scale(Iso_norm);

    Double_t mix_AntiIso_integ = Corr_AntiIso_mix[izt+ipt*nztbins]->Integral(mix_bin_phi,mix_bin_phi,mix_bin_eta,mix_bin_eta);
    std::cout<<"Anti Iso Mix Integral: "<<mix_AntiIso_integ<<std::endl;
    Double_t AntiIso_norm = 1.0/mix_AntiIso_integ;
    Corr_AntiIso_mix[izt+ipt*nztbins]->Scale(AntiIso_norm);

    //DIVIDE MIXING
//     Corr_Iso_same[izt+ipt*nztbins]->Divide(Corr_Iso_mix[izt+ipt*nztbins]);
//     Corr_AntiIso_same[izt+ipt*nztbins]->Divide(Corr_AntiIso_mix[izt+ipt*nztbins]);
//     std::cout<<"Got through divide "<<std::endl;

    //Phi Projection
    TAxis *same_Iso_yaxis = Corr_Iso_same[izt+ipt*nztbins]->GetYaxis();
    Int_t bin_eta_neg = same_Iso_yaxis->FindBin(-0.6);
    Int_t bin_eta_pos = same_Iso_yaxis->FindBin(0.6);
    
    IsoPhiProj[izt+ipt*nztbins] = Corr_Iso_same[izt+ipt*nztbins]->ProjectionX(
    Form("Iso_Phi_Projection_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
    ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]),bin_eta_neg,bin_eta_pos);

    AntiIsoPhiProj[izt+ipt*nztbins] = Corr_AntiIso_same[izt+ipt*nztbins]->ProjectionX(
    Form("Anti_Phi_Projection_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
    ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]),bin_eta_neg,bin_eta_pos);

    //Phi Projection at large Eta
    Int_t Large_bin_eta_neg = same_Iso_yaxis->FindBin(-1.4);
    Int_t Large_bin_eta_pos = same_Iso_yaxis->FindBin(1.4);
    IsoPhiProj_LargeEta[izt+ipt*nztbins] =Corr_Iso_same[izt+ipt*nztbins]->ProjectionX(
    Form("Iso_Phi_LargeEta_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
    ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]),Large_bin_eta_neg,Large_bin_eta_pos);

    AntiIsoPhiProj_LargeEta[izt+ipt*nztbins] = Corr_AntiIso_same[izt+ipt*nztbins]->ProjectionX(
    Form("Anti_Phi_LargeEta_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
    ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]),Large_bin_eta_neg,Large_bin_eta_pos);

    Int_t Large_bin_eta_temp1 = same_Iso_yaxis->FindBin(-0.8);
    Int_t Large_bin_eta_temp2 = same_Iso_yaxis->FindBin(0.8);

    TH1D* TempIso = Corr_Iso_same[izt+ipt*nztbins]->ProjectionX("temprI",Large_bin_eta_temp1,Large_bin_eta_temp2);
    IsoPhiProj_LargeEta[izt+ipt*nztbins]->Add(TempIso,-1);

    TH1D* TempAntiIso = Corr_AntiIso_same[izt+ipt*nztbins]->ProjectionX("temprA",Large_bin_eta_temp1,Large_bin_eta_temp2);
    AntiIsoPhiProj_LargeEta[izt+ipt*nztbins]->Add(TempAntiIso,-1);

    std::cout<<"Entries: "<<Corr_Iso_same[izt+ipt*nztbins]->GetEntries()<<std::endl;
    TempIso->Delete();
    TempAntiIso->Delete();
    
    }//zt bin
    N_Iso_Triggers[ipt] = (TH1D*)corr->Get(Form("N_Iso_Trig_ptmin%1.0f_ptmax%1.0f",ptbins[ipt],ptbins[ipt+1]));
    N_BKGD_Triggers[ipt] = (TH1D*)corr->Get(Form("N_Low_DNN_Triggers_ptmin%1.0f_ptmax%1.0f",ptbins[ipt],ptbins[ipt+1]));
  }
  
  TFile *MyFile = new TFile("Ratio_Correlation.root","RECREATE");
  TCanvas *test = new TCanvas("","test");

  {
    float N_Isos[nptbins];
    float N_BKGD[nptbins];
    float npts[nptbins];
    
    for (int ipt = 0; ipt < nptbins; ipt ++){
      std::cout<<"Iso_Triggers between "<<ptbins[ipt]<<" and "<<ptbins[ipt+1]<<": "<<N_Iso_Triggers[ipt]->GetEntries()<<std::endl;
      std::cout<<"BKGD_Triggers between"<<ptbins[ipt]<<" and "<<ptbins[ipt+1]<<": "<<N_BKGD_Triggers[ipt]->GetEntries()<<std::endl;
      N_Isos[ipt] = N_Iso_Triggers[ipt]->GetEntries();
      N_BKGD[ipt] = N_BKGD_Triggers[ipt]->GetEntries();
      npts[ipt] = ptbins[ipt];
    }
    
    TGraph* gr1 = new TGraph(3,npts,N_Isos);
    gr1->SetTitle("N Triggers vs p_{T} bin");
    gr1->SetMarkerStyle(7);
    gr1->GetHistogram()->SetMaximum(30000);
    gr1->GetHistogram()->SetMinimum(0.0);
    TGraph* gr2 = new TGraph(3,npts,N_BKGD);
    //gr2->SetLineColor(kRed);
    gr2->SetMarkerStyle(7);
    gr2->SetLineColor(kRed);
    gr1->Draw();
    gr2->Draw("same");
    
    test->SaveAs("pics/N_Triggers.png");
  }

  //SET TITLE NAMES FOR PLOTTING

  for (int ipt = 0; ipt <nptbins; ipt++){
    TCanvas *Isocanvas = new TCanvas("Isocanv","Isocanvas",1350,1150); 
    Isocanvas->Divide(3,2);
    for (int izt = 0; izt<nztbins; izt++){
      //for (int izt = 0; izt<4; izt++){
      //if(izt == 0) continue;
      gStyle->SetOptStat("");
      gPad->SetLeftMargin(0.17);
      gPad->SetRightMargin(0.0);

      Corr_Iso_same[izt+ipt*nztbins]->SetTitle(Form("Iso #gamma-h: %1.1f < p_{T}^{Clus.} <%1.1f GeV, %1.2f < z_{T} < %1.2f, 0.55 < DNN < 0.85",
						    ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));
      Corr_Iso_same[izt+ipt*nztbins]->GetXaxis()->SetTitle("#Delta #phi");
      Corr_Iso_same[izt+ipt*nztbins]->GetYaxis()->SetTitle("#Delta #eta");
      Corr_Iso_same[izt+ipt*nztbins]->GetZaxis()->SetTitle("#frac{d^{2}N}{d#phi d#eta}");
      Corr_Iso_same[izt+ipt*nztbins]->GetZaxis()->SetTitleOffset(2.5);
      Corr_Iso_same[izt+ipt*nztbins]->Write(Form("LargeDNN_Correlation_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
						 ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
      Isocanvas->cd(izt+1);
      Corr_Iso_same[izt+ipt*nztbins]->Draw("SURF2");
    }
    Isocanvas->SaveAs(Form("pics/Large_Corr2D_pt_%1.0f_%1.0f.png",ptbins[ipt],ptbins[ipt+1]));
    Isocanvas->Clear();
    //FIXME: Maybe change to 1.0f
  }


  for (int ipt = 0; ipt < nptbins; ipt++){
    TCanvas *AntiIsocanvas = new TCanvas("AntiIsocanv","AntiIsocanvas",1350,950);
    AntiIsocanvas->Divide(3,2);
    for (int izt = 0; izt<nztbins-1; izt++){
      gStyle->SetOptStat("");
      gPad->SetLeftMargin(0.17);
      gPad->SetRightMargin(0.0);
      
      Corr_AntiIso_same[izt+ipt*nztbins]->SetTitle(
      Form("~Background #gamma-h: %1.1f < p_{T}^{Clus.} <%1.1f GeV, %1.2f < z_{T} < %1.2f, 0.0 < DNN < 0.3",
      ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));
      
      Corr_AntiIso_same[izt+ipt*nztbins]->GetXaxis()->SetTitle("#Delta #phi");
      Corr_AntiIso_same[izt+ipt*nztbins]->GetYaxis()->SetTitle("#Delta #eta");
      Corr_AntiIso_same[izt+ipt*nztbins]->GetZaxis()->SetTitle("#frac{1}{N_{trig}}#frac{d^{2}N}{d#phi d#eta}");
      Corr_AntiIso_same[izt+ipt*nztbins]->GetZaxis()->SetTitleOffset(2.5);
      AntiIsocanvas->cd(izt+1);
      Corr_AntiIso_same[izt+ipt*nztbins]->Draw("SURF2");
      Corr_AntiIso_same[izt+ipt*nztbins]->Write(Form("SmallDNN_Correlation_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
						     ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
    }
    AntiIsocanvas->SaveAs(Form("pics/Small_DNNCorr2D_pt_%1.0f_%1.0f.png",ptbins[ipt],ptbins[ipt+1]));
    AntiIsocanvas->Clear();
  }

  for (int ipt = 0; ipt < nptbins; ipt++){
    for (int izt = 0; izt<nztbins; izt++){
      IsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("Isolated #Delta#phi Projection  %1.1f < p_{T}^{Clus.} <%1.1f GeV  %1.2f < z_{T} < %1.2f, 0.55 < DNN < 0.85, |#Delta#eta| < 0.6",
      ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
      
      IsoPhiProj[izt+ipt*nztbins]->SetMinimum(0.0);
      IsoPhiProj[izt+ipt*nztbins]->Write(Form("LargeDNN_Phi_Projection_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
					      ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
    }
  }

  for (int ipt = 0; ipt < nptbins; ipt++){
    for (int izt = 0; izt<nztbins; izt++){
      AntiIsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("~Background #Delta#phi %1.1f < p_{T}^{Clus.} <%1.1f GeV %1.2f < z_{T} < %1.2f, 0.0 < DNN < 0.3, |#Delta#eta| < 0.6", 
      ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));

      AntiIsoPhiProj[izt+ipt*nztbins]->SetMinimum(0.0);
      AntiIsoPhiProj[izt+ipt*nztbins]->Write(Form("SmallDNN_Phi_Projection_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
						  ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
    }
  }

  for (int ipt = 0; ipt < nptbins; ipt++){
    for (int izt = 0; izt<nztbins; izt++){
      IsoPhiProj_LargeEta[izt+ipt*nztbins]->SetTitle(
      Form("Isolated #Delta#phi %1.1f < p_{T}^{Clus.} <%1.1f GeV %1.2f < z_{T} < %1.2f, 0.55 < DNN < 0.85, 0.8 < |#Delta#eta| < 1.4",
      ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));

      IsoPhiProj_LargeEta[izt+ipt*nztbins]->SetMinimum(0.0);
      //IsoPhiProj_LargeEta[izt+ipt*nztbins]->Write(Form("LargeDNN_Large_Eta_Phi_Projection_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
      //						       ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
    }
  }
  
  for (int ipt = 0; ipt < nptbins; ipt++){
    for (int izt = 0; izt<nztbins; izt++){
      AntiIsoPhiProj_LargeEta[izt+ipt*nztbins]->SetTitle(
      Form("~Background #Delta#phi %1.1f < p_{T}^{Clus.} <%1.1f GeV %1.2f < z_{T} < %1.2f, 0.0 < DNN < 0.3, 0.8 < |#Delta#eta| < 1.4",
      ptbins[ipt],ptbins[ipt+1], 10*ztbins[izt],10*ztbins[izt+1]));

      AntiIsoPhiProj_LargeEta[izt+ipt*nztbins]->SetMinimum(0.0);
      //      AntiIsoPhiProj_LargeEta[izt+ipt*nztbins]->Write();
    }
  }

  //PHI PROJECTION, PER TRIGGER SCALING AND PLOT ZYAM

  for (int ipt = 0; ipt < nptbins; ipt++){

    TCanvas *canvas = new TCanvas("canv","canvas",2000,1500);
    gStyle->SetOptStat("");
    canvas->Divide(4,3);
    
    double ntriggers = N_Iso_Triggers[ipt]->GetEntries();
    double nbkgd = N_BKGD_Triggers[ipt]->GetEntries();

    for (int izt = 0; izt<nztbins-1; izt++){
      canvas->cd(izt*2+1);
      gPad->SetLeftMargin(0.15);
      gPad->SetRightMargin(0.0);
      gPad->SetBottomMargin(0.1);

      //PER TRIGGER SCALING

      IsoPhiProj[izt+ipt*nztbins]->Scale(1.0/1.2);
      IsoPhiProj_LargeEta[izt+ipt*nztbins]->Scale(1.0/1.2);
      IsoPhiProj[izt+ipt*nztbins]->Scale(1.0/ntriggers);

      AntiIsoPhiProj[izt+ipt*nztbins]->Scale(1/1.2);
      AntiIsoPhiProj_LargeEta[izt+ipt*nztbins]->Scale(1/1.2);
      AntiIsoPhiProj[izt+ipt*nztbins]->Scale(1.0/nbkgd);

      float max1 = IsoPhiProj[izt+ipt*nztbins]->GetMaximum();
      float max2 = AntiIsoPhiProj[izt+ipt*nztbins]->GetMaximum();
      float Ymax = 1.2*TMath::Max(max1,max2);

      IsoPhiProj_LargeEta[izt+ipt*nztbins]->SetLineColor(kRed);
      IsoPhiProj[izt+ipt*nztbins]->SetMarkerStyle(1);
      IsoPhiProj[izt+ipt*nztbins]->SetMarkerSize(1);
      IsoPhiProj[izt+ipt*nztbins]->SetMarkerColor(kBlue);
      IsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("Iso #gamma-h: %1.1f < p_{T}^{Clus.} <%1.1f GeV,  %1.2f < z_{T} < %1.2f, 0.55 < DNN < 0.85", 
      ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));
      
      IsoPhiProj[izt+ipt*nztbins]->SetXTitle("d#phi [rad]");
      IsoPhiProj[izt+ipt*nztbins]->SetYTitle("#frac{1}{N_{trig}} #frac{d^{2}N}{d#phid#eta}"); //dEta bc scale by eta units
      IsoPhiProj[izt+ipt*nztbins]->GetXaxis()->CenterTitle();
      IsoPhiProj[izt+ipt*nztbins]->GetYaxis()->CenterTitle();
      IsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetTitleOffset(2.2);
      
      IsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetRangeUser(0,Ymax);
      IsoPhiProj[izt+ipt*nztbins]->Draw();
      //IsoPhiProj_LargeEta[izt+ipt*nztbins]->Draw("same");
      
      TLegend *Iso_legend = new TLegend(0.75,0.75,0.95,0.85);
      Iso_legend->AddEntry(IsoPhiProj[izt+ipt*nztbins],"|#Delta#eta| < 0.6","p");
      //Iso_legend->AddEntry(IsoPhiProj_LargeEta[izt+ipt*nztbins],"0.6 < |#Delta#eta| < 1.4","l");      
      
      //ZYAM
      float Iso_ZYAM_Int = (IsoPhiProj[izt+ipt*nztbins]->Integral(11,13))/3;
      TLine *Iso_ZYAM = new TLine(-M_PI/2,Iso_ZYAM_Int,3*M_PI/2,Iso_ZYAM_Int);
      Iso_ZYAM->SetLineColorAlpha(kRed, 0.9);
      Iso_ZYAM->SetLineWidth(4);
      Iso_ZYAM->Draw("same");
      IsoPhiProj[izt+ipt*nztbins]->Draw("same");
      
      Iso_legend->AddEntry(Iso_ZYAM,"ZYAM","l");
      Iso_legend->Draw();
      
      canvas->cd(izt*2+2);
      gPad->SetLeftMargin(0.0);
      gPad->SetRightMargin(0.15);
      
      AntiIsoPhiProj_LargeEta[izt+ipt*nztbins]->SetLineColor(kRed);
      AntiIsoPhiProj[izt+ipt*nztbins]->SetMarkerStyle(1);
      AntiIsoPhiProj[izt+ipt*nztbins]->SetMarkerSize(1);
      AntiIsoPhiProj[izt+ipt*nztbins]->SetMarkerColor(kBlue);
      AntiIsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("~Background #gamma-h: %1.1f < p_{T}^{Clus.} <%1.1f GeV, %1.2f < z_{T} < %1.2f, 0.0 < DNN < 0.3",
      ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));
      
      AntiIsoPhiProj[izt+ipt*nztbins]->SetXTitle("d#phi [rad]");
      AntiIsoPhiProj[izt+ipt*nztbins]->SetYTitle("1/N_{trigger} #frac{d^{2}N}{d#phid#eta}"); //dEta b/c scale by eta bins
      AntiIsoPhiProj[izt+ipt*nztbins]->SetYTitle("");
      AntiIsoPhiProj[izt+ipt*nztbins]->GetXaxis()->CenterTitle();
      AntiIsoPhiProj[izt+ipt*nztbins]->GetYaxis()->CenterTitle();
      
      AntiIsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetRangeUser(0,Ymax);
      AntiIsoPhiProj[izt+ipt*nztbins]->Draw();
      //AntiIsoPhiProj_LargeEta[izt+ipt*nztbins]->Draw("same");
      
      Double_t AntiIso_ZYAM_Int = (AntiIsoPhiProj[izt+ipt*nztbins]->Integral(11,13))/3;
      TLine *AntiIso_ZYAM = new TLine(-M_PI/2,AntiIso_ZYAM_Int,3*M_PI/2,AntiIso_ZYAM_Int);
      AntiIso_ZYAM->SetLineColorAlpha(kRed, 0.9);
      AntiIso_ZYAM->SetLineWidth(4);
      AntiIso_ZYAM->Draw("same");
      AntiIsoPhiProj[izt+ipt*nztbins]->Draw("same");
      
      TLegend *AntiIso_legend = new TLegend(0.68,0.75,0.88,0.85);
      AntiIso_legend->AddEntry(IsoPhiProj[izt+ipt*nztbins],"|#Delta#eta| < 0.6","p");
      //AntiIso_legend->AddEntry(IsoPhiProj_LargeEta[izt+ipt*nztbins],"0.6 < |#Delta#eta| < 1.4","l");
      AntiIso_legend->AddEntry(AntiIso_ZYAM, "ZYAM","l");
      AntiIso_legend->Draw();
    }  
    canvas->SaveAs(Form("pics/phi_proj_pt_%1.0f_%1.0f.png",ptbins[ipt],ptbins[ipt+1]));      
  }

  //SUBTRACT ZYAM

  TH1D *IsoPhiPEDSUB[nztbins*nptbins];
  TH1D *AntiIsoPhiPEDSUB[nztbins*nptbins];
  TH1D* Isoped[nztbins*nptbins];
  TH1D* Antiped[nztbins*nptbins];

  for (int ipt = 0; ipt < nptbins; ipt++){
    
    TCanvas *pedcanvas = new TCanvas("pedcanv","pedcanvas",1600,800);
    gStyle->SetOptStat("");
    pedcanvas->Divide(4,3);
    
    for (int izt = 0; izt<nztbins-1; izt++){      
    //for (int izt = 0; izt<4; izt++){      
      
      Double_t Iso_ZYAM_Int = (IsoPhiProj[izt+ipt*nztbins]->Integral(11,13))/3;
      for (int i = 1; i <25; i++) {
	Double_t y = IsoPhiProj[izt+ipt*nztbins]->GetBinContent(i);
	Double_t y_error = IsoPhiProj[izt+ipt*nztbins]->GetBinError(i);
	Double_t new_y = y - Iso_ZYAM_Int;
	//Double_t new_y_error = (y-Iso_ZYAM_Int)*y_error/y;
	Double_t new_y_error = y_error;
// 	IsoPhiProj[izt+ipt*nztbins]->SetBinContent(i,new_y);
// 	IsoPhiProj[izt+ipt*nztbins]->SetBinError(i,new_y_error);
      }
      
      //    IsoPhiProj[izt+ipt*nztbins]->Add(IsoPhiProj_LargeEta[izt],-1);
      IsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("Iso #gamma-h: %1.1f < p_{T}^{Clus.} <%1.1f GeV %1.2f < z_{T} < %1.2f",
      ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));
      
      Double_t AntiIso_ZYAM_Int = (AntiIsoPhiProj[izt+ipt*nztbins]->Integral(11,13))/3;
      for (int i = 1; i <25; i++) {
	Double_t y = AntiIsoPhiProj[izt+ipt*nztbins]->GetBinContent(i);
	Double_t y_error = AntiIsoPhiProj[izt+ipt*nztbins]->GetBinError(i);
	Double_t new_y = y - AntiIso_ZYAM_Int;
	//Double_t new_y_error = (y-AntiIso_ZYAM_Int)*y_error/y;
	Double_t new_y_error = y_error;
// 	AntiIsoPhiProj[izt+ipt*nztbins]->SetBinContent(i,new_y);
// 	AntiIsoPhiProj[izt+ipt*nztbins]->SetBinError(i,new_y_error);
      }

      float max1 = IsoPhiProj[izt+ipt*nztbins]->GetMaximum();
      float max2 = AntiIsoPhiProj[izt+ipt*nztbins]->GetMaximum();
      float Ymax = 1.2*TMath::Max(max1,max2);

      pedcanvas->cd(izt*2+1);
      gPad->SetLeftMargin(0.17);
      gPad->SetRightMargin(0.0);
      IsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetRangeUser(0,Ymax);
      IsoPhiProj[izt+ipt*nztbins]->Draw();

      //AntiIsoPhiProj[izt+ipt*nztbins]->Add(AntiIsoPhiProj_LargeEta[izt],-1);
      AntiIsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("Iso BKGD #gamma-h%1.1f < p_{T}^{Clus.} <%1.1f GeV ,%1.2f < z_{T} < %1.2f",
      ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));

      pedcanvas->cd(izt*2+2);
      gPad->SetLeftMargin(0.1);
      gPad->SetRightMargin(0.07);
      AntiIsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetRangeUser(0,Ymax);
      AntiIsoPhiProj[izt+ipt*nztbins]->Draw();
     
 //save
//       IsoPhiPEDSUB[izt+ipt*nztbins] = IsoPhiProj[izt+ipt*nztbins];
//       IsoPhiPEDSUB[izt+ipt*nztbins]->SetName(
//       Form("IsoPhiPEDSUB_%1.0fpt%1.0f_%1.0fzT%1.0f",ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));

//       AntiIsoPhiPEDSUB[izt+ipt*nztbins] = AntiIsoPhiProj[izt+ipt*nztbins];
//       AntiIsoPhiPEDSUB[izt+ipt*nztbins]->SetName(
//       Form("AntiIsoPhiPEDSUB_%1.0fpt%1.0f_%1.0fzT%1.0f",ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));

//       IsoPhiPEDSUB[izt+ipt*nztbins]->Write(Form("LargeDNN_SUB_PED_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
// 						ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
    }
    pedcanvas->SaveAs(Form("pics/PED_SUB%1.0f_%1.0f.png",ptbins[ipt],ptbins[ipt+1]));
  }

//   for (int ipt = 0; ipt < nptbins; ipt++){
//     for (int izt = 0; izt <nztbins; izt++){
//       AntiIsoPhiPEDSUB[izt+ipt*nztbins]->Write(Form("SmallDNN_Phi_Projection_ptmin%1.0f_ptmax%1.0f_ztmin%1.0f_ztmax%1.0f",
// 						    ptbins[ipt],ptbins[ipt+1],10*ztbins[izt],10*ztbins[izt+1]));
//     }
//   }

  //rember to comment out mixing and scaling of triggers
  TCanvas *NPcanvas = new TCanvas("NPcanv","NPcanvas",1000,800);
  NPcanvas->Divide(1,3);
  for (int ipt = 0; ipt < nptbins; ipt++){
    float n_pairs[nztbins];
    float nzts[nztbins];
    for (int izt = 0; izt <nztbins; izt++){
      n_pairs[izt] = IsoPhiProj[izt+ipt*nztbins]->Integral(14,24); //integrate away side peak after ZYAM
      //nzts[izt] = ztbins[izt] + (ztbins[izt+1]-ztbins[izt])/2;
      nzts[izt] = ztbins[izt];
    }
    NPcanvas->cd(ipt+1);
    TGraph* nps = new TGraph(6,nzts,n_pairs);
    nps->SetTitle(Form("Number of Associated Pairs in %1.0f < pT < %1.0f (Befor Mixing, after ZYAM)", ptbins[ipt], ptbins[ipt+1]));
    nps->GetXaxis()->SetTitle("z_{T} bin");
    nps->GetYaxis()->SetTitle("Number of Pairs");
    nps->SetFillColor(38);
    nps->Draw("AB1");
  }
  NPcanvas->SaveAs("pics/Number_of_Pairs.png");



  //OVERLAY AND PURITY SCALE
  for (int ipt = 0; ipt < nptbins; ipt++){
    double ntriggers = N_Iso_Triggers[ipt]->GetEntries();
    double nbkgd = N_BKGD_Triggers[ipt]->GetEntries();
    double purity = 0.4; //to match Alwina and Miguel's purity studies
    TCanvas *OLcanvas = new TCanvas("OLcanv","OLcanvas",1000,800);
    OLcanvas->Divide(3,2);
    
    for (int izt = 0; izt<nztbins-1; izt++){
      //for (int izt = 0; izt<4; izt++){
      gStyle->SetOptStat("");
      OLcanvas->cd(izt+1);
      gPad->SetLeftMargin(0.135);
      gPad->SetRightMargin(0.0);

      IsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetTitleOffset(1.85);
      double IsoErrors[24];
      double AntiIsoErrors[24];
      for (int i = 1; i <25; i++) {
	IsoErrors[i] = IsoPhiProj[izt+ipt*nztbins]->GetBinError(i);
	AntiIsoErrors[i] = AntiIsoPhiProj[izt+ipt*nztbins]->GetBinError(i);
      }
      TLegend *Overlay_legend = new TLegend(0.45,0.75,0.95,0.88);

      Overlay_legend->AddEntry(IsoPhiProj[izt+ipt*nztbins], "0.55 < DNN < 0.85","l");
      Overlay_legend->AddEntry(AntiIsoPhiProj[izt+ipt*nztbins], "0.0 < DNN < 0.3 PURITY SCALED (1-0.4) ","l");

      AntiIsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("Iso & BKGD #gamma-h: ZYAM Subtracted, %1.1f < p_{T}^{Clus.} <%1.1f GeV, %1.2f < z_{T} < %1.2f",
      ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));

//       IsoPhiProj[izt+ipt*nztbins]->SetTitle(
//       Form("Iso & ~BKGD #gamma-h: ZYAM Subtracted, %1.1f < p_{T}^{Clus.} <%1.1f GeV ,%1.2f < z_{T} < %1.2f",
//       ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));

      IsoPhiProj[izt+ipt*nztbins]->SetTitle(
      Form("Iso & BKGD #gamma-h: No ZYAM, No Mixing, %1.1f < p_{T}^{Clus.} <%1.1f GeV ,%1.2f < z_{T} < %1.2f",
      ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));

      AntiIsoPhiProj[izt+ipt*nztbins]->SetMarkerStyle(1);
      AntiIsoPhiProj[izt+ipt*nztbins]->SetMarkerColor(kRed);
      AntiIsoPhiProj[izt+ipt*nztbins]->SetLineColor(kRed);
      IsoPhiProj[izt+ipt*nztbins]->SetYTitle("#frac{1}{N_{Trig}} #frac{d^{2}N}{d#phid#eta}");
      //IsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetRangeUser(-50,800);

      //Scale according to purity

      //IsoPhiProj[izt+ipt*nztbins]->Scale(ntriggers);
      AntiIsoPhiProj[izt+ipt*nztbins]->Scale((1-purity));

      float Ymax = 0.0;
      for (int izt = 0; izt<nztbins-1; izt++){
	float max1 = IsoPhiProj[izt+ipt*nztbins]->GetMaximum();
	float max2 = AntiIsoPhiProj[izt+ipt*nztbins]->GetMaximum();
	float temp = TMath::Max(max1,max2);
	if (Ymax < temp) Ymax = temp;
      }
      for (int i = 1; i <25; i++) {
 	IsoPhiProj[izt+ipt*nztbins]->SetBinError(i,IsoErrors[i]);
 	AntiIsoPhiProj[izt+ipt*nztbins]->SetBinError(i,AntiIsoErrors[i]);
      }

//       float max1 = IsoPhiProj[izt+ipt*nztbins]->GetMaximum();
//       float max2 = AntiIsoPhiProj[izt+ipt*nztbins]->GetMaximum();
//       float Ymax = 1.2*TMath::Max(max1,max2);

//       IsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetRangeUser(0,Ymax);
//       AntiIsoPhiProj[izt+ipt*nztbins]->GetYaxis()->SetRangeUser(0,Ymax);
      IsoPhiProj[izt+ipt*nztbins]->Draw("same");
      AntiIsoPhiProj[izt+ipt*nztbins]->Draw("same");
      Overlay_legend->Draw("same");
    }
    OLcanvas->SaveAs(Form("pics/Overlay_pt_%1.0f_%1.0f.png",ptbins[ipt],ptbins[ipt+1]));
    //OLcanvas->Clear();
  }

    for (int ipt = 0; ipt < nptbins; ipt++){
    TCanvas *PSubcanvas = new TCanvas("PSubcanv","PSubcanvas",1000,800);
    PSubcanvas->Divide(3,2);
    for (int izt = 0; izt<nztbins; izt++){
      //for (int izt = 0; izt<4; izt++){
	gStyle->SetOptStat("");
	PSubcanvas->cd(izt+1); //FIXME:might break if izt=0   
	gPad->SetLeftMargin(0.135);
	gPad->SetRightMargin(0.0);
	IsoPhiProj[izt+ipt*nztbins]->SetTitle(
        Form("Isolated #gamma-h: BKGD Subtracted, %1.1f < p_{T}^{Clus.} <%1.1f GeV, %1.2f < z_{T} < %1.2f",
	ptbins[ipt],ptbins[ipt+1],ztbins[izt],ztbins[izt+1]));

	double IsoErrors[24];
	for (int i = 1; i <25; i++) {
	  double temp_iso = IsoPhiProj[izt+ipt*nztbins]->GetBinError(i); 
	  double temp_bkgd = AntiIsoPhiProj[izt+ipt*nztbins]->GetBinError(i);
	  IsoErrors[i] = sqrt(pow(temp_iso,2)+pow(temp_bkgd,2)-(2*temp_iso*temp_bkgd));
	}
	for (int i = 1; i <25; i++) {
	  IsoPhiProj[izt+ipt*nztbins]->SetBinError(i,IsoErrors[i]);
	}
	IsoPhiProj[izt+ipt*nztbins]->Add(AntiIsoPhiProj[izt+ipt*nztbins],-1);
	IsoPhiProj[izt+ipt*nztbins]->Draw("same");
      }
      PSubcanvas->SaveAs(Form("pics/PSubtracted_pt_%1.0f_%1.0f.png",ptbins[ipt],ptbins[ipt+1]));
      //PSubcanvas->Clear();
    }    
      MyFile->Close();
}
    
