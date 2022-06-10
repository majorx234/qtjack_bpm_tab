#include <math.h>
#include <cassert>
#include <cstring>
#include "beatthis/filterbank.hpp"

Filterbank::Filterbank(size_t samples,
                   unsigned int sample_rate)
  : samples_(samples)
  , sample_rate_(sample_rate)
  , window_fct_(WindowFct::NONE)
  , window_(nullptr)
  , work_(nullptr)
{
  window_ = fftw_alloc_real(samples);
  work_ = fftw_alloc_real(samples);
  signal_ = fftw_alloc_complex(samples);
  result_ = fftw_alloc_complex(samples);
  p_ = fftw_plan_dft_1d(samples, signal_, result_, FFTW_FORWARD, FFTW_ESTIMATE);
  calculate_window_fct();

}

Filterbank::~Filterbank() {
  fftw_destroy_plan(p_);
  fftw_free(window_);
  fftw_free(signal_);
  fftw_free(result_);
}

float** Filterbank::filter_signal(float* in_buffer,
  std::vector<unsigned int> bandlimits,
  unsigned int max_freq){
  for(int i = 0;i<samples_;i++) {
    signal_[i][0] = window_[i] * static_cast<double>(in_buffer[i]);
    signal_[i][1] = 0;
  }
  fftw_execute(p_);
  for(int i = 0;i<samples_;i++) {
    work_[i] = sqrt(result_[i][0] * result_[i][0] +
                    result_[i][1] * result_[i][1]);
  }
  unsigned int nbands = bandlimits.size();
  float** output = (float**)malloc(nbands*sizeof(float*));
  for(int i =0;i<nbands;i++) {
    output[i] = (float*)malloc((samples_)*sizeof(float*));
    memset(output[i],0,(samples_)*sizeof(float*));
  }

  unsigned int bl[nbands];
  unsigned int br[nbands];
  
  for (int i = 0; i < nbands-1; ++i)
  {
    bl[i] = floor((bandlimits[i]/max_freq)*(samples_/2));
    br[i] = floor((bandlimits[i+1]/max_freq)*(samples_/2))-1;
  }
  bl[nbands-1] = floor((bandlimits[nbands-1]/max_freq)*(samples_/2));
  br[nbands-1] = floor(samples_/2);

  for(int i =0;i<nbands;i++) {
    for(int j = bl[i];j < br[i];j++) {
      output[i][j] = work_[j];
      output[i][(samples_-1)-j] = work_[(samples_-1)-j];
    }
  }

  return output;
}

void Filterbank::calculate_window_fct()
{
  for (int i=0; i<samples_; ++i) {
    double x = 0.0;

    switch (window_fct_) {
      case WindowFct::NONE:
        x = 1.0;
        break;
      case WindowFct::HANNING:
        x = 0.5 * (1 - cos((2 * M_PI * i) / (samples_ - 1)));
        break;
      default:
          assert(false);
    }
    window_[i] = x;
  }
}  