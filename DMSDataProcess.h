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
    Int_t           fPreBaseSearch_i[4];
    Int_t           fPreBaseSearch_f[4];
    Int_t           fPreBaseSearchWidth[4];
    Int_t           fPostBaseSearch_i[4];
    Int_t           fPostBaseSearch_f[4];
    Int_t           fPostBaseSearchWidth[4];
    Int_t           fPrePulseSearch_i[4];
    Int_t           fPrePulseSearch_f[4];
    Int_t           fPostPulseSearch_i[4];
    Int_t           fPostPulseSearch_f[4];
    Float_t         fPreBaseline[4];
    Float_t         fPreBaseline_RMS[4];
    Float_t         fPostBaseline[4];
    Float_t         fPostBaseline_RMS[4];
    Float_t         fBaseline[4];
    Float_t         fBaselineDifference[4];
    Int_t           fPulseFlag[4];
    Float_t         fPulsePeak[4];
    Int_t           fPulsePeakTimeBin[4];
    Int_t           fPulseStartBin[4];
    Int_t           fPulseTailBin[4];
    Int_t           fPulseEndBin[4];
    Float_t         fPulseIntegralTotal[4];
    Float_t         fPulseIntegralTail[4];
    Float_t         fPSD[4];

  ClassDef(DMSDataProcess, 0)
};

#endif

