#ifndef DMSPLOTTER_H
#define DMSPLOTTER_H

#include <utility>
#include <vector>

#include <TGClient.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TH1D.h>
#include <TFile.h>
#include <TMarker.h>
#include <TLine.h>
#include <TGaxis.h>

class DMSPlotter : public TGMainFrame {
  public:
    DMSPlotter(const TGWindow *p, UInt_t w, UInt_t h, TFile* histFile);
    virtual ~DMSPlotter();
    void OnNextEventButtonClick();
    void OnPreviousEventButtonClick();
    void OnGoToEventButtonClick();
    void OnExitButtonClick();
    void LoadHistograms();
    void DrawHistograms();
    void HandleResize();

  private:
    TRootEmbeddedCanvas* fEmbeddedCanvas;
    TGHorizontalFrame*   fButtonFrame;
    UInt_t		           fButtonFrameHeight;
    TGTextButton*        fNextEventButton;
    TGTextButton*        fPreviousEventButton;
    TGTextEntry*         fEventEntry;
    TGTextButton*        fGoToEventButton;
    TGHorizontalFrame*   fHistFrame;
    TGaxis*              fRightAxis;
    TFile*		           fHistFile;
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
    TMarker*		         fDet1PulseStartMarker;
    TMarker*             fDet2PulseStartMarker;
    TMarker*             fChe1PulseStartMarker;
    TMarker*             fChe2PulseStartMarker;
    TMarker*             fDet1PulseEndMarker;
    TMarker*             fDet2PulseEndMarker;
    TMarker*             fChe1PulseEndMarker;
    TMarker*             fChe2PulseEndMarker;
    TLine*               fDet1ThresholdLine;
    TLine*               fDet2ThresholdLine;
    TLine*               fChe1ThresholdLine;
    TLine*               fChe2ThresholdLine;
    UInt_t               fNumberOfZeroCrossing;
    UInt_t               fNumberOfPulse;

    ClassDef(DMSPlotter, 0)
};

#endif

