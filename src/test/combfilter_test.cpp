#include <cmath>
#include <cstring>
#include <cassert>
#include "beatthis/envelope_extractor.hpp"
#include "beatthis/differectifier.hpp"
#include "beatthis/combfilter.hpp"

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
  
  //create periodic signal
  bool create_decay = frequency>1.0f;
  unsigned int period_length_proto = static_cast<unsigned int>((float)sample_rate/frequency);
  unsigned int period_length = create_decay ? period_length_proto : num_samples;
  do {
    for(int i = offset;i<samples_length+offset;i++){
      values[i] += 1.11* (std::exp((-2.30259f* (i-offset)* ratio / (sample_rate)))-0.1);
    }
    offset += period_length;
    if((offset+samples_length)>num_samples)
      create_decay = false;
  }while(create_decay);
}

int main(int argc, char const *argv[])
{
  float decay_values[192000];
  unsigned int samples = 192000;
  unsigned int sample_rate = 48000;
  unsigned int offset = 8000;

  float data1[samples];
  float data2[samples];
  float data3[samples];
  float data4[samples];
  float data5[samples];
  float data6[samples];
  float sum[samples];

  int freq1 = 100;
  int freq2 = 300;
  int freq3 = 500;
  int freq4 = 1100;
  int freq5 = 1700;
  int freq6 = 2900;
  unsigned int length_ms[6] = {690,470,290,170,110,50};

  sine_wave(data1, freq1, 1, samples, 0, sample_rate);
  sine_wave(data2, freq2, 1, samples, 0, sample_rate);
  sine_wave(data3, freq3, 1, samples, 0, sample_rate);
  sine_wave(data4, freq4, 1, samples, 0, sample_rate);
  sine_wave(data5, freq5, 1, samples, 0, sample_rate);
  sine_wave(data6, freq6, 1, samples, 0, sample_rate);

  float* data_array[6] = {data1,data2,data3,data4,data5,data6};
  for(int i = 0;i<6;i++){
    exponential_decay(decay_values,
      samples,
      offset,
      sample_rate,
      length_ms[i],
      2.0f);
    for(int j=0;j<samples;j++){
      data_array[i][j] = data_array[i][j] * decay_values[j];
    }
  }

  std::vector<unsigned int> bandlimits = {0,200,400,800,1600,3200};
  unsigned int max_freq = 4096;
  unsigned int nbands = bandlimits.size();

  EnvelopeExtractor envelope_extractor(samples,sample_rate);

  float** result = envelope_extractor.extract_envelope(data_array,
    bandlimits,
    max_freq,
    0.4);

  differectifier(result,
                 samples,
                 6,
                 true);

  Combfilter comb_filter(samples, sample_rate, nbands);

  float bpm = comb_filter.bpm_refinement(result,
    bandlimits,
    max_freq,
    3);
/*
  for(int i = 0;i<6;i++){
    for(int j=0;j<samples;j++){
      printf("%f\n",result[i][j]);
    }
  }*/
  printf("bpm = %f \n",bpm);

  for(int i = 0;i<6;i++){
    free(result[i]);
  }
  free(result);
  return 0;
}