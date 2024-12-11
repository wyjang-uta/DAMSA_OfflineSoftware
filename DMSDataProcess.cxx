#include <iostream>
#include <string>
#include <vector>

#include <TMath.h>

#include "DMSDataProcess.h"
#include "DMSMathUtils.h"

DMSDataProcess::DMSDataProcess()
{
}

DMSDataProcess::DMSDataProcess(char* pInputFile, char* pOutputFile)
{// {{{
  fInputFileName = pInputFile;
  fOutputFileName = pOutputFile;
  fInputFile = TFile::Open(fInputFileName, "READ");
	fMaxEventNumber = fInputFile->GetNkeys()/4;
  fOutputFile = new TFile(fOutputFileName, "RECREATE");
}// }}}

DMSDataProcess::~DMSDataProcess() {
  delete fOutputFile;
}

void DMSDataProcess::ProcessFile()
{
  fOutputFile->cd();
  for( UInt_t i = 1; i <= fMaxEventNumber; ++i )
  {
    fDet1Histogram = (TH1D*)fInputFile->Get(Form("det1_evt_%d", i));
    fDet2Histogram = (TH1D*)fInputFile->Get(Form("det2_evt_%d", i));
    fChe1Histogram = (TH1D*)fInputFile->Get(Form("che1_evt_%d", i));
    fChe2Histogram = (TH1D*)fInputFile->Get(Form("che2_evt_%d", i));
    fDet1HistogramKDE = DMS_MathUtils::PerformKDE(fDet1Histogram);
    fDet2HistogramKDE = DMS_MathUtils::PerformKDE(fDet2Histogram);
    fChe1HistogramKDE = DMS_MathUtils::PerformKDE(fChe1Histogram);
    fChe2HistogramKDE = DMS_MathUtils::PerformKDE(fChe2Histogram);
    // Calculate derivatives
    fDet1HistogramKDE_prime = DMS_MathUtils::GetDerivative(fDet1HistogramKDE);
    fDet2HistogramKDE_prime = DMS_MathUtils::GetDerivative(fDet2HistogramKDE);
    fChe1HistogramKDE_prime = DMS_MathUtils::GetDerivative(fChe1HistogramKDE);
    fChe2HistogramKDE_prime = DMS_MathUtils::GetDerivative(fChe2HistogramKDE);
    float pulseStart, pulseEnd;
    float pedestal;

    // Determine the pulse range of detector 1
    pedestal = DMS_MathUtils::GetPedestal(fDet1Histogram, 50, 70);
    fDet1PulseFound = DMS_MathUtils::GetPulseRange(pedestal, &pulseStart, &pulseEnd, fDet1HistogramKDE);
    //std::cout << "Event: " << i << ", Detector 1 - Pulse Start: " << pulseStart << ", Pulse End: " << pulseEnd << std::endl;

    // Determine the pulse range of detector 1
    pedestal = DMS_MathUtils::GetPedestal(fDet2Histogram, 1, 10);
    fDet2PulseFound = DMS_MathUtils::GetPulseRange(pedestal, &pulseStart, &pulseEnd, fDet2HistogramKDE);

    pedestal = DMS_MathUtils::GetPedestal(fChe1Histogram, 1, 10);
    //fChe1PulseFound = GetPulseRange(fChe1HistogramKDE, &pulseStart, &pulseEnd, GetCherenkovPedestal(fChe1Histogram));
    pedestal = DMS_MathUtils::GetPedestal(fChe1Histogram, 1, 10);
    //fChe2PulseFound = GetPulseRange(fChe2HistogramKDE, &pulseStart, &pulseEnd, GetCherenkovPedestal(fChe2Histogram));
  }
  fOutputFile->Close();
  fInputFile->Close();
}
