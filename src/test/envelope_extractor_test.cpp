#include <cmath>
#include <cstring>
#include <cassert>
#include "beatthis/filterbank.hpp"

// just for test reasons, will be replaced in future
void sine_wave( float values[],
                int freq,
                int freq_amp,
                int num_samples,
                int offset,
                int sample_rate) {
  //int num_samples  = static_cast<int>(duration * sample_rate);
  for (int i =  offset; i < num_samples + offset; i++) {
    values[i-offset] =
        sin((2 * M_PI * freq_amp * i)/ sample_rate)
        * sin((2 * M_PI * freq * i) / sample_rate);
  }
}

void exponential_decay( float values[],
                        unsigned int num_samples,
                        unsigned int offset,
                        unsigned int sample_rate,
                        unsigned int length_ms,
                        float frequency = 0.0f)
{
  //e^(-t) = 0.1 ? (99% decay) => -ln (0.1) = 2.30259 
  unsigned int samples_length = (0.001 * length_ms*sample_rate);
  assert(offset+samples_length <= num_samples);

  memset(values,0,num_samples * sizeof(float));
  float ratio = (float)sample_rate/(float)samples_length;
  
  for(int i = offset;i<samples_length+offset;i++){
    values[i] = 1.11* (std::exp((-2.30259f* (i-offset)* ratio / (sample_rate)))-0.1);
  }

}

int main(int argc, char const *argv[])
{
  float decay_values[48000];
  unsigned int samples = 48000;
  unsigned int sample_rate = 48000;
  unsigned int offset = 8000;
  unsigned int length_ms = 200;
  exponential_decay(decay_values,
    samples,
    offset,
    sample_rate,
    length_ms);
  for (int i = 0; i < samples; i++) {
    printf("%g\n",decay_values[i]);
  }
  return 0;
}