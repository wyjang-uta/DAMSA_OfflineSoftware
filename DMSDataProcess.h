#ifndef DMSDATAPROCESS_H
#define DMSDATAPROCESS_H

#include <string>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>

class DMSDataProcess : public TObject {
  public:
    DMSDataProcess();
    DMSDataProcess(char* pInputDirectory, char* pOutputFileName);
    ~DMSDataProcess();
    void ProcessFile();
    void CheckCherenkovSignals();
    void FindPulsePeak();
    void FindPulseTail();
    ULong64_t CountLinesInText();
  private:
    char*           fInputDirectory;
    char*           fOutputFileName;
    char*           fFullPathsToFiles[4];
    std::ifstream*  fInputStream[4];
    float           fAdcValue[4][1024];
    TFile*          fInputFile;
    TFile*          fOutputFile;
    TTree*          fTree;
    ULong64_t       fEventNumber;
    Int_t           fChannelNumber;
    ULong64_t       fMaxEventNumber;
    TH1D*           fDet1Histogram;
    TH1D*           fDet2Histogram;
    TH1D*           fChe1Histogram;
    TH1D*           fChe2Histogram;
    TH1D*           fDet1HistogramKDE;
    TH1D*           fDet2HistogramKDE;
    TH1D*           fChe1HistogramKDE;
    TH1D*           fChe2HistogramKDE;
    TH1D*           fDet1HistogramKDE_prime;
    TH1D*           fDet2HistogramKDE_prime;
    TH1D*           fChe1HistogramKDE_prime;
    TH1D*           fChe2HistogramKDE_prime;
    bool            fDet1PulseFound;
    bool            fDet2PulseFound;
    bool            fChe1PulseFound;
    bool            fChe2PulseFound;
    UInt_t          fNumberOfZeroCrossing;
    UInt_t          fNumberOfPulse;
    Int_t           fCherenkovInner;
    Int_t           fCherenkovOuter;
    Float_t         fPreBaseline[4];
    Float_t         fPreBaseline_RMS[4];
    Float_t         fPostBaseline[4];
    Float_t         fPostBaseline_RMS[4];
    Float_t         fBaselineDifference[4];
    Float_t         fDet1PulsePeak;
    Float_t         fDet1PulsePeakTime;
    Float_t         fDet2PulsePeak;
    Float_t         fDet2PulsePeakTime;
    Float_t         fDet1PulseStart;
    Int_t           fDet1PulseStartBin;
    Float_t         fDet1PulseEnd;
    Float_t         fDet1PulseEndBin;
    Float_t         fDet2PulseStart;
    Int_t           fDet2PulseStartBin;
    Float_t         fDet2PulseEnd;
    Int_t           fDet2PulseEndBin;
    Float_t         fDet1PulseTail;
    Int_t           fDet1PulseTailBin;
    Float_t         fDet2PulseTail;
    Int_t           fDet2PulseTailBin;
    Float_t         fDet1PulseIntegralTotal;
    Float_t         fDet1PulseIntegralTail;
    Float_t         fDet2PulseIntegralTotal;
    Float_t         fDet2PulseIntegralTail;
    Float_t         fDet1PSD;
    Float_t         fDet2PSD;

  ClassDef(DMSDataProcess, 0)
};

#endif

