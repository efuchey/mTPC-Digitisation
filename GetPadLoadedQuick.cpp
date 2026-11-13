#include <iostream>
#include <stdlib.h>

#include "TH2Poly.h"
#include "TMath.h"
#include "TCanvas.h"

#include <chrono>

using namespace std;

// Double_t Rout = 150.0;
// Double_t Rin = 50.0;//40.0;
// Double_t Rpad = 5.0;//3.0;//2.0;
// Int_t nPads = 144;//;

const Double_t Rout = 150.0;
const Double_t Rin = 50.0;//40.0;
const Int_t nPlanes = 10;
const Int_t nRings = 20;
//const Int_t nRings = 21;
const Double_t Rpad = (Rout-Rin)/nRings;
const Int_t nPads = 120;
//const Int_t nPads = 122;//120;

const Double_t nPads_mTPC = nPlanes*nRings*nPads;

vector<Int_t> GetPadLoadedQuick(Double_t x, Double_t y, Double_t z, TH2Poly *hPads){

  Double_t OuterRadius = Rout;
  Double_t InnerRadius = Rin;
  Double_t RingWidth = Rpad;
  const Int_t nRings = ceil( (OuterRadius-InnerRadius) / RingWidth);
  RingWidth = (OuterRadius-InnerRadius)/nRings;
  Int_t BinCounter=0;
  Int_t PadIDs[nRings*nPads];
  Int_t counter = -1;
  for(Int_t rInc=0; rInc<nRings; rInc++){
    for(Int_t pInc=0; pInc<nPads; pInc++){
      BinCounter++;
      counter++;
      PadIDs[counter] = BinCounter;
    }
  }
  
  Int_t plane = -999;
  if(z>=-250.0 && z<=-200.0) plane = 0;
  if(z>-200.0 && z<=-150.0) plane = 1;
  if(z>-150.0 && z<=-100.0) plane = 2;
  if(z>-100 && z<=-50.0) plane = 3;
  if(z>-50.0 && z<=0.0) plane = 4;
  if(z>0.0 && z<=50.0) plane = 5;
  if(z>50.0 && z<=100.0) plane = 6;
  if(z>100.0 && z<=150.0) plane = 7;
  if(z>150.0 && z<=200.0) plane = 8;
  if(z>200.0 && z<=250.0) plane = 9;
  // cout << endl;
  // cout <<"making readout pads" << endl;
  // cout << "Corresponding z plane is " << plane << endl;
  
  
  Int_t NPadBins=hPads->GetNumberOfBins();
  // cout << "NPadBins " << NPadBins << endl;
  Int_t hitbin = -999;
  TList *bin_list=hPads->GetBins();
  if( !bin_list ){
    cout << "Problem in generating pads for mtps in GetPad(), exitting." << endl;
    exit (EXIT_FAILURE);
  }
  // for(Int_t i=0; i<NPadBins; ++i){
  //   bool is_in = hPads->IsInsideBin(i, x, y);
  //   if( is_in == 1){
  //     hitbin = i;
  //     // cout <<  endl << "hit bin is " << hitbin << endl << endl;
  //   }//if
  // }// bin loop
  hitbin = hPads->FindBin(x,y);

  // if we couldn't find the bin for some reason smear it to the nearest mm
  if(hitbin==-999 || hitbin==-1){
    if(x<0.) x = 1.0*floor(x);
    else x = 1.0*ceil(x);
    if(y<0.) y = 1.0*floor(y);
    else y = 1.0*ceil(y);
    // for(Int_t i=0; i<NPadBins; ++i){
    //   bool is_in = hPads->IsInsideBin(i, x, y);
    //   if( is_in == 1)
    // 	hitbin = i;
    // }
    hitbin = hPads->FindBin(x,y);
  }
  // if(hitbin==-999){
  //   if(x<0.) x = 1.0*floor(x);
  //   else x = 1.0*ceil(x);
  //   if(y<0.) y = 1.0*floor(y);
  //   else y = 1.0*ceil(y);
  //   for(Int_t i=0; i<NPadBins; ++i){
  //     bool is_in = hPads->IsInsideBin(i, x, y);
  //     if( is_in == 1)
  // 	hitbin = i;
  //   }
  // }
  
  // check radius and pad
  Int_t RingID = -999;
  Int_t PadID = -999;
  Int_t inc = -1;
  for(Int_t ring=0; ring<nRings; ring++){
    for(Int_t pad=0; pad<nPads; pad++){
      inc++;
      if(PadIDs[inc]==hitbin){
	// cout << "PadIDs[inc] " << PadIDs[inc] << " hitbin " << hitbin
	//      << " ring is " << ring << " pad is " << pad << endl;
	RingID = ring;
	PadID = pad;
	break;
      }
    }
  }
  // fill vector to be returned
  vector<Int_t> IDsReturn;
  IDsReturn.push_back(hitbin);
  IDsReturn.push_back(RingID);
  IDsReturn.push_back(PadID);
  IDsReturn.push_back(plane);

  return IDsReturn;
  
  // return hitbin;
}


const double PhiStart[nRings] = {0., 0., 0., 0., 0.,
  0., 0., 0., 0., 0.,
  0., 0., 0., 0., 0.,
  0., 0., 0., 0., 0.};
const double DeltaPhi = 360.0/nPads;

void GetPadLoadedSuperQuick(Double_t x, Double_t y, Double_t z,
			    Int_t& PlaneID, Int_t& RingID, Int_t PadID){
  // Double_t OuterRadius = Rout;
  // Double_t InnerRadius = Rin;
  // Double_t RingWidth = Rpad;
  // const Int_t nRings = ceil( (OuterRadius-InnerRadius) / RingWidth);
    //RingWidth = (OuterRadius-InnerRadius)/nRings;
  
  // Int_t BinCounter=0;
  // Int_t PadIDs[nRings*nPads];
  // Int_t counter = -1;
  // for(Int_t rInc=0; rInc<nRings; rInc++){
  //   for(Int_t pInc=0; pInc<nPads; pInc++){
  //     BinCounter++;
  //     counter++;
  //     PadIDs[counter] = BinCounter;
  //   }
  // }

  Double_t R = sqrt(x*x+y*y);
  Double_t phi = atan2(y, x)*TMath::RadToDeg();
  
  //Int_t PlaneID = -999;
  for(Int_t i = 0; i<nPlanes; i++){
    if(-250.0+i*50.0 <= z && z<= -200.0+i*50.0){
      PlaneID = i;
      break;
    }
  }
  
  //Int_t RingID = -999;
  for(Int_t ring=0; ring<nRings; ring++){
    //if(Rin+ring*RingWidth <= R && R < Rin+(ring+1)*RingWidth ){
    if(Rin+ring*Rpad <= R && R < Rin+(ring+1)*Rpad ){
      RingID = ring;
      break;
    }
  }
  //Int_t PadID = -999;

  // check radius and pad
  Int_t inc = -1;
  // for(Int_t ring=0; ring<nRings; ring++){
  for(Int_t pad=0; pad<nPads; pad++){
    if(PhiStart[RingID]+pad*DeltaPhi <= phi && phi < PhiStart[RingID]+(pad+1)*DeltaPhi ){
      PadID = pad;
    }
    // inc++;
    // if(PadIDs[inc]==hitbin){
    //   // cout << "PadIDs[inc] " << PadIDs[inc] << " hitbin " << hitbin
    //   //      << " ring is " << ring << " pad is " << pad << endl;
    //   RingID = ring;
    //   PadID = pad;
    //   break;
    // }
  }
  // }
  // fill vector to be returned
  // vector<Int_t> IDsReturn;
  // //IDsReturn.push_back(hitbin);
  // IDsReturn.push_back(RingID);
  // IDsReturn.push_back(PadID);
  // IDsReturn.push_back(plane);

  // return IDsReturn;
  
  // return hitbin;
}
