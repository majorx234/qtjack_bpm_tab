#include <cmath>
#include <cstring>
#include <cassert>
#include <random>
#include <iostream>
#include <chrono>
#include "beatthis/filterbank.hpp"
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
  for(int iter=0;iter<1000;iter++){
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
    float data7[samples];
    float data8[samples];
    float sum[samples];

    int freq1 = 100;
    int freq2 = 300;
    int freq3 = 500;
    int freq4 = 1100;
    int freq5 = 1700;
    int freq6 = 2900;
    int freq7 = 200;
    unsigned int length_ms[8] = {690,470,290,170,110,50,550,30};

    std::chrono::steady_clock::time_point time1_start_wave_creation = std::chrono::steady_clock::now();
    sine_wave(data1, freq1, 10, samples, 0, sample_rate);
    sine_wave(data2, freq2, 30, samples, 0, sample_rate);
    sine_wave(data3, freq3, 50, samples, 0, sample_rate);
    sine_wave(data4, freq4, 110, samples, 0, sample_rate);
    sine_wave(data5, freq5, 170, samples, 0, sample_rate);
    sine_wave(data6, freq6, 290, samples, 0, sample_rate);
    sine_wave(data7, freq7, 50, samples, 0, sample_rate);

    const int range_from  = -1000;
    const int range_to    = 1000;
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<int> distr(range_from, range_to);

    for(int i = 0;i<samples;i++) {
      data8[i] = (float)distr(generator)/1000.0;
    }

    std::chrono::steady_clock::time_point time2_start_enveolope_and_sum_wave = std::chrono::steady_clock::now();

    float* data_array[8] = {data1,data2,data3,data4,data5,data6,data7,data8};
    for(int i = 0;i<8;i++){
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

    for(int i = 0;i < 8;i++){
      for (int j = 0; j < samples; j++) {
        sum[j] += 0.125*data_array[i][j];
      }
    }

    std::chrono::steady_clock::time_point time3_start_filterbank = std::chrono::steady_clock::now();
    Filterbank filterbank(samples, sample_rate);
    float** filter_result = filterbank.filter_signal(sum,{0,200,400,800,1600,3200 },4096);

    std::chrono::steady_clock::time_point time4_start_envelope_extractor = std::chrono::steady_clock::now();
    EnvelopeExtractor envelope_extractor(samples,sample_rate);

    float** result = envelope_extractor.extract_envelope(filter_result,
      {0,200,400,800,1600,3200},
      4096,
      0.4);

    std::chrono::steady_clock::time_point time5_differectifier = std::chrono::steady_clock::now();
    differectifier(result,
                   samples,
                   6,
                   true);

    std::chrono::steady_clock::time_point time6_start_combfilter = std::chrono::steady_clock::now();
    Combfilter comb_filter(samples,sample_rate);

    float bpm = comb_filter.bpm_refinement(result,
      {0,200,400,800,1600,3200},
      4096,
      3);
/*
  for(int i = 0;i<6;i++){
    for(int j=0;j<samples;j++){
      printf("%f\n",result[i][j]);
    }
  }*/
    std::chrono::steady_clock::time_point time7_free = std::chrono::steady_clock::now();

    printf("bpm = %f \n",bpm);

    for (int i = 0;i<6;i++) {
      free(filter_result[i]);
    }
    free(filter_result);
    for(int i = 0;i<6;i++){
      free(result[i]);
    }
    free(result);
    std::chrono::steady_clock::time_point time8_finish = std::chrono::steady_clock::now();

    std::string time_points_str[8] = {
      "time1_start_wave_creation",
      "time2_start_enveolope_and_sum_wave",
      "time3_start_filterbank",
      "time4_start_envelope_extractor",
      "time5_differectifier",
      "time6_start_combfilter",
      "time7_free",
      "time8_finish"
      };
    std::chrono::steady_clock::time_point time_points[8] = {  
      time1_start_wave_creation,
      time2_start_enveolope_and_sum_wave,
      time3_start_filterbank,
      time4_start_envelope_extractor,
      time5_differectifier,
      time6_start_combfilter,
      time7_free,
      time8_finish
    };
    for(int i = 0;i<7;i++) {
      std::cout << "Time difference = " << time_points_str[i] << " " << std::chrono::duration_cast<std::chrono::microseconds>(time_points[i+1] - time_points[i]).count() <<std::endl;
    }
  }
  return 0;
}
