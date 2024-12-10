#ifndef DMSDATAPROCESS_H
#define DMSDATAPROCESS_H

#include <string>

#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>

class DMSDataProcess : public TObject {
  public:
    DMSDataProcess();
    DMSDataProcess(char* pInputFileName, char* pOutputFileName);
    ~DMSDataProcess();
    void ProcessFile();
  private:
    char* fInputFileName;
    char* fOutputFileName;
    TFile* fInputFile;
    TFile* fOutputFile;
    TTree* fTree;
    UInt_t		           fEventNumber;
    UInt_t		           fMaxEventNumber;
    TH1D*                fDet1Histogram;
    TH1D*                fDet2Histogram;
    TH1D*                fChe1Histogram;
    TH1D*                fChe2Histogram;
    TH1D*		             fDet1HistogramKDE;
    TH1D*		             fDet2HistogramKDE;
    TH1D*                fChe1HistogramKDE;
    TH1D*                fChe2HistogramKDE;
    TH1D*		             fDet1HistogramKDE_prime;
    TH1D*		             fDet2HistogramKDE_prime;
    TH1D*                fChe1HistogramKDE_prime;
    TH1D*                fChe2HistogramKDE_prime;
    bool                 fDet1PulseFound;
    bool                 fDet2PulseFound;
    bool                 fChe1PulseFound;
    bool                 fChe2PulseFound;
    UInt_t               fNumberOfZeroCrossing;
    UInt_t               fNumberOfPulse;

  ClassDef(DMSDataProcess, 0)
};

#endif

