#include <TROOT.h>
#include <TGraph.h>
#include <iostream>
#include <fstream>
#include <TChainElement.h>
#include <TMath.h>
#include <TH2Poly.h>
#include <TF1.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TRandom3.h>
#include <TRandom.h>
#include <string>
#include <TString.h>
#include <TCanvas.h>
#include <vector>
#include <algorithm>
#include <TLorentzVector.h>
// #include <TIter.h>

//#include "g4sbs_tree_acqu.C"
#include "g4sbs_tree_acqu_light.C"

// #include "GetPadLoaded.cpp"
#include "GetPadLoadedQuick.cpp"
#include "SampaShapingAndSample_ReturnVals.cpp"

#include <chrono>

using namespace std;

//==============================================================================
// FUNCTIONS
vector<Double_t> CalcCharge(Double_t Edep, Double_t PathLength);
Double_t ZtoGEM(Double_t zpos);


//==============================================================================
// GLOBAL VARIABLES
const Int_t GEMGain = 1000;//700;//1000; //is typical gem gain 1k?
// const Int_t Nevents = 50000;
//const Int_t Nevents = 10000;


//==============================================================================
// START MAIN FUNCTION
// void DigiSimpleFarm(const char *infile = "/w/work0/home/rachel/HallA/TDIS/EventsFromSteve/Sim/g4sbsout_EPCEvents_0"){
//void DigiSimpleFarm(Int_t EvStart){
void DigiSimpleFarm(const char* filelist, const TString* outpath = "", const Int_t Nevents = 10000, const Int_t EvStart = 0){

  //============================================================================
  // CHECK START EVENT
  // std::string str = infile;
  // size_t t=0;
  // for(; t < str.length(); t++){
  //   if (isdigit(str[t])) break;
  // }
  // int id = atoi(str.c_str());
  // cout << "testing id " << id << endl;
  // Int_t EvStart=id;
  Double_t POIEnergyDeposit[10][21][122];//Energy deposit due to "Primaries" ()
  Double_t TotalEnergyDeposit[10][21][122];
  //Int_t PID_poihit[10][21][122];//PID of largest energy deposit
  Double_t TotalTime_poihit[10][21][122];
  Double_t ZtoGEM_poihit[10][21][122];
  Double_t x_poihit[10][21][122];
  Double_t y_poihit[10][21][122];
  Double_t z_poihit[10][21][122];
  Int_t PID_hiedephit[10][21][122];//PID of largest energy deposit
  Double_t TotalTime_hiedephit[10][21][122];
  Double_t ZtoGEM_hiedephit[10][21][122];
  Double_t x_hiedephit[10][21][122];
  Double_t y_hiedephit[10][21][122];
  Double_t z_hiedephit[10][21][122];
  Double_t hiedephit[10][21][122];
  //Double_t ADCSamples[10][21][122][];
  Int_t n_memset_int = nPads_mTPC*size_of(Int_t);
  Int_t n_memset_double = nPads_mTPC*size_of(Double_t);

  //Read the file list:
  ifstream configfile(filelist);
  TChain *C_poi = new TChain("T");
  TChain *C_pqe = new TChain("T");
  TChain *C_pip = new TChain("T");
  TChain *C_pim = new TChain("T");
  TFile* f_beam;
  Int_t nprotons_avg;
  Int_t npiplus_avg;
  Int_t npiminus_avg;
  TH1D* h1_mTPChits_E_electrons;
  TH1D* h1_mTPChits_Z_electrons;
  TH1D* h1_mTPChits_R_electrons[500];

  
  TString currentline;

  while( currentline.ReadLine( configfile ) && !currentline.BeginsWith("end_tdis_files") ){
    if( !currentline.BeginsWith("#") ){
      C_poi->Add(currentline);
    }
  }

  while( currentline.ReadLine( configfile ) && !currentline.BeginsWith("end_elastic_files") ){
    if( !currentline.BeginsWith("#") ){
      C_pqe->Add(currentline);
    }
  }

  while( currentline.ReadLine( configfile ) && !currentline.BeginsWith("end_piplus_files") ){
    if( !currentline.BeginsWith("#") ){
      C_pip->Add(currentline);
    }
  }

  while( currentline.ReadLine( configfile ) && !currentline.BeginsWith("end_piminus_files") ){
    if( !currentline.BeginsWith("#") ){
      C_pim->Add(currentline);
    }
  }

  while( currentline.ReadLine( configfile ) && !currentline.BeginsWith("end_beambkgd_files") ){
    if( !currentline.BeginsWith("#") ){
      f_beam = TFile::Open(currentline.Data());
      TH2D* h2D_mTPChits_EvR_electrons = (TH1D*)f_beam->Get("h2D_mTPChits_EvR_electrons");
      TH2D* h2D_mTPChits_RvZ_electrons = (TH1D*)f_beam->Get("h2D_mTPChits_RvZ_electrons");
      h1_mTPChits_E_electrons = h2D_mTPChits_EvR_electrons->ProjectionY("h1_mTPChits_E_electrons", 2, 199);
      h1_mTPChits_Z_electrons = h2D_mTPChits_RvZ_electrons->ProjectionX("h1_mTPChits_Z_electrons", 2, 199);
      //Choose R projection as function of Z;
      for(int m = 0; m<500; m++){
	h1_mTPChits_R_electrons[m] = h2D_mTPChits_RvZ_electrons->ProjectionX(Form("h1_mTPChits_R_electrons_%d", m), m, m);
	h1_mTPChits_R_electrons[m]->SetBinContent(1, h2D_mTPChits_EvR_electrons->Integral(1, 1, 140, -1)/h2D_mTPChits_EvR_electrons->Integral(1, 1, 0, -1));
	h1_mTPChits_R_electrons[m]->SetBinContent(200, h2D_mTPChits_EvR_electrons->Integral(200, 200, 140, -1)/h2D_mTPChits_EvR_electrons->Integral(200, 200, 0, -1));
      }
    }
  }
  
  while( currentline.ReadLine( configfile ) && !currentline.BeginsWith("end_config_files") ){
    if( !currentline.BeginsWith("#") ){
      ObjArray *tokens = currentline.Tokenize(" ");

      int ntokens = tokens->GetEntries();
      
      if( ntokens >= 2 ){
	TString skey = ( (TObjString*) (*tokens)[0] )->GetString();
	
	if( skey == "nprotons_pertdisevent" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	  nprotons_avg = stemp.Atoi();
	}
	
	if( skey == "npiplus_pertdisevent" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	  npiplus_avg = stemp.Atoi();
	}
	
	if( skey == "npiminus_pertdisevent" ){
	  TString stemp = ( (TObjString*) (*tokens)[1] )->GetString();
	  npiplus_avg = stemp.Atoi();
	}
	
      }
    }
  }
  
  
  //============================================================================
  // FLAGS AND VARIABLES
  Int_t EventCounter = 0;
  Int_t nevent = 0;
  TRandom3 RandomVar(0);

  //
  // values for diffusion from 07/04/10 update tdis daq
  //
  // he:ch4 90:10 at 1.2kV/cm:
  // Double_t DL = 0.022;//0.015;//0.03;//0.015;//sqrt(cm) up to 0.03sqrt(cm)
  // Double_t DT = 0.025;
  // vd = 19um/ns = max 2.6us
  //
  // he:ch4 70:30 at 1.2kV/cm:
  // density for this mix is 0.000316706 g/cm3
  Double_t DL = 0.015;//0.015;//0.03;//0.015;//sqrt(cm) up to 0.03sqrt(cm)
  Double_t DT = 0.015;
  Double_t MaxDriftTime = 1700.0;// convert 1.7e-6 to ns
  Double_t DriftVelocity = 32.1;//30um/ns
  // vd = 30um/ns = max 1.7us

  // he:ch4 70:30 at 3kV/cm for gem regions
  // Double_t DL_GEM = 0.017;////sqrt(cm)
  // Double_t DT_GEM = 0.025;//sqrt(cm)
  // Double_t DriftVelocity_GEM = 32.0; // um/ns

  // he:ch4 70:30 at 3.4kV/cm for gem regions
  Double_t DL_GEM = 0.014468; //sqrt(cm)
  Double_t DT_GEM = 0.0263103; //sqrt(cm)
  Double_t DriftVelocity_GEM = 33.04; // um/ns


  // gem dimensions
  const Double_t GEMGap = 0.1; // take 1mm, units cm to match diffusion units
  // smear the transverse drift of each electron to get charge cloud area
  Double_t sigmatransverse_GEM = DT_GEM * TMath::Sqrt(GEMGap);
  // smear the longitudinal drift of each electron to get time window 
  Double_t sigmalongitudinal_GEM = DL_GEM * TMath::Sqrt(GEMGap); 

  //============================================================================
  
  // get histo of readout pads
  TFile *fPads =  TFile::Open("hPads2.root"); //hPads2 is steve's suggested pad phis
  TCanvas *c1 = (TCanvas*)fPads->Get("c1");
  TH2Poly *hReadoutPads = (TH2Poly*)c1->GetPrimitive("hPolyBins");
  // to draw the pads
  // TCanvas *cPads = new TCanvas();
  // cPads->cd();
  // hReadoutPads->Draw();
  fPads->Close();
  // get bin list
  TList *bin_list=hReadoutPads->GetBins();
  if( !bin_list ){
    cout << "Problem in getting bin list from hReadoutPads, exitting." << endl;
    exit (EXIT_FAILURE);
  }
  TIter next(bin_list);
  TObject *obj;
  TH2PolyBin *bin;
  double area;
  vector<Double_t> vbinareas;
  while((obj=next())){
    bin = (TH2PolyBin*) obj;
    vbinareas.push_back(bin->GetArea());
  }
  
  // ASSIGN TREES
  // g4sbs_tdis_tree *T = new g4sbs_tdis_tree(C);
  g4sbs_tree_acqu_light *T_poi = new g4sbs_tree_acqu_light(C_poi);
  g4sbs_tree_acqu_light *T_pqe = new g4sbs_tree_acqu_light(C_pqe);
  g4sbs_tree_acqu_light *T_pip = new g4sbs_tree_acqu_light(C_pip);
  g4sbs_tree_acqu_light *T_pim = new g4sbs_tree_acqu_light(C_pim);

  Int_t NEventsChain = C_poi->GetEntries();
  cout << "N events in chain is " << NEventsChain << endl << endl;
  
  // Text file for hit outputs
  // TString txtfile = "/w/work0/home/rachel/TDIS/EPC/Aug24/DigiOut/1p5T";
  // txtfile += EvStart;
  // txtfile += "/g4sbsout_EPCEvents_";
  // txtfile += EvStart;
  // txtfile += ".txt";
  // TString txtfile = infile;
  // TString txtfile = "g4sbsout_EPCEvents_digitised_";
  // txtfile += EvStart;
  // txtfile += ".txt";

  TString txtfile = outpath;
  //txtfile += EvStart;
  txtfile += "/g4sbsout_EPCEvents_";
  txtfile += EvStart;
  txtfile += ".txt";


  ofstream TxtFileOut;
  TxtFileOut.open (txtfile);

  int planeid, ringid, padid;

  Bool_t POI_flag = false;
  
  Int_t PID;
  Double_t hitx, hity, hitz;
  Double_t HitTime, drifttime, TotalTime, ztogem;
  Int_t nprotons_gen, npiplus_gen, npiminus_gen, npiplus_gen, nbeambkgdhits_gen;
  Double_t acctime0, hitr, hitphi;
  //============================================================================
  // EVENT LOOP
  
  // Variables outside loop
  //------------------------------------------------------------ start event loop
  // for TDIS events, Set "ProtonOfInterest" flag to true!
  for(Int_t ev_poi=0; ev_poi<Nevents; ev_poi++){
    //reinitialize arrays:
    memset(POIEnergyDeposit, n_memset_double, 0);
    memset(TotalEnergyDeposit, n_memset_double, 0);
    //memset(PID_poihit, n_memset_int, 0);
    memset(TotalTime_poihit, n_memset_double, 0);
    memset(ZtoGEM_poihit, n_memset_double, 0);
    memset(x_poihit, n_memset_double, 0);
    memset(y_poihit, n_memset_double, 0);
    memset(z_poihit, n_memset_double, 0);
    memset(PID_hiedephit, n_memset_int, 0);
    memset(TotalTime_hiedephit, n_memset_double, 0);
    memset(ZtoGEM_hiedephit, n_memset_double, 0);
    memset(x_hiedephit, n_memset_double, 0);
    memset(y_hiedephit, n_memset_double, 0);
    memset(z_hiedephit, n_memset_double, 0);
    memset(hiedephit, n_memset_double, 0);
    
    // for(Int_t i=0; i<10; i++){     
    // for(Int_t i=0; i<1; i++){     
    // cout << endl;
    // cout << "On event " << i << " / " << NEventsChain << endl;
    T_poi->GetEntry(EvStart+ev_poi);
    if (EventCounter % 1000 == 0) cout << EventCounter << "/" << NEventsChain << endl;
    
    //Int_t evid = EvStart+;
    TxtFileOut << "Event " << right << evid << "\n";
    // TxtFileOut << "Event " << right << i << "\n";
     
    // for acqu
    TVector3 vprimary(T_poi->AcquMCPrimaries_Px, T_poi->AcquMCPrimaries_Py, T_poi->AcquMCPrimaries_Pz);
    Double_t thetaprimary = vprimary.Theta()*TMath::RadToDeg();
    Double_t phiprimary = vprimary.Phi()*TMath::RadToDeg();
    TxtFileOut << "\t" << right << T_poi->AcquMCPrimaries_Pt
	       << "\t" << right << thetaprimary
	       << "\t" << right << phiprimary
	       << "\t" << right << T_poi->AcquMCPrimaries_Vz
	       << "\n";
     
    // mTPC hits
    // cout << "new event" << endl;
    
    POI_flag = true;
    for(int hitinc = 0; hitinc<T_poi->SBS_mTPC_hit_nhits; hitinc++){
      
      hitx =  (*(T_poi->SBS_mTPC_hit_xhitg))[hitinc];
      hity =  (*(T_poi->SBS_mTPC_hit_yhitg))[hitinc];
      hitz =  (*(T_poi->SBS_mTPC_hit_zhitg))[hitinc];
      
      GetPadLoadedSuperQuick(hitx*1000.0,
			     hity*1000.0,
			     hitz*1000.0,
			     planeid,
			     ringid,
			     padid);
      
      HitTime = (*(T_poi->SBS_mTPC_hit_tavg))[hitinc];
      // ztogem must be in cm since diff coeffs in cm
      ztogem =  ZtoGEM(hitz*1000.0); // convert argument, from g4, to mm for fncn, return is mm
      ztogem = ztogem/10.0; // convert to cm
      driftime = (ztogem*10000.0)/DriftVelocity;// convert z to gem from cm to um, result will be ns
      TotalTime = HitTime+driftime;

      
      TotalEnergyDeposit[planeid][ringid][padid]+= (*(T_poi->SBS_mTPC_hit_edep))[hitinc];
      if( (*(T_poi->SBS_mTPC_hit_pid))[hitinc]==2212 && POI_flag){
	POI_fraction[planeid][ringid][padid]+= (*(T_poi->SBS_mTPC_hit_edep))[hitinc];
	//PID_poihit[planeid][ringid][padid] = 2212;
	TotalTime_poihit[planeid][ringid][padid] = TotalTime;
	ZtoGEM_poihit[planeid][ringid][padid] = ztogem;
	x_poihit[planeid][ringid][padid] = hitx;
	y_poihit[planeid][ringid][padid] = hity;
	z_poihit[planeid][ringid][padid] = hitz;
      }

      if((*(T_poi->SBS_mTPC_hit_edep))[hitinc]>hiedephit[planeid][ringid][padid]){
	hiedephit[planeid][ringid][padid] = (*(T_poi->SBS_mTPC_hit_edep))[hitinc];
	PID_hiedephit[planeid][ringid][padid] = (*(T_poi->SBS_mTPC_hit_pid))[hitinc];
	TotalTime_hiedephit[planeid][ringid][padid] = TotalTime;
	ZtoGEM_hiedephit[planeid][ringid][padid] = ztogem;
	x_hiedephit[planeid][ringid][padid] = hitx;
	y_hiedephit[planeid][ringid][padid] = hity;
	z_hiedephit[planeid][ringid][padid] = hitz;
      }
    }// tdis  hit loop

    POI_flag = false;
    nprotons_gen = RandomVar.Poisson(nprotons_avg);
    for(Int_t ev_pqe=0; ev_pqe<nprotons_gen, ev_pqe++){
      acctime0 = RandomVar.Uniform(-1000., 1000.);
      for(int hitinc = 0; hitinc<T_pqe->SBS_mTPC_hit_nhits; hitinc++){
      
	hitx =  (*(T_pqe->SBS_mTPC_hit_xhitg))[hitinc];
	hity =  (*(T_pqe->SBS_mTPC_hit_yhitg))[hitinc];
	hitz =  (*(T_pqe->SBS_mTPC_hit_zhitg))[hitinc];
      
	GetPadLoadedSuperQuick(hitx*1000.0,
			       hity*1000.0,
			       hitz*1000.0,
			       planeid,
			       ringid,
			       padid);
	
	HitTime = (*(T_pqe->SBS_mTPC_hit_tavg))[hitinc];
	// ztogem must be in cm since diff coeffs in cm
	ztogem =  ZtoGEM(hitz*1000.0); // convert argument, from g4, to mm for fncn, return is mm
	ztogem = ztogem/10.0; // convert to cm
	driftime = (ztogem*10000.0)/DriftVelocity;// convert z to gem from cm to um, result will be ns
	TotalTime = acctime0+HitTime+driftime;

      
	TotalEnergyDeposit[planeid][ringid][padid]+= (*(T_pqe->SBS_mTPC_hit_edep))[hitinc];
	if((*(T_pqe->SBS_mTPC_hit_edep))[hitinc]>hiedephit[planeid][ringid][padid]){
	  hiedephit[planeid][ringid][padid] = (*(T_pqe->SBS_mTPC_hit_edep))[hitinc];
	  PID_hiedephit[planeid][ringid][padid] = (*(T_pqe->SBS_mTPC_hit_pid))[hitinc];
	  TotalTime_hiedephit[planeid][ringid][padid] = TotalTime;
	  ZtoGEM_hiedephit[planeid][ringid][padid] = ztogem;
	  x_hiedephit[planeid][ringid][padid] = hitx;
	  y_hiedephit[planeid][ringid][padid] = hity;
	  z_hiedephit[planeid][ringid][padid] = hitz;
	}
      }// elastic protons hit loop
    }
    
    npiplus_gen = RandomVar.Poisson(npiplus_avg);
    for(Int_t ev_pip=0; ev_pip<npiplus_gen, ev_pip++){
      acctime0 = RandomVar.Uniform(-1000., 1000.);
      for(int hitinc = 0; hitinc<T_pip->SBS_mTPC_hit_nhits; hitinc++){
      
	hitx =  (*(T_pip->SBS_mTPC_hit_xhitg))[hitinc];
	hity =  (*(T_pip->SBS_mTPC_hit_yhitg))[hitinc];
	hitz =  (*(T_pip->SBS_mTPC_hit_zhitg))[hitinc];
      
	GetPadLoadedSuperQuick(hitx*1000.0,
			       hity*1000.0,
			       hitz*1000.0,
			       planeid,
			       ringid,
			       padid);
      
	HitTime = (*(T_pip->SBS_mTPC_hit_tavg))[hitinc];
	// ztogem must be in cm since diff coeffs in cm
	ztogem =  ZtoGEM(hitz*1000.0); // convert argument, from g4, to mm for fncn, return is mm
	ztogem = ztogem/10.0; // convert to cm
	driftime = (ztogem*10000.0)/DriftVelocity;// convert z to gem from cm to um, result will be ns
	TotalTime = acctime0+HitTime+driftime

      
	TotalEnergyDeposit[planeid][ringid][padid]+= (*(T_pip->SBS_mTPC_hit_edep))[hitinc];
	if((*(T_pip->SBS_mTPC_hit_edep))[hitinc]>hiedephit[planeid][ringid][padid]){
	  hiedephit[planeid][ringid][padid] = (*(T_pip->SBS_mTPC_hit_edep))[hitinc];
	  PID_hiedephit[planeid][ringid][padid] = (*(T_pip->SBS_mTPC_hit_pid))[hitinc];
	  TotalTime_hiedephit[planeid][ringid][padid] = TotalTime;
	  ZtoGEM_hiedephit[planeid][ringid][padid] = ztogem;
	  x_hiedephit[planeid][ringid][padid] = hitx;
	  y_hiedephit[planeid][ringid][padid] = hity;
	  z_hiedephit[planeid][ringid][padid] = hitz;
	}
      }// pi+ hit loop
    }
    
    npiminus_gen = RandomVar.Poisson(npiminus_avg);
    for(Int_t ev_pim=0; ev_pim<npiminus_gen, ev_pim++){
      acctime0 = RandomVar.Uniform(-1000., 1000.);
      for(int hitinc = 0; hitinc<T_pim->SBS_mTPC_hit_nhits; hitinc++){
      
	hitx =  (*(T_pim->SBS_mTPC_hit_xhitg))[hitinc];
	hity =  (*(T_pim->SBS_mTPC_hit_yhitg))[hitinc];
	hitz =  (*(T_pim->SBS_mTPC_hit_zhitg))[hitinc];
      
	GetPadLoadedSuperQuick(hitx*1000.0,
			       hity*1000.0,
			       hitz*1000.0,
			       planeid,
			       ringid,
			       padid);
      
	HitTime = (*(T_pim->SBS_mTPC_hit_tavg))[hitinc];
	// ztogem must be in cm since diff coeffs in cm
	ztogem =  ZtoGEM(hitz*1000.0); // convert argument, from g4, to mm for fncn, return is mm
	ztogem = ztogem/10.0; // convert to cm
	driftime = (ztogem*10000.0)/DriftVelocity;// convert z to gem from cm to um, result will be ns
	TotalTime = acctime0+HitTime+driftime;

      
	TotalEnergyDeposit[planeid][ringid][padid]+= (*(T_pim->SBS_mTPC_hit_edep))[hitinc];
	if((*(T_pim->SBS_mTPC_hit_edep))[hitinc]>hiedephit[planeid][ringid][padid]){
	  hiedephit[planeid][ringid][padid] = (*(T_pim->SBS_mTPC_hit_edep))[hitinc];
	  PID_hiedephit[planeid][ringid][padid] = (*(T_pim->SBS_mTPC_hit_pid))[hitinc];
	  TotalTime_hiedephit[planeid][ringid][padid] = TotalTime;
	  ZtoGEM_hiedephit[planeid][ringid][padid] = ztogem;
	  x_hiedephit[planeid][ringid][padid] = hitx;
	  y_hiedephit[planeid][ringid][padid] = hity;
	  z_hiedephit[planeid][ringid][padid] = hitz;
	}
      }// pi- hit loop
    }
    
    
    nbeambkgdhits_gen = RandomVar.Poisson(h1_mTPChits_Z_electrons->Integral());
    for(Int_t ev_beam=0; ev_beam<nbeambkgd_gen, ev_beam++){
      acctime0 = RandomVar.Uniform(-1000.0, 1000.0);
      edep = h1_mTPChits_E_electrons->GetRandom();
      hitz = h1_mTPChits_Z_electrons->GetRandom();
      binz = h2D_mTPChits_RvZ_electrons->GetXaxis()->FindBin(hitz);
      //h1_mTPChits_R_electrons = h2D_mTPChits_RvZ_electrons->ProjectionY("h1_mTPChits_R_electrons",2, 200);
      hitr = h1_mTPChits_R_electrons_[binz]->GetRandom();
      hitphi = RandomVar.Uniform(0.0, 6.283185307);//0->twopi
      hitx = hitr*cos(hitphi);
      hity = hitr*sin(hitphi);
      
      GetPadLoadedSuperQuick(hitx*1000.0,
			     hity*1000.0,
			     hitz*1000.0,
			     planeid,
			     ringid,
			     padid);
      
      ztogem =  ZtoGEM(hitz*1000.0); // convert argument, from g4, to mm for fncn, return is mm
      ztogem = ztogem/10.0; // convert to cm
      driftime = (ztogem*10000.0)/DriftVelocity;
      TotalTime = acctime0+driftime;
      TotalEnergyDeposit[planeid][ringid][padid]+= edep;
      if(edep>hiedephit[planeid][ringid][padid]){
	hiedephit[planeid][ringid][padid] = edep;
	PID_hiedephit[planeid][ringid][padid] = 11;
	TotalTime_hiedephit[planeid][ringid][padid] = TotalTime;
	ZtoGEM_hiedephit[planeid][ringid][padid] = ztogem;
	x_hiedephit[planeid][ringid][padid] = hitx;
	y_hiedephit[planeid][ringid][padid] = hity;
	z_hiedephit[planeid][ringid][padid] = hitz;
      }
    }

    for(int i = 0; i<nPlanes; i++){
      for(int j = 0; j<nRings; j++){
	for(int k = 0; k<nPads; k++){
	  if(POIEnergyDeposit[i][j][k]>0){
	    TxtFileOut << "\t" << right << TotalTime_poihit[i][j][k] << "\t"
		       << right << 2212 << "\t"
		       << right << POIEnergyDeposit[i][j][k]/TotalEnergyDeposit[i][j][k] << "\t"
	      // << right << energydepositedbyhit  << "\t"
		       << right << totalenergydeposit[i][j][k]  << "\t"
		       << right << x_poihit[i][j][k] << "\t"
		       << right << y_poihit[i][j][k] << "\t"
		       << right << z_poihit[i][j][k] << "\t"
		       << right << i << "\t"
		       << right << j << "\t"
		       << right << k << "\t"
		       << right << ZtoGEM_poihit[i][j][k]/100.0 << "\t"
		       << endl; // convert z to gem in m
	  }else if(TotalEnergyDeposit[i][j][k]>0){
	    TxtFileOut << "\t" << right << TotalTime_hiedephit[i][j][k] << "\t"
		       << right << PID_hiedephit[i][j][k] << "\t"
		       << right << POIEnergyDeposit[i][j][k]/TotalEnergyDeposit[i][j][k] << "\t"
	      // << right << energydepositedbyhit  << "\t"
		       << right << totalenergydeposit[i][j][k]  << "\t"
		       << right << x_hiedephit[i][j][k] << "\t"
		       << right << y_hiedephit[i][j][k] << "\t"
		       << right << z_hiedephit[i][j][k] << "\t"
		       << right << i << "\t"
		       << right << j << "\t"
		       << right << k << "\t"
		       << right << ZtoGEM_hiedephit[i][j][k]/100.0 << "\t"
		       << endl; // convert z to gem in m
	  }
	}
      }
    }
    
    EventCounter++;
  } //------------------------------------------------------------ end event loop
 
  cout << endl;
 
  //============================================================================
 
 
  // CLOSE FILES
  TxtFileOut.close();
 
  
} //------------------------------------------------------------ end main function
  
  vector<Double_t> CalcCharge(Double_t Edep, Double_t PathLength){
    // helium values from pdg table 33.5
    Double_t He_density = 0.179; // mg/cm^3
    Double_t He_Ex = 19.8; // eV
    Double_t He_Ei = 24.6; // eV
    Double_t He_Wi = 41.3; // eV
    Double_t He_dEdx_MIP = 0.32; // 0.32 keV/cm
    Double_t He_Np = 3.5; // cm^-1
    Double_t He_Nt = 8.0; // cm^-1
    // methane values from pdg table 33.5
    Double_t CH4_density = 0.667; // mg/cm^3
    Double_t CH4_Ex = 8.8; // eV
    Double_t CH4_Ei = 12.6; // eV
    Double_t CH4_Wi = 30.0; // eV
    Double_t CH4_dEdx_MIP = 1.61; // 0.32 keV/cm
    Double_t CH4_Np = 28.0; // cm^-1
    Double_t CH4_Nt = 54.0; // cm^-1
    
    Double_t He_Frac = 0.7;//0.9;
    Double_t CH4_Frac = 0.3;//0.1;
    Double_t Mix_Nt = (He_Frac*He_Nt) + (CH4_Frac*CH4_Nt); // cm^-1
    Double_t Mix_Wi = (He_Frac*He_Wi) + (CH4_Frac*CH4_Wi); // cm^-1
    // cout << "Wi of mix " << Mix_Wi << endl;
    
    Double_t PathLengthIncm = PathLength*100.0; // assume g4 in [m]
    // cout << "PathLengthincm " << PathLengthIncm << endl;
    
    Double_t EDepInkeV = Edep*1000000.0;
    // cout << "EDep in keV " << EDepInkeV << endl;
    
    Double_t EDep_keVPercm = EDepInkeV/PathLengthIncm;
    Double_t EDep_eVPercm = EDep_keVPercm*1000.0;
    Double_t NDeltaElecPercm = EDep_eVPercm/Mix_Wi; // this is per cm
    // cout << "NDeltaElec per cm " << NDeltaElecPercm << endl;
    
    Double_t NDeltaElec = NDeltaElecPercm * PathLengthIncm;
    // cout << "NDeltaElec per path length " << NDeltaElec << endl;
    
    Double_t GainDoubleGem = GEMGain;
    Double_t TotalElec = GainDoubleGem * NDeltaElec;
    
    Double_t ChargeElectronInfC = 1.60217662e-19/1.0e-15;
    // Double_t Charge = NDeltaElec * ChargeElectronInfC;
    Double_t Charge = TotalElec * ChargeElectronInfC;
    
    vector<Double_t> vreturn;
    vreturn.push_back(Charge);
    vreturn.push_back(NDeltaElec);
    vreturn.push_back(TotalElec);
    
    return vreturn;
  }
  
  
  Double_t ZtoGEM(Double_t zpos){ // it is input in mm, m from g4 multiplied by 1000
    
    Double_t DistanceToGEM = -999.0;
    if(zpos>=-250.0 && zpos<=-200.0)
      DistanceToGEM = fabs(zpos - (-250.0));
    if(zpos>-200.0 && zpos<=-150.0)
      DistanceToGEM = fabs(zpos - (-150.0));
    if(zpos>-150.0 && zpos<=-100.0)
      DistanceToGEM = fabs(zpos - (-150.0));
    if(zpos>-100.0 && zpos<=-50.0)
      DistanceToGEM = fabs(zpos - (-50.0));
    if(zpos>-50.0 && zpos<=0.0)
      DistanceToGEM = fabs(zpos - (-50.0));
    if(zpos>0.0 && zpos<=50.0)
      DistanceToGEM = fabs(zpos - (50.0));
    if(zpos>50.0 && zpos<=100.0)
      DistanceToGEM = fabs(zpos - (50.0));
    if(zpos>100.0 && zpos<=150.0)
      DistanceToGEM = fabs(zpos - (150.0));
    if(zpos>150.0 && zpos<=200.0)
      DistanceToGEM = fabs(zpos - (150.0));
    if(zpos>200.0 && zpos<=250.0)
      DistanceToGEM = fabs(zpos - (250.0));
    
    return DistanceToGEM; // it is in mm
  }
  
  // incCell 0 Z of readout is -0.24987;
  // incCell 1 Z of readout is -0.15013;
  // incCell 2 Z of readout is -0.14987;
  // incCell 3 Z of readout is -0.0501305;
  // incCell 4 Z of readout is -0.0498695;
  // incCell 5 Z of readout is 0.0498695;
  // incCell 6 Z of readout is 0.0501305;
  // incCell 7 Z of readout is 0.14987;
  // incCell 8 Z of readout is 0.15013;
  // incCell 9 Z of readout is 0.24987;
