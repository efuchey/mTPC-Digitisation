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

#include "g4sbs_tree_acqu.C"

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
const Int_t Nevents = 10000;

//==============================================================================
// START MAIN FUNCTION
// void DigiSimpleFarm(const char *infile = "/w/work0/home/rachel/HallA/TDIS/EventsFromSteve/Sim/g4sbsout_EPCEvents_0"){
void DigiSimpleFarm(Int_t EvStart){


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


  //  READ IN SIM FILE
  TChain *C = new TChain("T");
  TString simfile = "/scratch/rachel/tdis/";
  simfile += EvStart;
  simfile += "/g4sbsout_EPCEvents_";
  simfile += EvStart;
  simfile += ".root";

  // TString simfile = infile;
  // // // simfile += EvStart;
  // simfile += ".root";

  // TString simfile = "/w/work0/home/rachel/TDIS/EPC/Aug24/g4sbsout/1p5T/g4sbsout_EPCEvents_";
  // simfile += EvStart;
  // simfile += ".root";

  C->Add(simfile);
  
  // ASSIGN TREE
  // g4sbs_tdis_tree *T = new g4sbs_tdis_tree(C);
  g4sbs_tree_acqu *T = new g4sbs_tree_acqu(C);

  Int_t NEventsChain = C->GetEntries();
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

  TString txtfile = "/scratch/rachel/tdis/";
  txtfile += EvStart;
  txtfile += "/g4sbsout_EPCEvents_";
  txtfile += EvStart;
  txtfile += ".txt";


  ofstream TxtFileOut;
  TxtFileOut.open (txtfile);

  
  //============================================================================
  // EVENT LOOP
  
  // Variables outside loop
  //------------------------------------------------------------ start event loop
  for(Int_t i=0; i<Nevents; i++){   
  // for(Int_t i=0; i<10; i++){     
  
  // for(Int_t i=0; i<1; i++){     
     // cout << endl;
     // cout << "On event " << i << " / " << NEventsChain << endl;
     T->GetEntry(i);
     if (EventCounter % 1000 == 0) cout << EventCounter << "/" << NEventsChain << endl;

     Int_t evid = EvStart+i;
     TxtFileOut << "Event " << right << evid << "\n";
     // TxtFileOut << "Event " << right << i << "\n";
     
     // for acqu
     TVector3 vprimary(T->AcquMCPrimaries_Px, T->AcquMCPrimaries_Py, T->AcquMCPrimaries_Pz);
     Double_t thetaprimary = vprimary.Theta()*TMath::RadToDeg();
     Double_t phiprimary = vprimary.Phi()*TMath::RadToDeg();
     TxtFileOut << "\t" << right << T->AcquMCPrimaries_Pt
		<< "\t" << right << thetaprimary
		<< "\t" << right << phiprimary
		<< "\t" << right << T->AcquMCPrimaries_Vz
		<< "\n";
     
     // mTPC hits
     Int_t mTPC_nHitsPerEvent = T->SBS_mTPC_hit_nhits;
     
     Double_t energydepositedbyhit;
     Double_t hitx;
     Double_t hity;
     Double_t hitz;
     Double_t hitr;
     // cout << "new event" << endl;
     
     for(int hitinc = 0; hitinc<T->SBS_mTPC_hit_nhits; hitinc++){
       if( (*(T->SBS_mTPC_hit_pid))[hitinc]!=2212 || 
	   (*(T->SBS_mTPC_hit_mid))[hitinc]!=0)continue;
       
       energydepositedbyhit = (*(T->SBS_mTPC_hit_edep))[hitinc];
       // hitx =  (*(T->SBS_mTPC_hit_xhit))[hitinc];
       // hity =  (*(T->SBS_mTPC_hit_yhit))[hitinc];
       // hitz =  (*(T->SBS_mTPC_hit_zhit))[hitinc];
       hitx =  (*(T->SBS_mTPC_hit_xhitg))[hitinc];
       hity =  (*(T->SBS_mTPC_hit_yhitg))[hitinc];
       hitz =  (*(T->SBS_mTPC_hit_zhitg))[hitinc];
       hitr = TMath::Sqrt(hitx*hitx + hity*hity);
       // if(hitr<0.05)
       // cout << "hitr " << hitr << endl;
       
       Double_t HitTime = (*(T->SBS_mTPC_hit_tavg))[hitinc];
       
       // ztogem must be in cm since diff coeffs in cm
       Double_t ztogem =  ZtoGEM(hitz*1000.0); // convert argument, from g4, to mm for fncn, return is mm
       ztogem = ztogem/10.0; // convert to cm
       
       Double_t driftime = (ztogem*10000.0)/DriftVelocity;// convert z to gem from cm to um, result will be ns
       
       // get pad loaded script deals in mm
       // g4 out is in m, smearing is cm, so convert cm to mm in end
       vector<int> vhitpad;
       vhitpad = GetPadLoadedQuick(hitx*1000.0,
				   hity*1000.0,
				   hitz*1000.0,
				   hReadoutPads);
       
       // cout << "Hit bin " << meansmearedhitpad[0]
       //      << " hit ring " << meansmearedhitpad[1]
       //      << " hit pad " << meansmearedhitpad[2]
       //      << " hit plane " << meansmearedhitpad[3]
       //      << " hit bin area " << BinArea << endl;

       Double_t TotalTime = HitTime+driftime;
       

             // TxtFileOut << "\t" << right << TotalTime << "\t"
	     // 	  << right << energydepositedbyhit  << "\t"
	     // 	  << right << vhitpad[1] << "\t"
	     // 	  << right << vhitpad[2] << "\t"
	     // 	  << right << vhitpad[3] << "\t"
	     // 	  << right << ztogem/100.0 << "\t"
	     // 	  << endl; // convert z to gem in m

             TxtFileOut << "\t" << right << TotalTime << "\t"
	     		<< right << energydepositedbyhit  << "\t"
	     		<< right << hitx << "\t"
	     		<< right << hity << "\t"
	     		<< right << hitz << "\t"
	     		<< right << vhitpad[1] << "\t"
	     		<< right << vhitpad[2] << "\t"
	     		<< right << vhitpad[3] << "\t"
	     		<< right << ztogem/100.0 << "\t"
	     		<< endl; // convert z to gem in m
	     
       // clear vectors
       vhitpad.clear();
       
     }// hit loop
     
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
