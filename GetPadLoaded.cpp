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

Double_t Rout = 150.0;
Double_t Rin = 50.0;//40.0;
Int_t nRings = 21;
Double_t Rpad = (Rout-Rin)/nRings;
Int_t nPads = 122;//120;

vector<Int_t> GetPadLoaded(Double_t x, Double_t y, Double_t z, TH2Poly *hPads){
// vector<Int_t> GetPadLoaded(Double_t x, Double_t y, Double_t z, TH2Poly *hPads, double &t1, double &t2){

  // hPads->SetName("hPads");
  // hPads->SetTitle("hPads");
  // hPads->Draw();

  // cout << "input x " << x << " y " << y << " z " << z << endl;

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
  for(Int_t i=0; i<NPadBins; ++i){
    bool is_in = hPads->IsInsideBin(i, x, y);
    if( is_in == 1){
      hitbin = i;
      // cout <<  endl << "hit bin is " << hitbin << endl << endl;
    }//bin loop
  }

  // if we couldn't find the bin for some reason smear it to the nearest mm
  if(hitbin==-999){
    if(x<0.) x = 1.0*floor(x);
    else x = 1.0*ceil(x);
    if(y<0.) y = 1.0*floor(y);
    else y = 1.0*ceil(y);
    for(Int_t i=0; i<NPadBins; ++i){
      bool is_in = hPads->IsInsideBin(i, x, y);
      if( is_in == 1)
    	hitbin = i;
    }
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
