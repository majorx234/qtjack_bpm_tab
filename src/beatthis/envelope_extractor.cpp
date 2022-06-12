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
  hann_window_ = fftw_alloc_real(samples);

}

EnvelopeExtractor::~EnvelopeExtractor(){
  fftw_free(hann_window_);
}

void EnvelopeExtractor::calculate_hann_window_fct(
  unsigned int window_length,
  unsigned int max_freq)
{
  unsigned int hann_length = window_length*2*max_freq;
  for (int i=0;i<samples_;i++) {
    hann_window_[i] = 0;
  }
  assert(hann_length<samples_);
  for (int i = 0; i < hann_length; ++i)
  {
    hann_window_[i] = sin((M_PI * i) / (hann_length/2));
  }
}