// Rachel Montgomery: This macro models the SAMPA output pulse.
// An amount charge is inputted to the SAMPA chip.
// It is assumed the charge arrives over a uniform
// time window which is split into time bins.
// At each bin the sampa impulse respose for the charge
// collected during that bin's time window is calculated.
// The impulse pulses are then summed to yield the convoluted
// sampa pulse resulting from the total charge inputted.

#include <TROOT.h>
#include <TGraph.h>
#include <iostream>
#include <fstream>
#include <TChainElement.h>
#include <TChain.h>
#include <TMath.h>
#include <TH2Poly.h>
#include <TF1.h>
#include <TRandom3.h>
#include <TRandom.h>
#include <string>
#include <TString.h>
#include <TCanvas.h>
#include <TFile.h>

using namespace std;


// overload operator to sum vector of TF1 impulse resonses
struct SumTF1{
  SumTF1(const std::vector<TF1 *> & flist) : fFuncList(flist) {}
 double operator()(const double * x, const double *p){
    double result = 0;
    for(unsigned int i=0; i<fFuncList.size(); i++)
      result += fFuncList[i]->Eval(x[0]);
    return result;
    }
  std::vector<TF1*> fFuncList;
};



// general impulse response shape
double PulseShape(double *x, double *par){
  double f;
  if(x[0]<par[0])
    f = par[1];
  else
    f = ( (par[2])*TMath::Power(((x[0]-par[0])/par[3]),4.0)*TMath::Exp(-4.0*((x[0]-par[0])/par[3])) + par[1] );
  // in ed's version the first par[2] is divided by e^4
  return f;
}



//==============================================================================
// MAIN FUNCTION
// ShapingTime is the sampa shaping time
// all times are in units e-9, ie 160 is inputted but really means 40e-9
// Gain is gain factor for amplitude calculation in units ADC counts/fC
// B is the baseline in ADC counts
// Charge is charge of input pulse in fC
// w in time window for charge arrival
// PulseStartTime is the start of the charge arrival
// thresh is the threshold for sampling a pulse above threshold [ADC counts]
// nSamplesBefore is how many samples before pulse above threshold to include
// nSamplesAfter is how many samples after pulse above threshold to include

vector<Double_t> SampaShapingAndSample_ReturnVals(Double_t ShapingTime=160.0,Double_t Gain=10.06,
			   Double_t B=100.0, Double_t Charge=80.0,
			   Double_t w=40.0, Double_t PulseStartTime=10.0,
			   Double_t thresh=110.0,
			   Int_t nSamplesBefore=3, Int_t nSamplesAfter=7){
  // Default values in function definitions are for:
  // ShapingTime = 160ns for sampa
  // Charge = 80fC arriving on sampa
  // Uniform time window for charge arrival  w = 40ns
  // Charge pulse arriving at 10ns
  // Gain factor 10.06 ADC counts/fC

  // For 80ns shaping time the following values are relevant:
  // ShapingTime = 80ns for sampa
  // Gain = 11.86 ADC counts/fC

  // change timescale for 160ns setting above:
  // 50->25ns, would be shaping time 87ns with charge
  // arriving over 20-70ns rather thn 40 - 140ns.

  // vector to return
  vector<Double_t> vReturn;

  Double_t BinSize = 50.0; // re-bin plots in 50ns for better viewing
  Double_t p = ShapingTime/BinSize;
  Double_t TimeIntervals = 1.0/BinSize;
  Int_t nTimeIntervals = round(w);
  Double_t StartViewWindow = 0.0;//0.0;
  Double_t EndViewWindow = 100.0;//00.0;
  Double_t Amp = (TMath::Exp(4.0)* Gain * Charge)/w;
  Double_t Baseline = B/w;

  TF1 **fIntervalResponses = new TF1*[nTimeIntervals]; // interval impulses
  Double_t time = 0.0;
  std::vector<TF1 *> v; // vector of the impulse responses
  // new impulse response pulse at each new time bin in charge arrival window
  for(Int_t intervalInc=0; intervalInc<nTimeIntervals; intervalInc++){
    time = PulseStartTime + intervalInc*TimeIntervals;
    fIntervalResponses[intervalInc] = new TF1("fint",
					      PulseShape,
					      StartViewWindow,
					      EndViewWindow,
					      4);
    fIntervalResponses[intervalInc]->FixParameter(0,time);
    fIntervalResponses[intervalInc]->FixParameter(1,Baseline);
    fIntervalResponses[intervalInc]->FixParameter(2,Amp);
    fIntervalResponses[intervalInc]->FixParameter(3,p);
    v.push_back(fIntervalResponses[intervalInc]);
  }
  // TCanvas *cImpulsePulse = new TCanvas();
  // cImpulsePulse->cd();
  // fIntervalResponses[0]->Draw();

  // convolution of all impulse response functions
  TF1 * fsum = new TF1("fsum",
		       SumTF1(v),
		       StartViewWindow,
		       EndViewWindow,
		       0);
  // TCanvas *cConvultedPulse = new TCanvas();
  // cConvultedPulse->cd();
  // fsum->Draw();


  //------------------------------------------------------ Sampling
  // Sample the summes pulse at 20MHz, i.e. 50ns period

  // Start with baseline 80 ADC counts
  // Set a threshold of 90 ADC counts
  // if the pulse crosses the threshold the sampa chip will
  // output a cluster of ADC samples that includes the samples
  // over threshold and 3 samples before the initial threshold
  // crossing (up side of the pulse) and 7 samples after the
  // final threshold crossing (down side of the pulse) i.e.
  // # samples before threshold crossing = 3
  // # samples before threshold crossing = 7

  // 20MHz sample frequency = 50ns sampling period
  Double_t SampleFrequency = 20.0; // in units of MHz
  Double_t SamplePeriod = 50.0/BinSize; // in units of ns
  Double_t BinnedWindow = EndViewWindow - StartViewWindow;
  Int_t nSamplePoints = round(BinnedWindow/SamplePeriod);

  // want to randomize start of sampling since will not be synched with pulse each time
  TRandom3 RandomVar(0);
  Double_t TimeJitter = RandomVar.Uniform(-SamplePeriod/2.0, SamplePeriod/2.0);
  // cout <<  "SamplePeriod " << SamplePeriod << endl;
  // cout << "TimeJitter " << TimeJitter << endl;

  vector<Double_t> vSampledVals;
  vector<Double_t> vSampledTimes;

  Int_t ThreshCrossUpInc = -999;
  Double_t SampleTimeCrossUp = 0.0;
  for(Int_t SampleIncUp=0; SampleIncUp<nSamplePoints; SampleIncUp++){
    SampleTimeCrossUp = 1.0*SampleIncUp*SamplePeriod;
    // if((1.0*SampleIncUp*SamplePeriod + TimeJitter)<0.0)
    //   SampleTimeCrossUp = 0.0;
    // else
    //   SampleTimeCrossUp = 1.0*SampleIncUp*SamplePeriod + TimeJitter;
    Double_t tempval = fsum->Eval(SampleTimeCrossUp);
    if(tempval>thresh){
      ThreshCrossUpInc = SampleIncUp;
      break;
    }
  }// sampling loop to find threshold crossing up point
  Int_t ThreshCrossDownInc = -999;
  Double_t SampleTimeCrossDown = 0.0;
  for(Int_t SampleIncDown=ThreshCrossUpInc; SampleIncDown<nSamplePoints; SampleIncDown++){
    SampleTimeCrossDown = 1.0*SampleIncDown*SamplePeriod;
    // SampleTimeCrossDown = 1.0*SampleIncDown*SamplePeriod + TimeJitter;
    Double_t tempval = fsum->Eval(SampleTimeCrossDown);
    if(tempval<thresh){
      ThreshCrossDownInc = SampleIncDown-1;
      break;
    }
  }// sampling loop to find threshold crossing down point

  if(ThreshCrossUpInc>-999 && ThreshCrossDownInc>-999){
    Int_t SamplingStartInc = -999;
    Int_t SamplingEndInc = -999;
    if((ThreshCrossUpInc-nSamplesBefore)<0)
      SamplingStartInc=0;
    else SamplingStartInc = ThreshCrossUpInc-nSamplesBefore;
    if((ThreshCrossDownInc+nSamplesAfter)>(nSamplePoints-1))
      SamplingEndInc=(nSamplePoints-1);
    else SamplingEndInc = ThreshCrossDownInc+nSamplesAfter;
    Double_t SampleTimeAboveThresh = 0.0;
    for(Int_t SampleInc=SamplingStartInc; SampleInc<(SamplingEndInc+1); SampleInc++){
      SampleTimeAboveThresh = 1.0*SampleInc*SamplePeriod + TimeJitter;
      vSampledVals.push_back(fsum->Eval(SampleTimeAboveThresh));
      vSampledTimes.push_back(SampleTimeAboveThresh);
      // return vals
      vReturn.push_back(SampleTimeAboveThresh);
      vReturn.push_back(fsum->Eval(SampleTimeAboveThresh));
    }// loop through points above threshold

    // // for drawing
  //   Int_t nsampledvals = vSampledVals.size();
  //   Int_t nsampledtimes = vSampledTimes.size();
  //   if(nsampledvals==nsampledtimes){
  //     TGraph *gSampledPoints = new TGraph(nsampledvals,
  //   					  &vSampledTimes[0],
  //   					  &vSampledVals[0]);
  //     TCanvas *cSampled = new TCanvas();
  //     cSampled->cd();
  //     gSampledPoints->SetTitle("");
  //     gSampledPoints->GetXaxis()->SetTitle("Time [50ns binning]");
  //     gSampledPoints->GetXaxis()->SetLimits(StartViewWindow,
  //   					    EndViewWindow);
  //     gSampledPoints->GetYaxis()->SetLimits(0.0,
  //   					    1100.0);
  //     gSampledPoints->GetYaxis()->SetTitle("ADC Counts");
  //     gSampledPoints->SetMarkerStyle(20);
  //     // gSampledPoints->SetMarkerSize(20);
  //     gSampledPoints->Draw("AP");
  //     fsum->SetLineColor(kBlue);
  //     fsum->Draw("SAME");
  //   }
  //   else{
  //     cout << "The number of smapled times and values don't match, exitting"
  //   	   << endl;
  //     exit;
  //   }
  }// if we found a pulse above threshold
  else{
    cout << "We didn't cross threshold so we didn't sample anything!" << endl;
    exit;
  }//if we didn't find a pulse above threshold exit

  return vReturn;

} // SampaShapingAndSample
