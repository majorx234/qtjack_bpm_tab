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

#include "beatthis/differectifier.hpp"

#include <stdlib.h>
#include <cstring>

float** differectifier(float** in_signals,
                     size_t samples,
                     unsigned int nbands,
                     bool inplace) {
  float** output = nullptr;
  if(inplace){
    output = in_signals;
  } else {
    output = (float**)malloc(nbands*sizeof(float*));
    for(int i =0;i<nbands;i++) {
      output[i] = (float*)malloc((samples)*sizeof(float*));
      memset(output[i],0,(samples)*sizeof(float*));
    }
  }

  // calculate difference frome one sample to next for all bands
  for(int i =0;i<nbands;i++) {
    for(int j = 0;j<(samples-1);j++) {
      float difference = in_signals[i+1][j] - in_signals[i][j];
      output[i][j] = 0;

      // take only positive values -> half wave rectify)
      if(difference>0){
        output[i][j] = difference;
      }
 
    }
    output[i][samples-1] = 0;
  }
  return output;
}
