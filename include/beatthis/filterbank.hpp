#ifndef FILTERBANK_HPP_
#define FILTERBANK_HPP_ 

#include <vector>
#include <fftw3.h>

class Filterbank{
 public:
  enum class WindowFct {
    NONE = 0,
    HANNING = 1
  };  
  Filterbank(size_t samples,
             unsigned int sample_rate);
  ~Filterbank();
  float** filter_signal(float* signal,
    std::vector<unsigned int> bandlimits = {0,200,400,800,1600,3200 },
    unsigned int max_freq = 4096);
 private:
  void calculate_window_fct();
  size_t samples_;
  unsigned int sample_rate_;
  WindowFct window_fct_;
  double* window_;
  fftw_complex* work_;
  fftw_complex* signal_;
  fftw_complex* result_;
  fftw_complex* freqdomain_signal_;
  fftw_complex* timedomain_result_;
  fftw_plan plan_forward_;
  fftw_plan plan_backward_;
};

#endif // FILTERBANK_HPP_