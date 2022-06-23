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

#ifndef COMBFILTER
#define COMBFILTER
#endif // COMBFILTER

#include <vector>
#include <fftw3.h>

class Combfilter
{
public:
  Combfilter(size_t samples,
             unsigned int sample_rate,
             unsigned int nbands);
  ~Combfilter();
  float bpm_refinement(float** signal,
    std::vector<unsigned int> bandlimits = {0,200,400,800,1600,3200 },
    unsigned int max_freq = 4096,
    unsigned int npulses = 3);

  float comb_convolute(float** signal,
    float accuracy = 2.0,
    float min_bpm = 60.0,
    float max_bpm = 200.0,
    std::vector<unsigned int> bandlimits = {0,200,400,800,1600,3200 },
    unsigned int max_freq = 4096,
    unsigned int npulses = 3);
private:
  size_t samples_;
  unsigned int sample_rate_;
  unsigned int nbands_;
  fftw_complex* signal_;
  fftw_complex* result_;
  fftw_complex* freqdomain_signal_;
  fftw_complex* timedomain_result_;
  fftw_plan plan_forward_;
  fftw_plan plan_backward_;
  fftw_complex** bands_freq_domain_;
  float* filter_;
  float* filter_freq_;
};
