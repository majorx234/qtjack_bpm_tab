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

  for(int i =0;i<nbands;i++) {
    for(int j = 0;j<(samples-1);j++) {
      float difference = in_signals[i+1][j] - in_signals[i][j];
      output[i][j] = 0;
      if(difference>0){
        output[i][j] = difference;
      }
 
    }
  }
  return output;
}
