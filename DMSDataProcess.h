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
    DMSDataProcess(const char* pInputFile, const char* pOutputFileName);
    ~DMSDataProcess();

    void ProcessFile();
    void CheckCherenkovSignals();
    void FindPulsePeak();
    void FindPulseTail();
    ULong64_t CountLinesInText();

  private:
std::string fInputFilePath;
std::string fOutputFileName;

    std::ifstream*  fInputStream;
    float           fAdcValue[1024];

    TFile*          fInputFile;
    TFile*          fOutputFile;
    TTree*          fTree;

    ULong64_t       fEventNumber;
    Int_t           fChannelNumber;
    ULong64_t       fMaxEventNumber;

    // Pulse processing variables (single-channel version)
    Int_t           fPreBaseSearch_i;
    Int_t           fPreBaseSearch_f;
    Int_t           fPostBaseSearch_i;
    Int_t           fPostBaseSearch_f;
    Int_t           fPrePulseSearch_i;
    Int_t           fPrePulseSearch_f;
    Int_t           fPostPulseSearch_i;
    Int_t           fPostPulseSearch_f;

    Int_t           fPulseFlag;
    Float_t         fPulsePeak;
    Int_t           fPulsePeakTimeBin;
    Int_t           fPulseStartBin;
    Int_t           fPulseTailBin;
    Int_t           fPulseEndBin;
    Float_t         fPulseIntegralTotal;
    Float_t         fPulseIntegralTail;
    Float_t         fPSD;

  ClassDef(DMSDataProcess, 0)
};

#endif

