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