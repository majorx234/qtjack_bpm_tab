///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of qtjack_bpm_tab                                    //
//    Copyright (C) 2022 <majorx234@googlemail.com>                          //
//                                                                           //
//    qtjack_bpm_tab is free software: you can redistribute it and/or modify //
//    it under the terms of the GNU General Public License as published by   //
//    the Free Software Foundation, either version 3 of the License, or      //
//    (at your option) any later version.                                    //
//                                                                           //
//    qtjack_bpm_tab is distributed in the hope that it will be useful,      //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//    GNU General Public License for more details.                           //
//                                                                           //
//    You should have received a copy of the GNU General Public License      //
//    along with qtjack_bpm_tab. If not, see <http://www.gnu.org/licenses/>. //
//                                                                           //
//    It is possible to obtain a closed-source license of QtJack.            //
//    If you're interested, contact me at: majorx234@gmail.com               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <cassert>
#include <cstring>
#include "beatthis/envelope_extractor.hpp"

EnvelopeExtractor::EnvelopeExtractor(size_t samples,
                   unsigned int sample_rate)
  : samples_(samples)
  , sample_rate_(sample_rate)
{
  hann_window_freq_ = fftw_alloc_complex(samples);
  signal_ = fftw_alloc_complex(samples);
  result_ = fftw_alloc_complex(samples);
  freqdomain_signal_ = fftw_alloc_complex(samples);
  timedomain_result_ = fftw_alloc_complex(samples);
  plan_forward_ = fftw_plan_dft_1d(samples, signal_, result_, 
                                   FFTW_FORWARD, FFTW_ESTIMATE);
  plan_backward_ = fftw_plan_dft_1d(samples, freqdomain_signal_, timedomain_result_,
                                    FFTW_BACKWARD, FFTW_ESTIMATE);

}

EnvelopeExtractor::~EnvelopeExtractor(){
  fftw_free(hann_window_freq_);
  fftw_free(signal_);
  fftw_free(result_);
  fftw_free(freqdomain_signal_);
  fftw_free(timedomain_result_);
  fftw_destroy_plan(plan_forward_);
  fftw_destroy_plan(plan_backward_);
}

void EnvelopeExtractor::calculate_halfhann_window_fct(
  double window_length,
  unsigned int max_freq)
{
  unsigned int hann_length = static_cast<unsigned int>(window_length*2*max_freq);

  // have same length as input signal set to zero
  for (int i=0;i<samples_;i++) {
    signal_[i][0] = 0;
    signal_[i][1] = 0;
  }
  assert(hann_length<samples_);

  // only part at start is set to half hann
  for (int i = 0; i < hann_length; ++i)
  {
    signal_[i][0] = 0.5-0.5*cos((M_PI * i) / hann_length);
  }

  // calculate freq domain of half hann window
  fftw_execute(plan_forward_);
  for(int i = 0;i<samples_;i++) {
    hann_window_freq_[i][0] = result_[i][0];
    hann_window_freq_[i][1] = result_[i][1];
  }
}

float** EnvelopeExtractor::extract_envelope(float** in_signals,
                         std::vector<unsigned int> bandlimits,
                         unsigned int max_freq,
                         double window_length) {
  unsigned int nbands = bandlimits.size();

  // init data structures
  float** output = (float**)malloc(nbands*sizeof(float*));
  for(int i =0;i<nbands;i++) {
    output[i] = (float*)malloc((samples_)*sizeof(float*));
    memset(output[i],0,(samples_)*sizeof(float*));
  }
  calculate_halfhann_window_fct(window_length, max_freq);

  for(int i = 0;i<nbands;i++) {
    // rectivy signal
    for(int j = 0;j<samples_;j++) {
      signal_[j][0] = std::abs(in_signals[i][j]);
      signal_[j][1] = 0;
    }
    fftw_execute(plan_forward_);
    // convolute input signals with half hanning
    // can be done in frequency domain (convolution -> multiplication)
    for(int j = 0;j<samples_;j++) {
      // complex multiplication
      freqdomain_signal_[j][0] = result_[j][0] * hann_window_freq_[j][0] - result_[j][1] * hann_window_freq_[j][1];
      freqdomain_signal_[j][1] = result_[j][0] * hann_window_freq_[j][1] + result_[j][1] * hann_window_freq_[j][0];
    }
    fftw_execute(plan_forward_);
    for(int j = 0;j<samples_;j++) {
      output[i][j] = sqrt(timedomain_result_[j][0] * timedomain_result_[i][0] +
                          timedomain_result_[i][1] * timedomain_result_[i][1]);
    }
  }
  return output;
}
