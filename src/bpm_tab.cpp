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

#include <chrono>
#include <stdio.h>
#include <chrono>
#include <QString>
#include <MidiMsg>
#include "bpm_tab.hpp"

BpmTab::BpmTab(QWidget *parent)
  : QWidget(parent)
  , Processor(_client)
  , bpm_tab_ui(new Ui::BpmTab)
  , first_tab(true)
  , started(false)
  , max_wait(5000)
  , bpm(120)
  , count(0)
  , avrg_queue(8)
  , alive(true)
  , _audio_buffer_size(48000*30) // 30seconds audiobuffer
  , _new_samples_in_audio_buffer(0)
{
  _audio_buffer =
      (QtJack::AudioSample*)malloc(_audio_buffer_size
                                   * sizeof(QtJack::AudioSample));
    bpm_tab_ui->setupUi(this);
    wave_widget = new WaveWidget(parent);
    QHBoxLayout *waveHbox = new QHBoxLayout(parent);
    waveHbox->addWidget(wave_widget);
    bpm_tab_ui->waveBox->setLayout(waveHbox);
    setupJackClient();
    connect(this, &BpmTab::setBpm,
            this->bpm_tab_ui->bpmLabel, &QLabel::setText);
    connect(this->bpm_tab_ui->tabButton, &QPushButton::clicked,
            this, &BpmTab::on_tab_button);
    connect(this, &BpmTab::trigger_midi_msg_send,
            this, &BpmTab::on_midi_message_send, Qt::QueuedConnection);
    //    connect(this, &BpmTab::jack_tick,wave_widget,&WaveWidget::getChunk);

    //thread to generate periodic midimsgs
    cyclic_midi_msgs_sender = std::thread(&BpmTab::midi_message_send,this);
}

BpmTab::~BpmTab() {
  alive = false;
  cyclic_midi_msgs_sender.join();
  delete _audio_buffer;
}

void BpmTab::setupJackClient() {
  _client.connectToServer("BpmTab");
  // midi port
  _midi_out = _client.registerMidiOutPort("out_1");

  // ToDo size should be dependend on samplerate
  _midi_out_buffer = QtJack::MidiMsgRingBuffer(5*48000);

  // audio port
  _audio_in_port = _client.registerAudioInPort("in");
  _audio_ring_buffer = QtJack::AudioRingBuffer();

  _client.setMainProcessor(this);
  _client.activate();
}

void BpmTab::midi_message_send() {
  //thread function
  while (alive) {
    if (started) {
      emit trigger_midi_msg_send(true);
    }
    int ms = 60000 / bpm;
    //ToDo: need syncing with conditional variable here + timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }
}

void BpmTab::on_midi_message_send(bool note_on_off) {
  int t1 = _client.getJackTime();
  int t2 = t1+(100*48000/1000);  //+20ms in samples
  QtJack::MidiMsg note_on,note_off;

  note_on.midiData[0] = 0x91;
  note_on.midiData[1] = 0x69;
  note_on.midiData[2] = 0x3f;
  note_on.length = 3;
  note_on.timestamp = t1;

  note_off.midiData[0] = 0x81;
  note_off.midiData[1] = 0x69;
  note_off.midiData[2] = 0x3f;
  note_off.length = 3;
  note_off.timestamp = t2;

  int space = _midi_out_buffer.numberOfElementsCanBeWritten();
  if (space > 2) {
    int written1 = _midi_out_buffer.write(&note_on, 1);
    int written2 = _midi_out_buffer.write(&note_off, 1);
    // ToDo check if fail
    // if(!written1)
  }
}

void BpmTab::process(int samples) {
  // midi part
  QtJack::MidiBuffer port_buffer = _midi_out.buffer(samples);
  // to do check if buffer exists

  port_buffer.clearEventBuffer();
  last_frame_time = _client.getJackTime();
  jack_nframes_t first_added_message = 0;
  bool looped = false;
  int elements = _midi_out_buffer.numberOfElementsAvailableForRead();
  while (elements && !looped) {
    QtJack::MidiMsg ev;
    int read = _midi_out_buffer.read(&ev, 1);
    if(read < 1) continue;
    if (first_added_message == ev.timestamp) {
      //first element wich was written back for future use
      looped = true;
      // need to write it back:
      int space = _midi_out_buffer.numberOfElementsCanBeWritten();
      if (space) {
        int written = _midi_out_buffer.write(&ev, 1);
      }
      break;
    }
    int t = ev.timestamp + samples - last_frame_time;
    if (t >= static_cast<int>(samples)) { // write back for future use
      if(!first_added_message)
        first_added_message = ev.timestamp;
      int space = _midi_out_buffer.numberOfElementsCanBeWritten();
      if (space) {
        int written = _midi_out_buffer.write(&ev, 1);
        // ToDo check if fail
        // if(!written)
      }
    } else {
      if (t < 0)
        t = 0; // maybe we missed a cirlce because of xrun
      QtJack::MidiData *mididata = port_buffer.reserveEvent(t, ev.length);
      if (mididata)
        memcpy(mididata, ev.midiData, ev.length);
      // check if midi msgs was sent
      // if(!midi_was_sent)
    }
    elements = _midi_out_buffer.numberOfElementsAvailableForRead();
  }
  // audio part
  //_audio_in_port.buffer(samples).push(_audio_ring_buffer);
}

void BpmTab::audio_process_fct() {
  int max_elemets = _audio_buffer_size;
  while(alive)
  {
    {
      std::unique_lock<std::mutex> lock(audio_mutex);
      // ToDo: timeout shoul be a class member
      int timeInMillisec = 1000;
      while(audio_chunk_cv.wait_for(
                 lock,std::chrono::milliseconds(timeInMillisec))
             == std::cv_status::timeout) {
         if(!alive) return;
         //do stuff
        int num_elements =
            _audio_ring_buffer.numberOfElementsAvailableForRead();
        //need to copy, not more than buffer size
        max_elemets =
            num_elements < _audio_buffer_size ?
            num_elements : _audio_buffer_size;
        _audio_ring_buffer.read(_audio_buffer, max_elemets);
      } //read in finished
    }
    // Process read data
  }
}

void BpmTab::on_tab_button() {
  printf("button tabed\n");
  std::chrono::steady_clock::time_point new_timestamp =
      std::chrono::steady_clock::now();
  if(first_tab) {
    count = 1;
    first_tab = false;
    printf("first\n");
  } else {
    unsigned int milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            new_timestamp - last_timestamp).count();
  printf("ms : %d\n",milliseconds );
  if (milliseconds > max_wait) {
    last_timestamp = new_timestamp;
    count = 1;
    avrg_queue.holdLast();
    first_tab = true;
    return;
  }
  avrg_queue.add_value(milliseconds);
  double avrg_milliseconds = avrg_queue.get_avrg();
  double bpm_avg = 60000 / avrg_milliseconds;
  bpm = int(bpm_avg * 100 + 0.5)/ 100.0;
  count++;
  emit setBpm(QString::number(bpm));
  if(!started)
    started = true;
  }
  last_timestamp = new_timestamp;
  return;
}
