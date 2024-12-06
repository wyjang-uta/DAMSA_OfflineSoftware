#ifndef DAMSAMATHUTILS_H
#define DAMSAMATHUTILS_H

#include <vector>

#include <TMath.h>
#include <TH1D.h>

class DAMSA_MathUtils {
  public:
    static float GaussianKernel(float x, float xi, float bandwidth);
    static float GaussianKernelRegression(const std::vector<float>& x_data, const std::vector<float>& y_data, float x, float bandwidth);
    static TH1D* PerformKDE(TH1D* hist, int nSmoothBins = 0, float bandwidth = 2);
    static bool GetPulseRange(float threshold, float* start, float* end, TH1D* hist, float searchStart = 0.0f, float searchEnd = 200.0f);
    static std::pair<float, float> GetMeanAndVariance(TH1D* hist, int startBin, int endBin);
    static float GetPedestal(TH1D* hist, float start_ns, float end_ns);
    static float GetCherenkovPedestal(TH1D* hist);
    static TH1D* GetDerivative(TH1D* hist);
    static int GetNumberOfZeroCrossing(TH1D* hist);
};

#endif
