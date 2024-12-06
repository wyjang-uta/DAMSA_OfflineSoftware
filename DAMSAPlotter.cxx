#include <iostream>
#include <utility>
#include <vector>

#include <TApplication.h>
#include <TCanvas.h>
#include <TVirtualFFT.h>
#include <TMath.h>
#include <TMarker.h>
#include <TGaxis.h>

#include "DAMSAPlotter.h"
#include "DAMSAMathUtils.h"

DAMSAPlotter::DAMSAPlotter(const TGWindow *p, UInt_t w, UInt_t h, TFile* histFile) : TGMainFrame(p, w, h), fEventNumber(1), fHistFile(histFile)
{// {{{
	fButtonFrameHeight = 10;
	fMaxEventNumber = fHistFile->GetNkeys()/4;

	// 히스토그램 표시용 프레임을 추가
	fHistFrame = new TGHorizontalFrame(this, w-5, h-20);
	fEmbeddedCanvas = new TRootEmbeddedCanvas("EmbeddedCanvas", fHistFrame, w - 20, h - fButtonFrameHeight - 40);
	fHistFrame->AddFrame(fEmbeddedCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	AddFrame(fHistFrame, new TGLayoutHints(kLHintsExpandX | kLHintsTop, 10, 10, 10, 5));

	// Resize 이벤트가 발생할 때마다 HandleResize 함수를 호출
	this->Connect("ProcessedEvent(Event_t*)", "DAMSAPlotter", this, "HandleResize()");

	// 세로로 정렬될 버튼 프레임을 메인 프레임에 추가
	fButtonFrame = new TGHorizontalFrame(this, w-5, 10);

	// Previous Event 버튼 추가
	fPreviousEventButton = new TGTextButton(fButtonFrame, "Previous Event");
	fPreviousEventButton->Connect("Clicked()", "DAMSAPlotter", this, "OnPreviousEventButtonClick()");
	fButtonFrame->AddFrame(fPreviousEventButton, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 10, 10, 10, 5));

	// Next Event 버튼 추가
	fNextEventButton = new TGTextButton(fButtonFrame, "Next Event");
	fNextEventButton->Connect("Clicked()", "DAMSAPlotter", this, "OnNextEventButtonClick()");
	fButtonFrame->AddFrame(fNextEventButton, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 10, 10, 10, 5));

	// Event text entry 추가
	fEventEntry = new TGTextEntry(fButtonFrame, new TGTextBuffer(10));
	fButtonFrame->AddFrame(fEventEntry, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 10, 10, 10, 5));

	// Goto Event 버튼 추가
	fGoToEventButton = new TGTextButton(fButtonFrame, "Go");
	fGoToEventButton->Connect("Clicked()", "DAMSAPlotter", this, "OnGoToEventButtonClick()");
	fButtonFrame->AddFrame(fGoToEventButton, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 10, 10, 10, 5));

	// Exit 버튼 추가
	TGTextButton *exitButton = new TGTextButton(fButtonFrame, "Exit");
	exitButton->Connect("Clicked()", "DAMSAPlotter", this, "OnExitButtonClick()");
	fButtonFrame->AddFrame(exitButton, new TGLayoutHints(kLHintsCenterX | kLHintsTop, 10, 10, 10, 10));

	AddFrame(fButtonFrame, new TGLayoutHints(kLHintsCenterX | kLHintsCenterY, 10, 10, 10, 10));

	// 마커 초기화

  fDet1PulseFound = false;
  fDet2PulseFound = false;
  fChe1PulseFound = false;
  fChe2PulseFound = false;
	fDet1PulseStartMarker = new TMarker();
	fDet2PulseStartMarker = new TMarker();
	fChe1PulseStartMarker = new TMarker();
	fChe2PulseStartMarker = new TMarker();
	fDet1PulseEndMarker = new TMarker();
	fDet2PulseEndMarker = new TMarker();
	fChe1PulseEndMarker = new TMarker();
	fChe2PulseEndMarker = new TMarker();

	// Threshold line 초기화
	fDet1ThresholdLine = new TLine();
	fDet2ThresholdLine = new TLine();
	fChe1ThresholdLine = new TLine();
	fChe2ThresholdLine = new TLine();

	// 히스토그램 로드하고 그리기
	LoadHistograms();
	DrawHistograms();

	SetWindowName("DAMSAPlotter ROOT GUI");

	// 초기 크기 설정 (버튼이 표시될 수 있도록)
	Resize(w+1,h+1);  // this is a trick to resize correctly.
	Resize(w,h);  // 너비 200, 높이 100으로 설정, here the +1 is a magic number. 
	MapSubwindows();   // 모든 서브 윈도우를 맵핑
	MapWindow();       // 메인 프레임을 맵핑하여 표시
}// }}}

void DAMSAPlotter::LoadHistograms()
{// {{{
	fDet1Histogram = (TH1D*)fHistFile->Get(Form("det1_evt_%d", fEventNumber));
	fDet2Histogram = (TH1D*)fHistFile->Get(Form("det2_evt_%d", fEventNumber));
	fChe1Histogram = (TH1D*)fHistFile->Get(Form("che1_evt_%d", fEventNumber));
	fChe2Histogram = (TH1D*)fHistFile->Get(Form("che2_evt_%d", fEventNumber));
	fDet1HistogramKDE = DAMSA_MathUtils::PerformKDE(fDet1Histogram);
	fDet2HistogramKDE = DAMSA_MathUtils::PerformKDE(fDet2Histogram);
	fChe1HistogramKDE = DAMSA_MathUtils::PerformKDE(fChe1Histogram);
	fChe2HistogramKDE = DAMSA_MathUtils::PerformKDE(fChe2Histogram);
  // Calculate derivatives
  fDet1HistogramKDE_prime = DAMSA_MathUtils::GetDerivative(fDet1HistogramKDE);
  fDet2HistogramKDE_prime = DAMSA_MathUtils::GetDerivative(fDet2HistogramKDE);
  fChe1HistogramKDE_prime = DAMSA_MathUtils::GetDerivative(fChe1HistogramKDE);
  fChe2HistogramKDE_prime = DAMSA_MathUtils::GetDerivative(fChe2HistogramKDE);
	float pulseStart, pulseEnd;
	float pedestal;

  // Determine the pulse range of detector 1
	pedestal = DAMSA_MathUtils::GetPedestal(fDet1Histogram, 50, 70);
  std::cout << "Detector 1 Pedestal: " << pedestal << '\n';
	fDet1ThresholdLine->SetX1(0);
	fDet1ThresholdLine->SetX2(204.8);
	fDet1ThresholdLine->SetY1(pedestal);
	fDet1ThresholdLine->SetY2(pedestal);
	fDet1ThresholdLine->SetLineColor(1);
	fDet1PulseFound = DAMSA_MathUtils::GetPulseRange(pedestal, &pulseStart, &pulseEnd, fDet1HistogramKDE);
  if( fDet1PulseFound )
  {
    fDet1PulseStartMarker->SetX(pulseStart);
    fDet1PulseStartMarker->SetY(fDet1HistogramKDE->GetBinContent(fDet1HistogramKDE->FindBin(pulseStart)));
    fDet1PulseStartMarker->SetMarkerStyle(20);
    fDet1PulseEndMarker->SetX(pulseEnd);
    fDet1PulseEndMarker->SetY(fDet1HistogramKDE->GetBinContent(fDet1HistogramKDE->FindBin(pulseEnd)));
    fDet1PulseEndMarker->SetMarkerStyle(20);
  }
  std::cout << "Det1 pulse found?: " << fDet1PulseFound << std::endl;
  std::cout << "Det1 pulse start position: " << pulseStart << std::endl;
  std::cout << "Det1 pulse end position: " << pulseEnd << std::endl;
  std::cout << "Det1 PulseEnd_y: " << fDet1HistogramKDE->GetBinContent(fDet1HistogramKDE->FindBin(pulseEnd)) << std::endl;

  // Determine the pulse range of detector 1
	pedestal = DAMSA_MathUtils::GetPedestal(fDet2Histogram, 1, 10);
  std::cout << "Detector 2 Pedestal: " << pedestal << '\n';
	fDet2ThresholdLine->SetX1(0);
	fDet2ThresholdLine->SetX2(204.8);
	fDet2ThresholdLine->SetY1(pedestal);
	fDet2ThresholdLine->SetY2(pedestal);
	fDet2ThresholdLine->SetLineColor(2);
	fDet2PulseFound = DAMSA_MathUtils::GetPulseRange(pedestal, &pulseStart, &pulseEnd, fDet2HistogramKDE);
  if( fDet2PulseFound ) {
    fDet2PulseStartMarker->SetX(pulseStart);
    fDet2PulseStartMarker->SetY(fDet2HistogramKDE->GetBinContent(fDet2HistogramKDE->FindBin(pulseStart)));
    fDet2PulseStartMarker->SetMarkerStyle(20);
    fDet2PulseEndMarker->SetX(pulseEnd);
    fDet2PulseEndMarker->SetY(fDet2HistogramKDE->GetBinContent(fDet2HistogramKDE->FindBin(pulseEnd)));
    fDet2PulseEndMarker->SetMarkerStyle(20);
  }
  std::cout << "Det2 pulse found?: " << fDet2PulseFound << std::endl;
  std::cout << "Det2 pulse start position: " << pulseStart << std::endl;
  std::cout << "Det2 pulse end position: " << pulseEnd << std::endl;
  std::cout << "Det2 PulseEnd_y: " << fDet2HistogramKDE->GetBinContent(fDet2HistogramKDE->FindBin(pulseEnd)) << std::endl;

  pedestal = DAMSA_MathUtils::GetPedestal(fChe1Histogram, 1, 10);
	//fChe1PulseFound = GetPulseRange(fChe1HistogramKDE, &pulseStart, &pulseEnd, GetCherenkovPedestal(fChe1Histogram));
	fChe1PulseStartMarker->SetX(pulseStart);
	fChe1PulseStartMarker->SetY(fChe1HistogramKDE->GetBinContent(fChe1HistogramKDE->FindBin(pulseStart)));
	fChe1PulseStartMarker->SetMarkerStyle(20);
	fChe1PulseEndMarker->SetX(pulseEnd);
	fChe1PulseEndMarker->SetY(fChe1HistogramKDE->GetBinContent(fChe1HistogramKDE->FindBin(pulseEnd)));
	fChe1PulseEndMarker->SetMarkerStyle(20);
  pedestal = DAMSA_MathUtils::GetPedestal(fChe1Histogram, 1, 10);
	//fChe2PulseFound = GetPulseRange(fChe2HistogramKDE, &pulseStart, &pulseEnd, GetCherenkovPedestal(fChe2Histogram));
	fChe2PulseStartMarker->SetX(pulseStart);
	fChe2PulseStartMarker->SetY(fChe2HistogramKDE->GetBinContent(fChe2HistogramKDE->FindBin(pulseStart)));
	fChe2PulseStartMarker->SetMarkerStyle(20);
	fChe2PulseEndMarker->SetX(pulseEnd);
	fChe2PulseEndMarker->SetY(fChe2HistogramKDE->GetBinContent(fChe2HistogramKDE->FindBin(pulseEnd)));
	fChe2PulseEndMarker->SetMarkerStyle(20);
}// }}}

void DAMSAPlotter::DrawHistograms()
{// {{{
	fEmbeddedCanvas->GetCanvas()->cd();  // 캔버스 선택

	// 히스토그램 배열
	TH1D* histograms[4] = {
		fDet1Histogram,
		fDet2Histogram,
		fChe1Histogram,
		fChe2Histogram
	};

  TH1D* histograms_KDE[4] = {
    fDet1HistogramKDE,
    fDet2HistogramKDE,
    fChe1HistogramKDE,
    fChe2HistogramKDE
  };

  TH1D* derivatives[4] = {
    fDet1HistogramKDE_prime,
    fDet2HistogramKDE_prime,
    fChe1HistogramKDE_prime,
    fChe2HistogramKDE_prime
  };

  // 히스토그램 시각화 속성 부여
  fDet1Histogram->SetTitle("Detector 1");
  fDet2Histogram->SetTitle("Detector 2");
  fChe1Histogram->SetTitle("Cherenkov 1");
  fChe2Histogram->SetTitle("Cherenkov 2");
  fDet1HistogramKDE->SetTitle("Detector 1 (KDE)");
  fDet2HistogramKDE->SetTitle("Detector 2 (KDE)");
  fChe1HistogramKDE->SetTitle("Cherenkov 1 (KDE)");
  fChe2HistogramKDE->SetTitle("Cherenkov 2 (KDE)");


	// y축 범위 계산
	float minY = 1e10;
	float maxY = -1e10;
	for (int i = 0; i < 4; ++i) {
		float histMin = histograms[i]->GetMinimum();
		float histMax = histograms[i]->GetMaximum();
		if (histMin < minY) minY = histMin;
		if (histMax > maxY) maxY = histMax;
	}
	float yMin = minY * 0.9; // 최소값보다 10% 낮은 값
	float yMax = maxY * 1.1; // 최대값보다 10% 높은 값

  // 미분그래프 스케일링
  double oldMin = -32, oldMax = 32;  // 그룹2의 y축 범위
  yMin = 0; yMax = 4096;     // 그룹1의 y축 범위
  for (int i = 1; i <= fDet1HistogramKDE_prime->GetNbinsX(); i++) {
    double oldValue = fDet1HistogramKDE_prime->GetBinContent(i);
    double scaledValue = yMin + (oldValue - oldMin) * (yMax - yMin) / (oldMax - oldMin);
    fDet1HistogramKDE_prime->SetBinContent(i, scaledValue);
    oldValue = fDet2HistogramKDE_prime->GetBinContent(i);
    scaledValue = yMin + (oldValue - oldMin) * (yMax - yMin) / (oldMax - oldMin);
    fDet2HistogramKDE_prime->SetBinContent(i, scaledValue);
    oldValue = fChe1HistogramKDE_prime->GetBinContent(i);
    scaledValue = yMin + (oldValue - oldMin) * (yMax - yMin) / (oldMax - oldMin);
    fChe1HistogramKDE_prime->SetBinContent(i, scaledValue);
    oldValue = fChe2HistogramKDE_prime->GetBinContent(i);
    scaledValue = yMin + (oldValue - oldMin) * (yMax - yMin) / (oldMax - oldMin);
    fChe2HistogramKDE_prime->SetBinContent(i, scaledValue);
  }

  // 히스토그램 스타일링
	for (int i = 0; i < 4; ++i)
  {
    histograms_KDE[i]->SetAxisRange(yMin, yMax, "Y");
    histograms_KDE[i]->SetLineColor(i+1);
    histograms_KDE[i]->SetLineStyle(2);
    histograms_KDE[i]->SetLineWidth(2);
    derivatives[i]->SetLineColor(i+1);
    derivatives[i]->SetLineStyle(3);
  }

	// 히스토그램 그리기
	for (int i = 0; i < 4; ++i) {
		histograms[i]->SetAxisRange(yMin, yMax, "Y");
		histograms[i]->SetLineColor(i+1);

		if (i == 0) {
			histograms[i]->Draw();
      histograms_KDE[i]->Draw("same");
      derivatives[i]->Draw("same");
		} else {
			histograms[i]->Draw("SAME");
      histograms_KDE[i]->Draw("same");
      derivatives[i]->Draw("same");
		}
	}
  fEmbeddedCanvas->GetCanvas()->BuildLegend();

  // 두 번째 axis 그리기
  fRightAxis = new TGaxis(204.8, yMin, 204.8, yMax, oldMin, oldMax, 510, "+L");
  fRightAxis->SetTitle(Form("Derivatives (%3.1f to %3.1f)",oldMin, oldMax));
  fRightAxis->SetTitleColor(kRed);
  fRightAxis->SetLineColor(kRed);
  fRightAxis->SetLabelColor(kRed);
  fRightAxis->Draw();

	// 마커 그리기
  TMarker* markers[4][2] = {
    {fDet1PulseStartMarker, fDet1PulseEndMarker},
    {fDet2PulseStartMarker, fDet2PulseEndMarker},
    {fChe1PulseStartMarker, fChe1PulseEndMarker},
    {fChe2PulseStartMarker, fChe2PulseEndMarker},
  };

  for( int i = 0; i < 4; i++ ) {
    for( int j = 0; j < 2; j++ ) {
      markers[i][0]->SetMarkerColor(i+1);
      markers[i][1]->SetMarkerColor(i+1);
      markers[i][0]->SetMarkerStyle(21);
      markers[i][1]->SetMarkerStyle(21);
      markers[i][0]->SetMarkerSize(2);
      markers[i][1]->SetMarkerSize(2);
    }
  }
  if( fDet1PulseFound ) { fDet1PulseStartMarker->Draw(); fDet1PulseEndMarker->Draw(); }
  if( fDet2PulseFound ) { fDet2PulseStartMarker->Draw(); fDet2PulseEndMarker->Draw(); }
  if( fChe1PulseFound ) { fChe1PulseStartMarker->Draw(); fChe1PulseEndMarker->Draw(); }
  if( fChe2PulseFound ) { fChe2PulseStartMarker->Draw(); fChe2PulseEndMarker->Draw(); }

	// Threshold line 그리기
	fDet1ThresholdLine->Draw();
	fDet2ThresholdLine->Draw();


	fEmbeddedCanvas->GetCanvas()->Update(); // 캔버스 업데이트

	// 버튼 활성화/비활성화
	if( fEventNumber >= fMaxEventNumber ){
		fNextEventButton->SetState(kButtonDisabled);
	} else {
		fNextEventButton->SetState(kButtonUp);
	}
	if( fEventNumber <= 1 ) {
		fPreviousEventButton->SetState(kButtonDisabled);
	} else {
		fPreviousEventButton->SetState(kButtonUp);
	}

}// }}}

DAMSAPlotter::~DAMSAPlotter() {
	Cleanup();
}

void DAMSAPlotter::OnNextEventButtonClick() {
	fEventNumber++;// {{{
	std::cout << "////////////////////////////////////////////////////////////\nCurrent Event Number: " << fEventNumber << std::endl;

	LoadHistograms();
	DrawHistograms();
}// }}}

void DAMSAPlotter::OnPreviousEventButtonClick() {
	fEventNumber--; // 이벤트 번호 감소 {{{
	std::cout << "////////////////////////////////////////////////////////////\nCurrent Event Number: " << fEventNumber << std::endl;

	// 새로운 히스토그램 로드 및 그리기
	LoadHistograms();

	DrawHistograms(); // 히스토그램 다시 그리기
}// }}}

void DAMSAPlotter::OnGoToEventButtonClick() {
	const char* input = fEventEntry->GetText(); // 입력된 텍스트 가져오기{{{
	int eventNumber = std::atoi(input); // 문자열을 정수로 변환
	if( eventNumber >= 1 && eventNumber <= fMaxEventNumber )
	{
		fEventNumber = eventNumber;
		std::cout << "////////////////////////////////////////////////////////////\nCurrent Event Number: " << fEventNumber << '\n';

		// 새로운 히스토그램 로드 및 그리기
		LoadHistograms();
		DrawHistograms();
	}
	else
	{
		std::cerr << "Invalid event number!\n";
	}
}// }}}

// Exit 버튼 클릭 시 프로그램 종료
void DAMSAPlotter::OnExitButtonClick() {
	gApplication->Terminate(0);
}

// 창 크기 변경 시 호출되는 함수
void DAMSAPlotter::HandleResize() {
	// 메인 프레임의 현재 크기 얻기{{{
	int main_w = GetWidth();
	int main_h = GetHeight();

	// 캔버스 크기를 창 크기에 맞게 수동 조정
	fEmbeddedCanvas->Resize(main_w - 20, main_h - fButtonFrameHeight - 40); // 여백 고려하여 조정
	fEmbeddedCanvas->GetCanvas()->Modified();
	fEmbeddedCanvas->GetCanvas()->Update();
}// }}}


