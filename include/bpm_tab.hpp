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

#ifndef BPM_TAB_HPP_
#define BPM_TAB_HPP_

#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>

#include <QWidget>
#include "ui_bpm_tab.h"

// QtJack includes
#include <Client>
#include <Processor>
#include <RingBuffer>
#include <MidiMsg>

#include "avrg_queue.hpp"
#include "wave_widget.hpp"

namespace Ui {
class BpmTab;
}

class BpmTab : public QWidget, public QtJack::Processor {
  Q_OBJECT
 public:
  explicit BpmTab(QWidget *parent = 0);
  ~BpmTab();

  void setupJackClient();
  void process(int samples) override;
  void audio_process_fct();
 signals:
  void setBpm(QString bpm_string);
  void trigger_midi_msg_send(bool);
  void jack_tick();
 protected slots:
  void on_tab_button();
  void on_midi_message_send(bool);
 private:
  void midi_message_send();
  Ui::BpmTab *bpm_tab_ui;
  WaveWidget* wave_widget;
  QtJack::Client _client;
  QtJack::MidiPort _midi_out;
  QtJack::MidiMsgRingBuffer _midi_out_buffer;
  jack_nframes_t last_frame_time;
  QtJack::AudioPort _audio_in_port;
  QtJack::AudioRingBuffer _audio_ring_buffer;
  size_t _audio_buffer_size;
  QtJack::AudioSample* _audio_buffer;
  size_t _new_samples_in_audio_buffer;

  // midimessages
  std::atomic<int> _timestamp;
  std::atomic<bool> _note_on_off;
  std::atomic<unsigned int> _value;

  std::chrono::steady_clock::time_point last_timestamp;
  bool first_tab;
  bool started;
  unsigned int max_wait;
  unsigned int count;
  double bpm = 0;
  AvrgQueue avrg_queue;
  std::thread cyclic_midi_msgs_sender;
  std::thread audio_processing;
  std::atomic_int alive;
  std::condition_variable audio_chunk_cv;
  std::mutex audio_mutex;
};

#endif // BPM_TAB_HPP_
