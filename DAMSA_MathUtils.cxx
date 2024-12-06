#include <iostream>
#include "DAMSA_MathUtils.h"

float DAMSA_MathUtils::GaussianKernel(float x, float xi, float bandwidth) {
  float diff = x - xi;/*{{{*/
  return exp( -0.5 * ( diff * diff ) / ( bandwidth * bandwidth ));
}/*}}}*/

float DAMSA_MathUtils::GaussianKernelRegression(const std::vector<float>& x_data, const std::vector<float>& y_data, float x, float bandwidth) {
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

TH1D* DAMSA_MathUtils::PerformKDE(TH1D* hist, int nSmoothBins, float bandwidth) {
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

bool DAMSA_MathUtils::GetPulseRange(float threshold, float* start, float* end, TH1D* hist, float searchStart, float searchEnd)
{/*{{{*/
  bool inPulse = false;
  bool found = false;
  float x, y, y_next;
  float pulseStart = 0.0f, pulseEnd = 0.0f;
  float minimumPulseWidth = 5.0f;
  int startBin, endBin;
  float reference_area;
  float pulse_area;

  startBin = hist->FindBin(searchStart);
  endBin = hist->FindBin(searchEnd);

  if( hist->GetMinimum() > threshold ){
    *start = -1.0f;
    *end = -1.0f;
    return false;
  }

  // Get pulse start
  for( size_t i = startBin; i <= hist->GetNbinsX(); ++i ) {
    x = hist->GetBinCenter(i);
    y = hist->GetBinContent(i);
    y_next = hist->GetBinContent(i+5);

    if( !inPulse && y < threshold ) {
      // Check if the data in the next 20 bins decreases monotonically
      bool monotonicDecrease = true;
      for( size_t j = i; j < i+20; j++){
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
  // Get pulse end
  if( inPulse )
  {
    for( size_t i = hist->GetMinimumBin(); i <= hist->GetNbinsX(); ++i ) {
      x = hist->GetBinCenter(i);
      y = hist->GetBinContent(i);
      if ( y >= threshold ) {
        pulseEnd = x;
        found = true;
        break;
      }
      else if ( i == hist->GetNbinsX() )
      {
        pulseEnd = x;
        found = true;
      }
    }
  }

  // when range could not found after scanned all data,
  // pulseEnd is set to be the very last element of the data
  if( pulseEnd == 0.0f ) {
    pulseEnd = hist->GetXaxis()->GetXmax()-1;
  }

  *start = pulseStart;
  *end = pulseEnd;

  return found;
}/*}}}*/

std::pair<float, float> DAMSA_MathUtils::GetMeanAndVariance(TH1D* hist, int startBin, int endBin)
{// {{{
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
}// }}}

float DAMSA_MathUtils::GetPedestal(TH1D* hist, float start_ns, float end_ns)
{// {{{
 // Pedestal finding algorithm
 //   Look for first 0 - 20 ns interval to get average ADC and standard deviation(sigma).
 //   Return average - 2.5 sigma
  int start_bin = hist->GetBin(start_ns);
  int end_bin = hist->GetBin(end_ns);
  int nbin = end_bin - start_bin + 1;
  float mean = 0.0f;
  float sigma = 0.0f;
  float bin_content;

  for( int i = start_bin; i <= end_bin; ++i ) {
    bin_content = hist->GetBinContent(i);
    mean += bin_content;
    sigma += bin_content * bin_content;
  }
  mean /= nbin;
  sigma = sqrt( sigma/nbin - (mean * mean) );

  return mean - 3.0 * sigma;
}// }}}

float DAMSA_MathUtils::GetCherenkovPedestal(TH1D* hist)
{// {{{
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

}// }}}

TH1D* DAMSA_MathUtils::GetDerivative(TH1D* hist)
{// {{{
 // return a derivative of a given histogram
  TH1D* deriv = (TH1D*)hist->Clone(TString(hist->GetName())+"_prime");
  float data[2] = {0.0f, 0.0f};
  float rate;
  for( int i = 1; i <= deriv->GetNbinsX(); ++i ){
    data[0] = hist->GetBinContent(i);
    data[1] = hist->GetBinContent(i+1);
    rate = 0.5 * ( data[1] - data[0]);
    deriv->SetBinContent( i+1, rate );
  }

  // treat the very first bin using linear interpolation
  float x2, x1;
  float y2, y1, y0;
  x1 = hist->GetBinCenter(1);
  x2 = hist->GetBinCenter(2);
  y1 = hist->GetBinContent(1);
  y2 = hist->GetBinContent(2);
  y0 - (x2-x1)/(y2-y1)*(deriv->GetBinCenter(1)-x1)+y1;
  deriv->SetBinContent(1, 0.5 * (y1 - y0));

  return deriv;
}// }}}

int DAMSA_MathUtils::GetNumberOfZeroCrossing(TH1D* hist)
{
  return 0;
}
