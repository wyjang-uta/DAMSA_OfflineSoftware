
#include <iostream>
#include <utility>
#include <vector>

#include <TApplication.h>
#include <TCanvas.h>
#include <TVirtualFFT.h>
#include <TMath.h>
#include <TMarker.h>

#include "DAMSAPlotter.h"

DAMSAPlotter::DAMSAPlotter(const TGWindow *p, UInt_t w, UInt_t h, TFile* histFile) : TGMainFrame(p, w, h), fEventNumber(1), fHistFile(histFile) {

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
}

void DAMSAPlotter::LoadHistograms() {
	fDet1Histogram = (TH1D*)fHistFile->Get(Form("det1_evt_%d", fEventNumber));
	fDet2Histogram = (TH1D*)fHistFile->Get(Form("det2_evt_%d", fEventNumber));
	fChe1Histogram = (TH1D*)fHistFile->Get(Form("che1_evt_%d", fEventNumber));
	fChe2Histogram = (TH1D*)fHistFile->Get(Form("che2_evt_%d", fEventNumber));
	fDet1HistogramKDE = PerformKDE(fDet1Histogram);
	fDet2HistogramKDE = PerformKDE(fDet2Histogram);
	fChe1HistogramKDE = PerformKDE(fChe1Histogram);
	fChe2HistogramKDE = PerformKDE(fChe2Histogram);
	float pulseStart, pulseEnd;
	float pedestal;

	pedestal = FindPedestal(fDet1Histogram);
	fDet1ThresholdLine->SetX1(0);
	fDet1ThresholdLine->SetX2(204.8);
	fDet1ThresholdLine->SetY1(pedestal);
	fDet1ThresholdLine->SetY2(pedestal);
	fDet1ThresholdLine->SetLineColor(1);
	FindPulseRange(fDet1HistogramKDE, &pulseStart, &pulseEnd, pedestal);
	fDet1PulseStartMarker->SetX(pulseStart);
	fDet1PulseStartMarker->SetY(fDet1HistogramKDE->GetBinContent(fDet1HistogramKDE->FindBin(pulseStart)));
	fDet1PulseStartMarker->SetMarkerStyle(20);
	fDet1PulseEndMarker->SetX(pulseEnd);
	fDet1PulseEndMarker->SetY(fDet1HistogramKDE->GetBinContent(fDet1HistogramKDE->FindBin(pulseEnd)));
	fDet1PulseEndMarker->SetMarkerStyle(20);
	pedestal = FindPedestal(fDet2Histogram, 50);
	fDet2ThresholdLine->SetX1(0);
	fDet2ThresholdLine->SetX2(204.8);
	fDet2ThresholdLine->SetY1(pedestal);
	fDet2ThresholdLine->SetY2(pedestal);
	fDet2ThresholdLine->SetLineColor(2);
	FindPulseRange(fDet2HistogramKDE, &pulseStart, &pulseEnd, pedestal);
	fDet2PulseStartMarker->SetX(pulseStart);
	fDet2PulseStartMarker->SetY(fDet2HistogramKDE->GetBinContent(fDet2HistogramKDE->FindBin(pulseStart)));
	fDet2PulseStartMarker->SetMarkerStyle(20);
	fDet2PulseEndMarker->SetX(pulseEnd);
	fDet2PulseEndMarker->SetY(fDet2HistogramKDE->GetBinContent(fDet2HistogramKDE->FindBin(pulseEnd)));
	fDet2PulseEndMarker->SetMarkerStyle(20);
	//FindPulseRange(fChe1HistogramKDE, &pulseStart, &pulseEnd, FindCherenkovPedestal(fChe1Histogram));
	fChe1PulseStartMarker->SetX(pulseStart);
	fChe1PulseStartMarker->SetY(fChe1HistogramKDE->GetBinContent(fChe1HistogramKDE->FindBin(pulseStart)));
	fChe1PulseStartMarker->SetMarkerStyle(20);
	fChe1PulseEndMarker->SetX(pulseEnd);
	fChe1PulseEndMarker->SetY(fChe1HistogramKDE->GetBinContent(fChe1HistogramKDE->FindBin(pulseEnd)));
	fChe1PulseEndMarker->SetMarkerStyle(20);
	//FindPulseRange(fChe2HistogramKDE, &pulseStart, &pulseEnd, FindCherenkovPedestal(fChe2Histogram));
	fChe2PulseStartMarker->SetX(pulseStart);
	fChe2PulseStartMarker->SetY(fChe2HistogramKDE->GetBinContent(fChe2HistogramKDE->FindBin(pulseStart)));
	fChe2PulseStartMarker->SetMarkerStyle(20);
	fChe2PulseEndMarker->SetX(pulseEnd);
	fChe2PulseEndMarker->SetY(fChe2HistogramKDE->GetBinContent(fChe2HistogramKDE->FindBin(pulseEnd)));
	fChe2PulseEndMarker->SetMarkerStyle(20);
}

void DAMSAPlotter::DrawHistograms() {
	fEmbeddedCanvas->GetCanvas()->cd();  // 캔버스 선택

	// 히스토그램 배열
	TH1D* histograms[8] = {
		fDet1Histogram,
		fDet2Histogram,
		fChe1Histogram,
		fChe2Histogram,
		fDet1HistogramKDE,
		fDet2HistogramKDE,
		fChe1HistogramKDE,
		fChe2HistogramKDE
	};

	// y축 범위 계산
	float minY = 1e10;
	float maxY = -1e10;
	for (int i = 0; i < 8; ++i) {
		float histMin = histograms[i]->GetMinimum();
		float histMax = histograms[i]->GetMaximum();
		if (histMin < minY) minY = histMin;
		if (histMax > maxY) maxY = histMax;
	}
	float yMin = minY * 0.9; // 최소값보다 10% 낮은 값
	float yMax = maxY * 1.1; // 최대값보다 10% 높은 값

	// 히스토그램 그리기
	for (int i = 0; i < 8; ++i) {
		histograms[i]->SetAxisRange(yMin, yMax, "Y");
		histograms[i]->SetLineColor(i+1);

		if (i == 0) {
			histograms[i]->Draw();
		} else {
			histograms[i]->Draw("SAME");
		}
	}
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
      markers[i][0]->Draw();
      markers[i][1]->Draw();
    }
  }

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

}

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


float DAMSAPlotter::GaussianKernel(float x, float xi, float bandwidth) {
	float diff = x - xi;/*{{{*/
	return exp( -0.5 * ( diff * diff ) / ( bandwidth * bandwidth ));
}/*}}}*/

float DAMSAPlotter::GaussianKernelRegression(const std::vector<float>& x_data, const std::vector<float>& y_data, float x, float bandwidth) {
	float numerator = 0.0f;/*{{{*/
	float denominator = 0.0f;

	for( size_t i = 0; i < x_data.size(); ++i )
	{
		float weight = GaussianKernel(x, x_data[i], bandwidth);
		numerator += weight * y_data[i];
		denominator += weight;
	}

	return numerator / denominator;
}/*}}}*/

TH1D* DAMSAPlotter::PerformKDE(TH1D* hist, int nSmoothBins, float bandwidth) {
	// KDE를 위한 새로운 히스토그램 생성{{{
	int nBins = hist->GetNbinsX();
	if( nSmoothBins == 0 ) nSmoothBins = nBins;
	float xMin = hist->GetXaxis()->GetXmin();
	float xMax = hist->GetXaxis()->GetXmax();

	TString kdeHistName = TString(hist->GetName()) + "_KDE";
	TString kdeHistTitle = "(KDE) " + TString(hist->GetTitle());
	TH1D* kdeHist = (TH1D*)hist->Clone(kdeHistName);
	kdeHist->SetTitle(kdeHistTitle);

	// 히스토그램에서 샘플링
	std::vector<float> xval;
	std::vector<float> yval;
	std::vector<float> esty;
	float estimatedY;
	for( int i = 1; i <= hist->GetNbinsX(); ++i ) {
		xval.push_back(hist->GetBinCenter(i));
		yval.push_back(hist->GetBinContent(i));
	}
	for( size_t i = 0; i < xval.size(); ++i ) {
		estimatedY = GaussianKernelRegression(xval, yval, xval[i], bandwidth);
		kdeHist->SetBinContent(i+1, estimatedY);
		esty.push_back(estimatedY);
	}

	return kdeHist;
}// }}}


bool DAMSAPlotter::FindPulseRange(TH1D* hist, float* start, float* end, float threshold) {
	bool inPulse = false;/*{{{*/
	bool found = false;
	float x, y, y_next;
	float pulseStart = 0.0f, pulseEnd = 0.0f;
	float minimumPulseWidth = 5.0f;
	int startBin, endBin;
	float reference_area;
	float pulse_area;
	if( hist->GetMinimum() > threshold ){
		*start = -1.0f;
		*end = -1.0f;
		return false;
	}

	// Find pulse start
	for( size_t i = 1; i <= hist->GetNbinsX(); ++i ) {
		x = hist->GetBinCenter(i);
		y = hist->GetBinContent(i);
		y_next = hist->GetBinContent(i+5);
		if( !inPulse && y < threshold ) {
			// check the next 30 bins monotonically decrease
			bool monotonicDecrease = true;
			for( size_t j = i; j < i+30; j++){
				float currentValue = hist->GetBinContent(j);
				float nextValue = hist->GetBinContent(j+1);
				if( currentValue <= nextValue )
				{
					monotonicDecrease = false;
					break;
				}
			}
			if( !monotonicDecrease )
			{
				inPulse = false;
				continue;
			}
			inPulse = true;
			pulseStart = x;
		}
	}
	// Find pulse end
	for( size_t i = hist->GetMinimumBin(); i <= hist->GetNbinsX(); ++i ) {
		x = hist->GetBinCenter(i);
		y = hist->GetBinContent(i);
	       	if ( y >= threshold ) {
			pulseEnd = x;
			found = true;
			break;
		}
	}
	/*
	for( size_t i = hist->FindBin(pulseStart); i < hist->GetNbinsX(); ++i ) {
		x = hist->GetBinCenter(i);
		y = hist->GetBinContent(i);
		y_next = hist->GetBinContent(i+5);
	       	if ( y >= threshold && y_next >= y ) {
			pulseEnd = x;
			found = true;
			break;
		}
	}
	*/

	// when range could not found after scanned all data,
	// pulseEnd is set to be the very last element of the data
	if( pulseEnd == 0.0f ) {
		pulseEnd = hist->GetXaxis()->GetXmax()-1;
	}
	std::cout << "(PulseStart, PulseEnd) = (" << pulseStart << ", " << pulseEnd << ")\n";

	*start = pulseStart;
	*end = pulseEnd;
	return found;
}/*}}}*/

std::pair<float, float> DAMSAPlotter::FindMeanAndVariance(TH1D* hist, int startBin, int endBin)
{
	float sum = 0.0f;
	float sumSquared = 0.0f;
	int numBins = endBin - startBin + 1;

	for (int bin = startBin; bin <= endBin; ++bin) {
		float value = hist->GetBinContent(bin);
		sum += value;
		sumSquared += value * value;
	}

	float mean = sum / numBins;
	float variance = (sumSquared / numBins) - (mean * mean);

	return std::make_pair(mean, variance);
}

float DAMSAPlotter::FindPedestal(TH1D* hist, int windowSize)
{
	int numBins = 500;
	float minVariance = std::numeric_limits<float>::max();
	float pedestal = 0.0f;

	// 슬라이딩 윈도우를 사용하여 분산이 가장 작은 구간 찾기
	for (int startBin = 1; startBin <= numBins - windowSize; ++startBin) {
		int endBin = startBin + windowSize - 1;
		auto [mean, variance] = FindMeanAndVariance(hist, startBin, endBin);

		if (variance < minVariance) {
			minVariance = variance;
			pedestal = mean;
		}
	}

	std::cout << hist->GetName() << " pedestal : " << pedestal << '\n';

	return pedestal - 2.5 * minVariance;
}

float DAMSAPlotter::FindCherenkovPedestal(TH1D* hist)
{
	int endbin = 30;
	float average = 0;
	float stddev = 0;
	float diff = 0;
	float threshold;
	int nbins = hist->GetNbinsX();
	for(int i = 1; i <= endbin; ++i){
		average += hist->GetBinContent(i);
	}
	average /= endbin;
	for(int i = 1; i <= endbin; ++i){
		diff = (average - hist->GetBinContent(i));
		stddev += diff*diff;
	}
	stddev /= endbin;
	stddev = sqrt(stddev);

	threshold = average - 5.0 * stddev;

	return threshold;

}
ClassImp(DAMSAPlotter)

