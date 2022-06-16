#include <cmath>
#include "beatthis/filterbank.hpp"

// just for test reasons, will be replaced in future
void sine_wave( float values[],
                int freq,
                int freq_amp,
                int num_samples,
                int  offset,
                int sample_rate) {
  //int num_samples  = static_cast<int>(duration * sample_rate);
  for (int i =  offset; i < num_samples + offset; i++) {
    values[i-offset] =
        sin((2 * M_PI * freq_amp * i)/ sample_rate)
        * sin((2 * M_PI * freq * i) / sample_rate);
  }
}

int main(int argc, char *argv[]) {
  unsigned int samples = 96000;
  unsigned int sample_rate = 48000;
  float data1[samples];
  float data2[samples];
  float data3[samples];
  float data4[samples];
  float sum[samples];

  int freq1 = 200;
  int freq2 = 500;
  int freq3 = 1300;
  int freq4 = 2100;

  sine_wave(data1, freq1, 1, samples, 0, 48000);
  sine_wave(data2, freq2, 1, samples, 0, 48000);
  sine_wave(data3, freq3, 1, samples, 0, 48000);
  sine_wave(data4, freq4, 1, samples, 0, 48000);
  
  for (int i = 0; i < samples; i++) {
    sum[i] = data1[i] + data2[i] + data3[i] + data4[i];   
  }

  Filterbank filterbank(samples,sample_rate);
  float** result = filterbank.filter_signal(sum);
  for (int i = 0; i < samples; i++) {
    printf("%g\n",sum[i]);
  }

  for (int i = 0; i < samples; i++) {
    printf("%g\n",result[0][i]);
  }
  for (int i = 0;i<6;i++) {
    free(result[i]);
  }
  free(result);
  return 0;
}