#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <TMath.h>

#include "DMSDataProcess.h"
#include "DMSMathUtils.h"

DMSDataProcess::DMSDataProcess()
{
}

DMSDataProcess::DMSDataProcess(Char_t* pInputFile, Char_t* pOutputFile)
{// {{{
  fInputFileName = pInputFile;
  fOutputFileName = pOutputFile;
  fInputFile = TFile::Open(fInputFileName, "READ");
	fMaxEventNumber = fInputFile->GetNkeys()/4;
  fCherenkovInner = kFALSE;
  fCherenkovOuter = kFALSE;
  fOutputFile = new TFile(fOutputFileName, "RECREATE");
  fTree = new TTree("DMSNtuple", "DAMSA Ntuple");
  fTree->Branch("lCherenkovInner", &fCherenkovInner, "lCherenkovInner/I");
  fTree->Branch("lCherenkovOuter", &fCherenkovOuter, "lCherenkovOuter/I");
  fTree->Branch("lDet1MeanPedestal", &fDet1MeanPedestal, "lDet1MeanPedestal/F");
  fTree->Branch("lDet2MeanPedestal", &fDet2MeanPedestal, "lDet2MeanPedestal/F");
  fTree->Branch("lDet1PulseFlag", &fDet1PulseFound, "lDet1PulseFlag/O");
  fTree->Branch("lDet2PulseFlag", &fDet2PulseFound, "lDet2PulseFlag/O");
  fTree->Branch("lDet1PulsePeak", &fDet1PulsePeak, "lDet1PulsePeak/F");
  fTree->Branch("lDet2PulsePeak", &fDet2PulsePeak, "lDet2PulsePeak/F");
  fTree->Branch("lDet1PulseStart", &fDet1PulseStart, "lDet1PulseStart/F");
  fTree->Branch("lDet1PulseTail", &fDet1PulseTail, "lDet1PulseTail/F");
  fTree->Branch("lDet1PulseEnd", &fDet1PulseEnd, "lDet1PulseEnd/F");
  fTree->Branch("lDet2PulseStart", &fDet2PulseStart, "lDet2PulseStart/F");
  fTree->Branch("lDet2PulseTail", &fDet2PulseTail, "lDet2PulseTail/F");
  fTree->Branch("lDet2PulseEnd", &fDet2PulseEnd, "lDet2PulseEnd/F");
  fTree->Branch("lDet1PulseIntegralTotal", &fDet1PulseIntegralTotal, "lDet1PulseIntegralTotal/F");
  fTree->Branch("lDet1PulseIntegralTail", &fDet1PulseIntegralTail, "lDet1PulseIntegralTail/F");
  fTree->Branch("lDet2PulseIntegralTotal", &fDet2PulseIntegralTotal, "lDet2PulseIntegralTotal/F");
  fTree->Branch("lDet2PulseIntegralTail", &fDet2PulseIntegralTail, "lDet2PulseIntegralTail/F");
  fTree->Branch("lDet1PSD", &fDet1PSD, "lDet1PSD/F");
  fTree->Branch("lDet2PSD", &fDet2PSD, "lDet2PSD/F");

}// }}}

DMSDataProcess::~DMSDataProcess() {
  delete fTree;
  delete fOutputFile;
}

void DMSDataProcess::ProcessFile()
{// {{{
  std::cout << "Start loop over the input file: " << fInputFileName << std::endl;
  fOutputFile->cd();

  Char_t symbols[] = {'-', '/', '|', '\\'};
  Int_t symbolIndex = 0;
  ULong64_t currentProgress = 0;
  ULong64_t newProgress = 0;

  for( ULong64_t i = 0; i < fMaxEventNumber; ++i )
  {
    newProgress = ( i * 100 ) / fMaxEventNumber;
    if( newProgress > currentProgress )
    {
      currentProgress = newProgress;
      // Print rotator
      std::cout << '\r'
                << symbols[symbolIndex]
                << " Progressing: " << std::setw(3) << currentProgress << "% ("
                << i << "/" << fMaxEventNumber << ")"
                << std::flush;
      symbolIndex = ( symbolIndex + 1 ) % 4;
    }

    fDet1Histogram = (TH1D*)fInputFile->Get(Form("det1_evt_%d", i+1));
    fDet2Histogram = (TH1D*)fInputFile->Get(Form("det2_evt_%d", i+1));
    fChe1Histogram = (TH1D*)fInputFile->Get(Form("che1_evt_%d", i+1));
    fChe2Histogram = (TH1D*)fInputFile->Get(Form("che2_evt_%d", i+1));
    fDet1HistogramKDE = DMS_MathUtils::PerformKDE(fDet1Histogram);
    fDet2HistogramKDE = DMS_MathUtils::PerformKDE(fDet2Histogram);
    fChe1HistogramKDE = DMS_MathUtils::PerformKDE(fChe1Histogram);
    fChe2HistogramKDE = DMS_MathUtils::PerformKDE(fChe2Histogram);

    // Calculate derivatives
    fDet1HistogramKDE_prime = DMS_MathUtils::GetDerivative(fDet1HistogramKDE);
    fDet2HistogramKDE_prime = DMS_MathUtils::GetDerivative(fDet2HistogramKDE);
    fChe1HistogramKDE_prime = DMS_MathUtils::GetDerivative(fChe1HistogramKDE);
    fChe2HistogramKDE_prime = DMS_MathUtils::GetDerivative(fChe2HistogramKDE);

    // Determine the pulse range of detector 1
    fDet1MeanPedestal   = DMS_MathUtils::GetPedestal(fDet1Histogram, 50, 70);
    fDet1PulseFound     = DMS_MathUtils::GetPulseRange(fDet1MeanPedestal, &fDet1PulseStart, &fDet1PulseEnd, fDet1HistogramKDE);
    fDet1PulseStartBin  = fDet1HistogramKDE->FindBin(fDet1PulseStart);
    fDet1PulseEndBin    = fDet1HistogramKDE->FindBin(fDet1PulseEnd);
    fDet1PulseIntegralTotal = (fDet1PulseEndBin - fDet1PulseStartBin + 1) * fDet1MeanPedestal - fDet1Histogram->Integral(fDet1PulseStartBin, fDet1PulseEndBin);

    // Determine the pulse range of detector 2
    fDet2MeanPedestal   = DMS_MathUtils::GetPedestal(fDet2Histogram, 1, 10);
    fDet2PulseFound     = DMS_MathUtils::GetPulseRange(fDet2MeanPedestal, &fDet2PulseStart, &fDet2PulseEnd, fDet2HistogramKDE);
    fDet2PulseStartBin  = fDet2HistogramKDE->FindBin(fDet2PulseStart);
    fDet2PulseEndBin    = fDet2HistogramKDE->FindBin(fDet2PulseEnd);
    fDet2PulseIntegralTotal = (fDet2PulseEndBin - fDet2PulseStartBin + 1) * fDet2MeanPedestal - fDet2Histogram->Integral(fDet2PulseStartBin, fDet2PulseEndBin);

    FindPulsePeak();
    FindPulseTail();

    fDet1PulseIntegralTail = (fDet1PulseEndBin - fDet1PulseTailBin + 1) * fDet1MeanPedestal - fDet1Histogram->Integral(fDet1PulseTailBin, fDet1PulseEndBin);
    fDet2PulseIntegralTail = (fDet2PulseEndBin - fDet2PulseTailBin + 1) * fDet2MeanPedestal - fDet2Histogram->Integral(fDet2PulseTailBin, fDet2PulseEndBin);

    fDet1PSD = fDet1PulseIntegralTail / fDet1PulseIntegralTotal;
    fDet2PSD = fDet2PulseIntegralTail / fDet2PulseIntegralTotal;

    /*
    std::cout << "\n(Detector1) Total Pulse Integral: " << fDet1PulseIntegralTotal << std::endl;
    std::cout << "\n(Detector2) Total Pulse Integral: " << fDet2PulseIntegralTotal << std::endl;

    std::cout << "(Detector1) Pulse start (ns): " << fDet1PulseStart << ", Pulse tail (ns): " << fDet1PulseTail << ", Pulse end (ns): " << fDet1PulseEnd << std::endl;
    std::cout << "(Detector2) Pulse start (ns): " << fDet2PulseStart << ", Pulse tail (ns): " << fDet2PulseTail << ", Pulse end (ns): " << fDet2PulseEnd << std::endl;
    */

    // Pulse check for Cherenkov detectors
    fChe1MeanPedestal = DMS_MathUtils::GetPedestal(fChe1Histogram, 1, 10);
    fChe2MeanPedestal = DMS_MathUtils::GetPedestal(fChe1Histogram, 1, 10);
    CheckCherenkovSignals();

    fTree->Fill();
  }
  std::cout << '\r'
            << symbols[symbolIndex]
            << " Progressing: " << std::setw(3) << "100% ("
            << fMaxEventNumber << "/" << fMaxEventNumber << ")"
            << std::endl;
  symbolIndex = ( symbolIndex + 1 ) % 4;

  fTree->Write();
  fOutputFile->Close();
  fInputFile->cd();
  fInputFile->Close();
}// }}}

void DMSDataProcess::CheckCherenkovSignals()
{// {{{
  Float_t fCherenkovSearchStart = 150.0f;
  Float_t fCherenkovSearchEnd   = 200.0f;
  Int_t che1StartBin = fChe1HistogramKDE->FindBin(fCherenkovSearchStart);
  Int_t che1EndBin   = fChe1HistogramKDE->FindBin(fCherenkovSearchEnd);

  Float_t fChe1Minimum;
  Float_t fChe2Minimum;
  Float_t che1_value;
  Float_t che1_minimum = fChe1MeanPedestal;
  Float_t che2_value;
  Float_t che2_minimum = fChe2MeanPedestal;

  for(Int_t i = che1StartBin; i <= che1EndBin; ++i )
  {
    che1_value = fChe1HistogramKDE->GetBinContent(i);
    che2_value = fChe2HistogramKDE->GetBinContent(i);
    if( che1_value < che1_minimum )
      che1_minimum = che1_value;
    if( che2_value < che2_minimum )
      che2_minimum = che2_value;
  }

  if( che1_minimum < fChe1MeanPedestal - 20.0f )
  {
    fCherenkovInner = kTRUE;
  }
  if( che2_minimum < fChe2MeanPedestal - 20.0f )
  {
    fCherenkovOuter = kTRUE;
  }
}// }}}

void DMSDataProcess::FindPulsePeak()
{
  Float_t det1_value;
  Float_t det1_minimum = fDet1MeanPedestal;
  Float_t che2_value;
  Float_t che2_minimum = fDet2MeanPedestal;
  Float_t det1_minimum_time;
  Float_t che2_minimum_time;

  for( Int_t i = fDet1PulseStartBin; i <= fDet1PulseEndBin; ++i )
  {
    det1_value = fChe1HistogramKDE->GetBinContent(i);
    if( det1_value < det1_minimum )
    {
      det1_minimum = det1_value;
      det1_minimum_time = fChe1HistogramKDE->GetBinCenter(i);
    }
    che2_value = fChe2HistogramKDE->GetBinContent(i);
    if( che2_value < che2_minimum )
    {
      che2_minimum = che2_value;
      che2_minimum_time = fChe2HistogramKDE->GetBinCenter(i);
    }
  }
  fDet1PulsePeak = det1_minimum;
  fDet2PulsePeak = che2_minimum;
  fDet1PulsePeakTime = det1_minimum_time;
  fDet2PulsePeakTime = che2_minimum_time;
}

void DMSDataProcess::FindPulseTail()
{
  Int_t det1_peak_bin = fDet1HistogramKDE->FindBin(fDet1PulsePeakTime);
  Int_t det2_peak_bin = fDet2HistogramKDE->FindBin(fDet2PulsePeakTime);
  Float_t det1_value;
  Float_t det2_value;
  Float_t det1_inverted_value;
  Float_t det2_inverted_value;
  Float_t det1_tail_threshold = (fDet1MeanPedestal - fDet1PulsePeak) * 0.3f;
  Float_t det2_tail_threshold = (fDet2MeanPedestal - fDet2PulsePeak) * 0.3f;

  for( Int_t i = det1_peak_bin; i <= fDet1PulseEndBin; ++i )
  {
    det1_value = fDet1HistogramKDE->GetBinContent(i);
    det1_inverted_value = fDet1MeanPedestal - det1_value;

    if( det1_inverted_value < det1_tail_threshold )
    {
      fDet1PulseTail = fDet1HistogramKDE->GetBinCenter(i);
      fDet1PulseTailBin = i;
      break;
    }
  }
  for( Int_t i = det2_peak_bin; i <= fDet2PulseEndBin; ++i )
  {
    det2_value = fDet2HistogramKDE->GetBinContent(i);
    det2_inverted_value = fDet1MeanPedestal - det1_value;

    if( det2_inverted_value < det2_tail_threshold )
    {
      fDet2PulseTail = fDet2HistogramKDE->GetBinCenter(i);
      fDet2PulseTailBin = i;
      break;
    }
  }
}

