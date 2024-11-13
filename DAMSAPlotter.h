#ifndef DAMSAPLOTTER_H
#define DAMSAPLOTTER_H

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

class DAMSAPlotter : public TGMainFrame {
public:
    DAMSAPlotter(const TGWindow *p, UInt_t w, UInt_t h, TFile* histFile);
    virtual ~DAMSAPlotter();
    void OnNextEventButtonClick();
    void OnPreviousEventButtonClick();
    void OnGoToEventButtonClick();
    void OnExitButtonClick();
    void LoadHistograms();
    void DrawHistograms();
    void HandleResize();
    float GaussianKernel(float x, float xi, float bandwidth);
    float GaussianKernelRegression(const std::vector<float>& x_data, const std::vector<float>& y_data, float x, float bandwidth);
    TH1D* PerformKDE(TH1D* hist, int nSmoothBins = 0, float bandwidth = 2);
    bool FindPulseRange(TH1D* hist, float* start, float* end, float threshold);
    std::pair<float, float> FindMeanAndVariance(TH1D* hist, int startBin, int endBin);
    float FindPedestal(TH1D* hist, float start_ns, float end_ns);
    float FindCherenkovPedestal(TH1D* hist);

private:
    TRootEmbeddedCanvas*  fEmbeddedCanvas;
    TGHorizontalFrame*    fButtonFrame;
    UInt_t		            fButtonFrameHeight;
    TGTextButton*         fNextEventButton;
    TGTextButton*         fPreviousEventButton;
    TGTextEntry*          fEventEntry;
    TGTextButton*         fGoToEventButton;
    TGHorizontalFrame*    fHistFrame;
    TFile*		            fHistFile;
    UInt_t		            fEventNumber;
    UInt_t		            fMaxEventNumber;
    TH1D*                 fDet1Histogram;
    TH1D*                 fDet2Histogram;
    TH1D*                 fChe1Histogram;
    TH1D*                 fChe2Histogram;
    TH1D*		              fDet1HistogramKDE;
    TH1D*		              fDet2HistogramKDE;
    TH1D*                 fChe1HistogramKDE;
    TH1D*                 fChe2HistogramKDE;
    bool                  fDet1PulseFound;
    bool                  fDet2PulseFound;
    bool                  fChe1PulseFound;
    bool                  fChe2PulseFound;
    TMarker*		          fDet1PulseStartMarker;
    TMarker*              fDet2PulseStartMarker;
    TMarker*              fChe1PulseStartMarker;
    TMarker*              fChe2PulseStartMarker;
    TMarker*              fDet1PulseEndMarker;
    TMarker*              fDet2PulseEndMarker;
    TMarker*              fChe1PulseEndMarker;
    TMarker*              fChe2PulseEndMarker;
    TLine*                fDet1ThresholdLine;
    TLine*                fDet2ThresholdLine;
    TLine*                fChe1ThresholdLine;
    TLine*                fChe2ThresholdLine;

    ClassDef(DAMSAPlotter, 0)
};

#endif

