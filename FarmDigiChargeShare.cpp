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
#include <unordered_map>
#include <unordered_set>

#include "g4sbs_tree_acqu.C"
#include "GetPadLoadedQuick.cpp"
#include "SampaShapingAndSample_ReturnVals.cpp"

#include <chrono>

using namespace std;



//==============================================================================
// GLOBAL VARIABLES
const Int_t GEMGain = 1000;
const Int_t Nevents = 10000; // note this has to be changed if running anything different from 10k events per file - or automise it using n events in chain



//==============================================================================
// FUNCTIONS
vector<Double_t> CalcCharge(Double_t Edep, Double_t PathLength, Double_t GemGain);
Double_t ZtoGEM(Double_t zpos);



//==============================================================================
// START MAIN FUNCTION
// local test
// void FarmDigiChargeShare(const char *infile = "/w/work0/home/rachel/TDIS/EPC/Feb26-2p8T/g4sbsout/g4sbsout_EPCEvents_"){
//farming
void FarmDigiChargeShare(Int_t EvStart){


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
  TRandom3 RandomVarGain(0);
  TRandom3 RandomVarCircle(0);


  // simulation values for gases from magboltz
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
  Double_t DriftVelocity = 32.1;//32.1um/ns
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
  TFile *fPads =  TFile::Open("hPads2.root");
  //hPads2 is steve's suggested pad phi's
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

  // for farming
  TString simfile = "/scratch/rachel/tdis/"; //change path for location on farm node
  simfile += EvStart;
  simfile += "/g4sbsout_EPCEvents_";
  simfile += EvStart;
  simfile += ".root";

  // for local testing
  // C->Add("/w/work0/home/rachel/TDIS/EPC/Feb26-2p8T/g4sbsout/g4sbsout_EPCEvents_1390000.root");

  // for farming
  C->Add(simfile);
  
  // ASSIGN TREE
  g4sbs_tree_acqu *T = new g4sbs_tree_acqu(C);

  Int_t NEventsChain = C->GetEntries();
  cout << "N events in chain is " << NEventsChain << endl << endl;
  
  // for local testing
  // TString txtfile = "/w/work0/home/rachel/TDIS/EPC/Aug24/DigiOut/1p5T";
  // txtfile += EvStart;
  // txtfile += "/g4sbsout_EPCEvents_";
  // txtfile += EvStart;
  // txtfile += ".txt";

  // for farming
  TString txtfile = "/scratch/rachel/tdis/"; // change path for location on farm node
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
  
  // for(Int_t i=0; i<NEventsChain; i++){     
     // cout << endl;
     // cout << "On event " << i << " / " << NEventsChain << endl;
     T->GetEntry(i);
     if (EventCounter % 1000 == 0) cout << EventCounter << "/" << NEventsChain << endl;

     Int_t evid = EvStart+i;
     TxtFileOut << "Event " << right << evid << "\n";
     
     // for acqu
     // get info about the track and write to text file
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

       // cout << "new hit" << endl;

       energydepositedbyhit = (*(T->SBS_mTPC_hit_edep))[hitinc]; // in GeV in g4sbs
       hitx =  (*(T->SBS_mTPC_hit_xhitg))[hitinc];
       hity =  (*(T->SBS_mTPC_hit_yhitg))[hitinc];
       hitz =  (*(T->SBS_mTPC_hit_zhitg))[hitinc];
       hitr = TMath::Sqrt(hitx*hitx + hity*hity);
       // if(hitr<0.05)
       // cout << "hitr " << hitr << endl;
       // cout << "unsmeared hitx mm " << hitx*1000.0 << " hity " << hity*1000.0 << " hitz " << hitz*1000.0 << endl;
       
       Double_t HitTime = (*(T->SBS_mTPC_hit_tavg))[hitinc];
       
       // ztogem must be in cm since diffusion coeffs in cm
       // convert argument, from g4, to mm for fncn, return from fncn is mm
       Double_t ztogem =  ZtoGEM(hitz*1000.0);
       ztogem = ztogem/10.0; // convert to cm
       
       Double_t driftime = (ztogem*10000.0)/DriftVelocity;// convert z to gem from cm to um, result will be ns
       
       // get pad loaded script deals in mm
       // g4 out is in m, smearing is cm, so convert cm to mm in end
       vector<int> vhitpad;
       vhitpad = GetPadLoadedQuick(hitx*1000.0,
				   hity*1000.0,
				   hitz*1000.0,
				   hReadoutPads);

       Double_t TotalTime = HitTime+driftime;
       
       // clear vectors
       vhitpad.clear();
       // end of basic digi part
       

       ///////////////////////////////////////////////////////////////////////////////
       // Start of charge part
       ///////////////////////////////////////////////////////////////////////////////

       // Delta electrons

       // Get charge from edep and number of electrons
       Double_t steplength = (*(T->SBS_mTPC_hit_hitL))[hitinc];       
       // Double_t gemgain = RandomVarGain.Exp(GEMGain);
       Double_t gemgain = RandomVarGain.Gaus(GEMGain,GEMGain*0.01);
       
       vector<Double_t> vCharge = CalcCharge(energydepositedbyhit, steplength, gemgain);
       Double_t hitcharge = vCharge[0]; // charge in fC
       Double_t nDeltaE = vCharge[1]; // number of electrons before gem
       Double_t nEAfterGem = vCharge[2]; // number after gem gain

	// Adding diffusion - loop over delta electrons and smear each one
	// smear the transverse drift of each electron to get the charge cloud size
	Double_t sigmatransverse = DT * TMath::Sqrt(ztogem);
	// smear the longitudinal drift of each electron to get time window 
	Double_t sigmalongitudinal = DL * TMath::Sqrt(ztogem); 
	// Int_t nelectrons = floor(nDeltaE);
	Int_t nelectrons = ceil(nDeltaE);

	vector<Double_t> vSmearedDriftTimes;
	vector<Double_t> vSmearedZDrift;
	vector<Double_t> vSmearedXHit;
	vector<Double_t> vSmearedYHit;
	vector<Double_t> vSmearedRHit;

	for(Int_t electronInc=0; electronInc<nelectrons; electronInc++){

	  // time smearing
	  Double_t TimeSmearDist = RandomVar.Gaus(0.0,sigmalongitudinal); //cm
	  Double_t SmearedDistToGEM = ztogem + TimeSmearDist; //cm
	  if(SmearedDistToGEM<0.0) SmearedDistToGEM = ztogem; // if we over subtract - set to original
	  vSmearedZDrift.push_back(SmearedDistToGEM); // in cm
	  // convert z to gem from cm to um, result will be ns
	  Double_t SmearedTimeDriftToGEM = HitTime + (SmearedDistToGEM*10000.0)/DriftVelocity;
	  vSmearedDriftTimes.push_back(SmearedTimeDriftToGEM);
	  // hDriftTime->Fill(SmearedTimeDriftToGEM/1000.0); // convert to us

	  // position smearing, all should be in cm, must convert g4 from m
	  Double_t xe_smeared = (hitx*100.0) + RandomVar.Gaus(0.0,sigmatransverse);
	  vSmearedXHit.push_back(xe_smeared);
	  Double_t ye_smeared = (hity*100.0) + RandomVar.Gaus(0.0,sigmatransverse);
	  vSmearedYHit.push_back(ye_smeared);
	  vSmearedRHit.push_back(TMath::Sqrt(xe_smeared*xe_smeared + ye_smeared*ye_smeared));

	}// end electron loop, this is delta electrons produced in mtpc gas

	// now do the GEM smearing if we have delta electrons
	// for moment double layer gem
	// variables for smearing by gems
	// gem 1
	vector<Double_t> vSmearedZGem1;
	vector<Double_t> vSmearedTimeGem1;
	vector<Double_t> vSmearedXGem1;
	vector<Double_t> vSmearedYGem1;
	vector<Double_t> vSmearedRGem1;
	// gem 2
	vector<Double_t> vSmearedZGem2;
	vector<Double_t> vSmearedTimeGem2;
	vector<Double_t> vSmearedXGem2;
	vector<Double_t> vSmearedYGem2;
	vector<Double_t> vSmearedRGem2;

	if(nelectrons>0){
	  // loop through the delta electrons for the hit
	  for(Int_t HitElectronInc=0; HitElectronInc<nelectrons; HitElectronInc++){
	    // loop through the hit delta electrons and add gem gain, smearing for each
	    // gem layer and each electron after smeared gain factor

	    for(Int_t GemGainInc=0; GemGainInc<gemgain; GemGainInc++){

	      // GEM LAYER 1
	      // time smearing, in ns
	      Double_t TimeSmearDistGEM1 = RandomVar.Gaus(0.0,sigmalongitudinal_GEM); //cm
	      Double_t SmearedDistToGEM1 = GEMGap + TimeSmearDistGEM1; //cm
	      vSmearedZGem1.push_back(SmearedDistToGEM1);// cm
	      Double_t SmearedTimeToGEM1 = vSmearedDriftTimes[HitElectronInc] + (SmearedDistToGEM1*10000.0)/DriftVelocity;
	      vSmearedTimeGem1.push_back(SmearedTimeToGEM1);//ns
	      // position smearing in cm
	      Double_t xSmearedGEM1 = vSmearedXHit[HitElectronInc] + RandomVar.Gaus(0.0,sigmatransverse_GEM);
	      Double_t ySmearedGEM1 = vSmearedYHit[HitElectronInc] + RandomVar.Gaus(0.0,sigmatransverse_GEM);
	      vSmearedXGem1.push_back(xSmearedGEM1);
	      vSmearedYGem1.push_back(ySmearedGEM1);
	      vSmearedRGem1.push_back(TMath::Sqrt(xSmearedGEM1*xSmearedGEM1 + ySmearedGEM1*ySmearedGEM1));

	      // GEM LAYER 2
	      // time smearing, in ns
	      Double_t TimeSmearDistGEM2 = RandomVar.Gaus(0.0,sigmalongitudinal_GEM); //cm
	      Double_t SmearedDistToGEM2 = GEMGap + TimeSmearDistGEM2; //cm
	      vSmearedZGem2.push_back(SmearedDistToGEM2);
	      Double_t SmearedTimeToGEM2 = SmearedTimeToGEM1 + (SmearedDistToGEM2*10000.0)/DriftVelocity;
	      vSmearedTimeGem2.push_back(SmearedTimeToGEM2);
	      // position smearing in cm
	      Double_t xSmearedGEM2 = xSmearedGEM1 + RandomVar.Gaus(0.0,sigmatransverse_GEM);
	      Double_t ySmearedGEM2 = ySmearedGEM1 + RandomVar.Gaus(0.0,sigmatransverse_GEM);
	      vSmearedXGem2.push_back(xSmearedGEM2);
	      vSmearedYGem2.push_back(ySmearedGEM2);
	      vSmearedRGem2.push_back(TMath::Sqrt(xSmearedGEM2*xSmearedGEM2 + ySmearedGEM2*ySmearedGEM2));
	      
	    } // gem gain loop
	  } // original electrons loop
	}// if we have delta electrons


	// ///////////
	// Check charge cloud sizes before and after double layer gems
	// ///////////
	//Before
	//smeared is in cm
	Int_t SizevSmearedRHit = vSmearedRHit.size();
	vector<Double_t> vSmearedRHitSorted(SizevSmearedRHit);
	copy(vSmearedRHit.begin(),vSmearedRHit.end(), vSmearedRHitSorted.begin());
	sort(vSmearedRHitSorted.begin(),vSmearedRHitSorted.end());
	Double_t ChargeDimension = fabs(vSmearedRHitSorted[SizevSmearedRHit-1] - vSmearedRHitSorted[0]);
	Double_t ChargeCloudRadius = (ChargeDimension*10.0)/2.0;
	Double_t ChargeCloudArea = TMath::Pi()*ChargeCloudRadius*ChargeCloudRadius; //mm^2
	// cout << "ChargeCloudArea in mm before GEMs " << ChargeCloudArea << endl;
	// hChargeCloudAreaBeforeGEMs->Fill(ChargeCloudArea);

	// After GEM 2
	Int_t SizevSmearedRGem2 = vSmearedRGem2.size();
	vector<Double_t> vSmearedRGem2Sorted(SizevSmearedRGem2);
	copy(vSmearedRGem2.begin(),vSmearedRGem2.end(), vSmearedRGem2Sorted.begin());
	sort(vSmearedRGem2Sorted.begin(),vSmearedRGem2Sorted.end());
	Double_t ChargeDimensionGem2 = fabs(vSmearedRGem2Sorted[SizevSmearedRGem2-1] - vSmearedRGem2Sorted[0]);
	Double_t ChargeCloudRadiusGem2 = (ChargeDimensionGem2*10.0)/2.0;
	Double_t ChargeCloudAreaGem2 = TMath::Pi()*ChargeCloudRadiusGem2*ChargeCloudRadiusGem2; // mm^2
	// cout << "ChargeCloudAreaGem2 in mm after GEMs " << ChargeCloudAreaGem2 << endl;
	// hChargeCloudAreaAfterGEMs->Fill(ChargeCloudAreaGem2);


	//
	// check a circle randomly matching charge cloud to find pads hit
	// digi pads
	vector<int> vhitpadsAfterGEMsID;
	vector<int> vhitpadsAfterGEMsRing;
	vector<int> vhitpadsAfterGEMsPad;
	vector<int> vhitpadsAfterGEMsPlane;
	vector<double> vhitpadsAfterGEMsTime;
	vector<double> vhitpadsAfterGEMsZDrift;
	vector<int> vhitpadcentre;
	vector<int> vhitpadgems;
	int npoints = 1000; // sample 1k points in circle - could probably reduce this
	if(SizevSmearedRHit>0){
	  // for(Int_t Deltainc=0; Deltainc<SizevSmearedRHit; Deltainc++){
	  for(Int_t Deltainc=0; Deltainc<1; Deltainc++){// only really care about any of the deltas to take as the circle centre
	    
	    double x0 = vSmearedXHit[Deltainc]*10.0;// convert from cm to mm 
	    double y0 = vSmearedYHit[Deltainc]*10.0;// convert from cm to mm
	    vhitpadcentre= GetPadLoadedQuick(x0,
					     y0,
					     hitz*1000.0,
					     hReadoutPads);

	    for(Int_t circleinc=0; circleinc<npoints; circleinc++){

	      double theta = RandomVarCircle.Uniform(0,2.0*TMath::Pi()); 
	      double radius = ChargeCloudRadiusGem2*TMath::Sqrt(RandomVarCircle.Uniform());
	      
	      double x = x0 + radius * TMath::Cos(theta);
	      double y = y0 + radius * TMath::Sin(theta);
	      
	      // get hit pad in digi
	      // GetPadLoaded works in mm
	      vhitpadgems = GetPadLoadedQuick(x,
					      y,
					      hitz*1000.0,
					      hReadoutPads);
	      
	      if(vhitpadgems[1]!=-999 && vhitpadgems[2]!=-999 && vhitpadgems[3]!=-999){
		vhitpadsAfterGEMsID.push_back(vhitpadgems[0]);
		vhitpadsAfterGEMsRing.push_back(vhitpadgems[1]);
		vhitpadsAfterGEMsPad.push_back(vhitpadgems[2]);
		vhitpadsAfterGEMsPlane.push_back(vhitpadgems[3]);
		vhitpadsAfterGEMsTime.push_back(vSmearedDriftTimes[Deltainc]); // ns
		vhitpadsAfterGEMsZDrift.push_back(vSmearedZDrift[Deltainc]/100.0); // convert z to gem into m
	      }

	      // cout << "circle  ring " << vhitpadgems[1] << " pad " << vhitpadgems[2]
	      // 	 << " plane " << vhitpadgems[3] << endl;
	      
	      vhitpadgems.clear();
	      
	    }//circle points
	    vhitpadcentre.clear();
	  }// delta e for
	}// if we have delta electrons

	// remove duplicate pads for the text file
	vector<int> vhitpadsAfterGEMsIDred;
	vector<int> vhitpadsAfterGEMsRingred;
	vector<int> vhitpadsAfterGEMsPadred;
	vector<int> vhitpadsAfterGEMsPlanered;
	vector<double> vhitpadsAfterGEMsTimered;
	vector<double> vhitpadsAfterGEMsZDriftred;

	std::unordered_set<int> seen;
	std::vector<size_t> keep_indices;

	for(size_t i=0; i<vhitpadsAfterGEMsID.size(); i++){
	  if(seen.find(vhitpadsAfterGEMsID[i]) == seen.end()){
	    seen.insert(vhitpadsAfterGEMsID[i]);
	    keep_indices.push_back(i);
	  }
	}

	auto filter_int = [&](std::vector<int>& v){
	  std::vector<int> temp;
	  for(size_t i : keep_indices) temp.push_back(v[i]);
	  v=temp;
	};
	auto filter_double = [&](std::vector<double>& v){
	  std::vector<double> temp;
	  for(size_t i : keep_indices) temp.push_back(v[i]);
	  v=temp;
	};

	filter_int(vhitpadsAfterGEMsID);
	filter_int(vhitpadsAfterGEMsRing);
	filter_int(vhitpadsAfterGEMsPad);
	filter_int(vhitpadsAfterGEMsPlane);
	filter_double(vhitpadsAfterGEMsTime);
	filter_double(vhitpadsAfterGEMsZDrift);


	// write out
	// for(size_t i=0; i<vhitpadsAfterGEMsID.size(); i++){
	//   TxtFileOut << "\t" << right << vhitpadsAfterGEMsTime[i] << "\t"
	// 	      << right << energydepositedbyhit  << "\t"
	// 	      << right << vhitpadsAfterGEMsRing[i] << "\t"
	// 	      << right << vhitpadsAfterGEMsPad[i] << "\t"
	// 	      << right << vhitpadsAfterGEMsPlane[i] << "\t"
	// 	      << right << vhitpadsAfterGEMsZDrift[i] << "\t"
	// 	      << endl; // convert z to gem in m
	// }// loop of hit vector size

	for(size_t i=0; i<vhitpadsAfterGEMsID.size(); i++){
	  TxtFileOut << "\t" << right << vhitpadsAfterGEMsTime[i] << "\t"
		     << right << energydepositedbyhit  << "\t"
		     << right << hitx << "\t"
		     << right << hity << "\t"
		     << right << hitz << "\t"
		     << right << vhitpadsAfterGEMsRing[i] << "\t"
		     << right << vhitpadsAfterGEMsPad[i] << "\t"
		     << right << vhitpadsAfterGEMsPlane[i] << "\t"
		     << right << vhitpadsAfterGEMsZDrift[i] << "\t"
		     << endl; // convert z to gem in m
	}// loop of hit vector size
       
     }// hit loop
     
     EventCounter++;
   } //------------------------------------------------------------ end event loop
 
 cout << endl;
 
 //============================================================================
 
 
 // CLOSE FILES
 TxtFileOut.close();

 
 
    } //------------------------------------------------------------ end main function
  

vector<Double_t> CalcCharge(Double_t Edep, Double_t PathLength, Double_t GemGain){
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
