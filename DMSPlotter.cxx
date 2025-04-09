#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

#include <TGClient.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TH1D.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TLine.h>
#include <TMarker.h>

#include "DMSPlotter.h"
#include "DMSMathUtils.h"

ClassImp(DMSPlotter)

DMSPlotter::DMSPlotter(const TGWindow *p, UInt_t w, UInt_t h, const std::string& waveformFilePath)
  : TGMainFrame(p, w, h), fEventNumber(1)
{
  fInputFilePath = waveformFilePath;
  fInputStreamSingle = new std::ifstream(fInputFilePath);
  if (!fInputStreamSingle->is_open()) {
    std::cerr << "Error: Cannot open file " << fInputFilePath << std::endl;
    exit(EXIT_FAILURE);
  }

  fMaxEventNumber = CountLinesInText() / 1024;
  std::cout << "Total number of events: " << fMaxEventNumber << std::endl;

  fDet1Histogram = new TH1D("h_det1", "Waveform;Time (ns);ADC", 1024, 0.0, 204.8);

  fButtonFrameHeight = 10;
  fHistFrame = new TGHorizontalFrame(this, w - 5, h - 20);
  fEmbeddedCanvas = new TRootEmbeddedCanvas("EmbeddedCanvas", fHistFrame, w - 20, h - fButtonFrameHeight - 40);
  fHistFrame->AddFrame(fEmbeddedCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
  AddFrame(fHistFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 10, 10, 10, 5));

  fButtonFrame = new TGHorizontalFrame(this, w - 5, 10);
  fPreviousEventButton = new TGTextButton(fButtonFrame, "Previous Event");
  fPreviousEventButton->Connect("Clicked()", "DMSPlotter", this, "OnPreviousEventButtonClick()");
  fButtonFrame->AddFrame(fPreviousEventButton);

  fNextEventButton = new TGTextButton(fButtonFrame, "Next Event");
  fNextEventButton->Connect("Clicked()", "DMSPlotter", this, "OnNextEventButtonClick()");
  fButtonFrame->AddFrame(fNextEventButton);

  fEventEntry = new TGTextEntry(fButtonFrame, new TGTextBuffer(10));
  fButtonFrame->AddFrame(fEventEntry);

  fGoToEventButton = new TGTextButton(fButtonFrame, "Go");
  fGoToEventButton->Connect("Clicked()", "DMSPlotter", this, "OnGoToEventButtonClick()");
  fButtonFrame->AddFrame(fGoToEventButton);

  TGTextButton* exitButton = new TGTextButton(fButtonFrame, "Exit");
  exitButton->Connect("Clicked()", "DMSPlotter", this, "OnExitButtonClick()");
  fButtonFrame->AddFrame(exitButton);
  AddFrame(fButtonFrame);

  LoadWaveform();
  DrawHistograms();

  SetWindowName("DMSPlotter Single File GUI");
  Resize(w + 1, h + 1);
  Resize(w, h);
  MapSubwindows();
  MapWindow();
}

void DMSPlotter::LoadWaveform() {
  fDet1Histogram->Reset();
  float adcValue;
  int bin = 1;

  const size_t offset = (fEventNumber - 1) * 1024;
  fInputStreamSingle->clear();
  fInputStreamSingle->seekg(0);

  std::string line;
  for (size_t i = 0; i < offset; ++i)
    std::getline(*fInputStreamSingle, line);

  while (bin <= 1024 && *fInputStreamSingle >> adcValue) {
    fDet1Histogram->SetBinContent(bin, adcValue);
    bin++;
  }
}

ULong64_t DMSPlotter::CountLinesInText() {
  int fd = open(fInputFilePath.c_str(), O_RDONLY);
  if (fd == -1) return 0;

  char buffer[4096];
  ssize_t bytesRead;
  long lineCount = 0;

  while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    for (ssize_t i = 0; i < bytesRead; ++i)
      if (buffer[i] == '\n') ++lineCount;

  close(fd);
  return lineCount;
}

DMSPlotter::~DMSPlotter() {
  if (fInputStreamSingle) {
    fInputStreamSingle->close();
    delete fInputStreamSingle;
  }
  delete fDet1Histogram;
  Cleanup();
}

void DMSPlotter::DrawHistograms() {
  fEmbeddedCanvas->GetCanvas()->cd();

  fDet1Histogram->SetTitle(Form("Waveform - Event %u", fEventNumber));

  float preBaseline = 0.0, postBaseline = 0.0;
  for (int i = 0; i < 20; ++i)
    preBaseline += fDet1Histogram->GetBinContent(i + 1);
  preBaseline /= 20;
  for (int i = 980; i < 1000; ++i)
    postBaseline += fDet1Histogram->GetBinContent(i + 1);
  postBaseline /= 20;
  float pedestal = 0.5 * (preBaseline + postBaseline);

  float min = fDet1Histogram->GetMinimum();
  float peak = pedestal - min;

  int startBin = -1;
  for (int i = 10; i < 1024; ++i) {
    float diff = pedestal - fDet1Histogram->GetBinContent(i + 1);
    if (diff > 30.0f) {
      startBin = i + 1;
      break;
    }
  }

  int tailBin = startBin + 270;
  int endBin = 1024;

  float integralTotal = 0.0f;
  float integralTail = 0.0f;

  for (int i = startBin; i <= endBin; ++i) {
    float signal = pedestal - fDet1Histogram->GetBinContent(i);
    integralTotal += signal;
    if (i >= tailBin) integralTail += signal;
  }

  float psd = (integralTotal > 0.0f) ? integralTail / integralTotal : 0.0f;

  fDet1Histogram->SetLineColor(4);
  fDet1Histogram->Draw();

  TLine* threshold = new TLine(0, pedestal, 204.8, pedestal);
  threshold->SetLineStyle(2);
  threshold->SetLineColor(1);
  threshold->Draw();

  TMarker* startMarker = new TMarker(fDet1Histogram->GetBinCenter(startBin), fDet1Histogram->GetBinContent(startBin), 20);
  TMarker* endMarker = new TMarker(fDet1Histogram->GetBinCenter(endBin), fDet1Histogram->GetBinContent(endBin), 20);
  startMarker->SetMarkerColor(1);
  endMarker->SetMarkerColor(1);
  startMarker->SetMarkerStyle(21);
  endMarker->SetMarkerStyle(21);
  startMarker->SetMarkerSize(2);
  endMarker->SetMarkerSize(2);
  startMarker->Draw();
  endMarker->Draw();

  std::cout << "Event: " << fEventNumber << std::endl;
  std::cout << "Baseline (Pedestal): " << pedestal << std::endl;
  std::cout << "Pulse Peak: " << peak << std::endl;
  std::cout << "Start Bin: " << startBin << std::endl;
  std::cout << "Tail Bin: " << tailBin << std::endl;
  std::cout << "End Bin: " << endBin << std::endl;
  std::cout << "Total Integral: " << integralTotal << std::endl;
  std::cout << "Tail Integral: " << integralTail << std::endl;
  std::cout << "PSD: " << psd << std::endl;

  fEmbeddedCanvas->GetCanvas()->Update();

  fNextEventButton->SetState(fEventNumber >= fMaxEventNumber ? kButtonDisabled : kButtonUp);
  fPreviousEventButton->SetState(fEventNumber <= 1 ? kButtonDisabled : kButtonUp);
}

void DMSPlotter::OnNextEventButtonClick() {
  fEventNumber++;
  LoadWaveform();
  DrawHistograms();
}

void DMSPlotter::OnPreviousEventButtonClick() {
  fEventNumber--;
  LoadWaveform();
  DrawHistograms();
}

void DMSPlotter::OnGoToEventButtonClick() {
  const char* input = fEventEntry->GetText();
  int eventNumber = std::atoi(input);
  if (eventNumber >= 1 && eventNumber <= fMaxEventNumber) {
    fEventNumber = eventNumber;
    LoadWaveform();
    DrawHistograms();
  } else {
    std::cerr << "Invalid event number!\n";
  }
}

void DMSPlotter::OnExitButtonClick() {
  gApplication->Terminate(0);
}

void DMSPlotter::HandleResize() {
  int main_w = GetWidth();
  int main_h = GetHeight();
  fEmbeddedCanvas->Resize(main_w - 20, main_h - fButtonFrameHeight - 40);
  fEmbeddedCanvas->GetCanvas()->Modified();
  fEmbeddedCanvas->GetCanvas()->Update();
}
