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

#include <cstdlib>
#include <cstring>
#include <cmath>
#include "beatthis/combfilter.hpp"

Combfilter::Combfilter(size_t samples,
                   unsigned int sample_rate)
  : samples_(samples)
  ,sample_rate_(sample_rate)
{
  signal_ = fftw_alloc_complex(samples);
  result_ = fftw_alloc_complex(samples);
  freqdomain_signal_ = fftw_alloc_complex(samples);
  timedomain_result_ = fftw_alloc_complex(samples);
  plan_forward_ = fftw_plan_dft_1d(samples, signal_, result_, 
                                   FFTW_FORWARD, FFTW_ESTIMATE);
  plan_backward_ = fftw_plan_dft_1d(samples, freqdomain_signal_, timedomain_result_,
                                    FFTW_BACKWARD, FFTW_ESTIMATE);

}

Combfilter::~Combfilter() {
  fftw_free(signal_);
  fftw_free(result_);
  fftw_free(freqdomain_signal_);
  fftw_free(timedomain_result_);
  fftw_destroy_plan(plan_forward_);
  fftw_destroy_plan(plan_backward_);
}

float Combfilter::comb_convolute(float** in_signals,
    float accuracy,
    float min_bpm,
    float max_bpm,
    std::vector<unsigned int> bandlimits,
    unsigned int max_freq,
    unsigned int npulses) {

  unsigned int nbands = bandlimits.size();

  //initialisation may better in cstr;
  fftw_complex** bands_freq_domain = (fftw_complex**)malloc(nbands*sizeof(fftw_complex*));
  float* filter = (float*)malloc(samples_*sizeof(float));
  float* filter_freq = (float*)malloc(samples_*sizeof(float));

  for(int i = 0;i<nbands;i++) {
    bands_freq_domain[i] = fftw_alloc_complex(samples_);
    for (int j = 0; j < samples_; ++j) {
      signal_[j][0] = in_signals[i][j];
      signal_[j][1] = 0;
    }
    fftw_execute(plan_forward_);
    for (int j = 0; j < samples_; ++j) {
      bands_freq_domain[i][j][0] = result_[j][0];
      bands_freq_domain[i][j][1] = result_[j][1];
    }
  }

  double max_energy = 0.0;

  float bpm = min_bpm;
  float result_bpm = min_bpm;
  while(bpm < max_bpm) {
    
    float energy = 0;
    memset(filter,0,samples_*sizeof(float));
    memset(filter_freq,0,samples_*sizeof(float));

    unsigned int nstep = floor((120/bpm)*max_freq);

    //double percent_done  = (100.0*(bpm-minbpm))/(maxbpm-minbpm);
    for(int i=0;i<npulses;i++) {
      filter[i*nstep] = 1.0;
    }    
    for (int j = 0; j < samples_; ++j) {
      signal_[j][0] = filter[j];
      signal_[j][1] = 0;
    }
    fftw_execute(plan_forward_);
    // calculate energy filter_freq*
    for (int j = 0; j < samples_; ++j) {
      filter_freq[j] = (result_[j][0] * result_[j][0])
                           +(result_[j][1] * result_[j][1]);
    }
    // no need for squareroot because for energy it is squared
    for(int i = 0;i<nbands;i++) {
      for(int j= 0;j<samples_;j++){
        float bfd = (bands_freq_domain[i][j][0] *bands_freq_domain[i][j][0])
                            +(bands_freq_domain[i][j][1] *bands_freq_domain[i][j][1]);
        energy += filter_freq[j] * bfd;
      }
    }
    if (energy > max_energy)
    {
      max_energy = energy;
      result_bpm = bpm;
    }
    bpm += accuracy;
  }

  free(filter);
  free(filter_freq);
  for (int i = 0; i < nbands; ++i)
  {
    fftw_free(bands_freq_domain[i]);
  }
  free(bands_freq_domain);
}
