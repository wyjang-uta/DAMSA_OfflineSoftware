#ifndef DMSPLOTTER_H
#define DMSPLOTTER_H
#define NDIV 1024

#include <utility>
#include <vector>
#include <string>
#include <fstream>

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
#include <Rtypes.h>

class DMSPlotter : public TGMainFrame {
public:
    DMSPlotter(const TGWindow *p, UInt_t w, UInt_t h, const std::string& waveformFilePath);
    virtual ~DMSPlotter();

    void OnNextEventButtonClick();
    void OnPreviousEventButtonClick();
    void OnGoToEventButtonClick();
    void OnExitButtonClick();
    ULong64_t CountLinesInText();
    void LoadWaveform();
    void DrawHistograms();
    void HandleResize();

private:
    TGHorizontalFrame*   fHistFrame;
    TRootEmbeddedCanvas* fEmbeddedCanvas;
    UInt_t               fButtonFrameHeight;
    TGHorizontalFrame*   fButtonFrame;
    TGTextButton*        fNextEventButton;
    TGTextButton*        fPreviousEventButton;
    TGTextEntry*         fEventEntry;
    TGTextButton*        fGoToEventButton;
    TGaxis*              fRightAxis;

    std::ifstream*       fInputStreamSingle = nullptr;
    std::string          fInputFilePath;
    UInt_t               fEventNumber;
    UInt_t               fMaxEventNumber;
    TH1D*                fDet1Histogram = nullptr;
    
    ClassDef(DMSPlotter, 0)
};

#endif
