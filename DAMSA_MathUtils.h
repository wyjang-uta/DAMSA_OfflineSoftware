#ifndef DAMSA_MATHUTILS_H
#define DAMSA_MATHUTILS_H

#include <vector>

#include <TMath.h>
#include <TH1D.h>

class DAMSA_MathUtils {
  public:
    static float GaussianKernel(float x, float xi, float bandwidth);
    static float GaussianKernelRegression(const std::vector<float>& x_data, const std::vector<float>& y_data, float x, float bandwidth);
    static TH1D* PerformKDE(TH1D* hist, int nSmoothBins = 0, float bandwidth = 2);
    static bool GetPulseRange(TH1D* hist, float* start, float* end, float threshold);
    static std::pair<float, float> GetMeanAndVariance(TH1D* hist, int startBin, int endBin);
    static float GetPedestal(TH1D* hist, float start_ns, float end_ns);
    static float GetCherenkovPedestal(TH1D* hist);
    static TH1D* GetDerivative(TH1D* hist);
};

#endif
