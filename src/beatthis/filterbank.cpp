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
  work_ = fftw_alloc_complex(samples);
  signal_ = fftw_alloc_complex(samples);
  result_ = fftw_alloc_complex(samples);
  freqdomain_signal_ = fftw_alloc_complex(samples);
  timedomain_result_ = fftw_alloc_complex(samples);
  plan_forward_ = fftw_plan_dft_1d(samples, signal_, result_, 
                                   FFTW_FORWARD, FFTW_ESTIMATE);
  plan_backward_ = fftw_plan_dft_1d(samples, freqdomain_signal_, timedomain_result_,
                                    FFTW_BACKWARD, FFTW_ESTIMATE);
  calculate_window_fct();

}

Filterbank::~Filterbank() {
  fftw_free(work_);
  fftw_free(window_);
  fftw_free(signal_);
  fftw_free(result_);
  fftw_free(freqdomain_signal_);
  fftw_free(timedomain_result_);
  fftw_destroy_plan(plan_forward_);
  fftw_destroy_plan(plan_backward_);
}


float** Filterbank::filter_signal(float* in_buffer,
  std::vector<unsigned int> bandlimits,
  unsigned int max_freq) {

  float ifft_factor = 1.0/samples_;

  // transform signal in frequency domain
  for(int i = 0;i<samples_;i++) {
    signal_[i][0] = window_[i] * static_cast<double>(in_buffer[i]);
    signal_[i][1] = 0;
  }
  fftw_execute(plan_forward_);
  for(int i = 0;i<samples_;i++) {
    work_[i][0] = result_[i][0];
    work_[i][1] = result_[i][1];
  }

  // create datastructure for nbands of the output signal
  unsigned int nbands = bandlimits.size();

  float** output = (float**)malloc(nbands*sizeof(float*));
  for(int i =0;i<nbands;i++) {
    output[i] = (float*)malloc((samples_)*sizeof(float*));
    memset(output[i],0,(samples_)*sizeof(float*));
  }

  unsigned int bl[nbands];
  unsigned int br[nbands];
  
  // calculate limits of filterbank
  for (int i = 0; i < nbands-1; ++i)
  {
    bl[i] = floor(((float)bandlimits[i]/(float)max_freq)*(samples_/2));
    br[i] = floor(((float)bandlimits[i+1]/(float)max_freq)*(samples_/2))-1;
  }
  bl[nbands-1] = floor(((float)bandlimits[nbands-1]/(float)max_freq)*(samples_/2));
  br[nbands-1] = floor(samples_/2);

  for(int i =0;i<nbands;i++) {
    // set freq domain to zero
    for(int j = 0;j<samples_;j++) {
      freqdomain_signal_[j][0] = 0;
      freqdomain_signal_[j][1] = 0;
    }
    // add only freq domain in limits of filterband
    for(int j = bl[i];j < br[i];j++) {
      freqdomain_signal_[j][0] = work_[j][0];
      freqdomain_signal_[j][1] = work_[j][1];
    }
    // add only freq domain in limits of filterband symetric:
    for(int j = bl[i];j < br[i];j++) {
      freqdomain_signal_[samples_-j-1][0] = work_[samples_-j-1][0];
      freqdomain_signal_[samples_-j-1][1] = work_[samples_-j-1][1];
    }
    // backtransformation of bands in time domain
    fftw_execute(plan_backward_);
    for(int j = 0;j<samples_;j++) {
      output[i][j] = ifft_factor*timedomain_result_[j][0];
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