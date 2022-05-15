#include <chrono>
#include <stdio.h>
#include <chrono>
#include <QString>
#include "bpm_tab.h"

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
{
    bpm_tab_ui->setupUi(this);
    wave_widget = new WaveWidget(parent);
    QHBoxLayout *waveHbox = new QHBoxLayout(parent);
    waveHbox->addWidget(wave_widget);
    bpm_tab_ui->waveBox->setLayout(waveHbox);
    setupJackClient();
    connect(this, SIGNAL(setBpm(QString)), this->bpm_tab_ui->bpmLabel, SLOT(setText(QString)));
    connect(this->bpm_tab_ui->tabButton, &QPushButton::clicked, this, &BpmTab::on_tab_button);
    connect(this, &BpmTab::trigger_midi_msg_send, this, &BpmTab::on_midi_message_send, Qt::QueuedConnection);
    //    connect(this, &BpmTab::jack_tick,wave_widget,&WaveWidget::getChunk);
    cyclic_midi_msgs_sender = std::thread(&BpmTab::midi_message_send,this);
}

BpmTab::~BpmTab() {
  alive = false;
  cyclic_midi_msgs_sender.join();
}

void BpmTab::setupJackClient() {
  _client.connectToServer("BpmTab");
  // midi port
  _midi_out = _client.registerMidiOutPort("out_1");
  _midi_out_buffer = new QtJack::MidiBuffer(); //not used yet

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
        std::thread send_node_off = std::thread([this]() {
          std::this_thread::sleep_for(std::chrono::milliseconds(20));
          emit trigger_midi_msg_send(false);
        });    
        send_node_off.detach();
    }
    int ms = 60000 / bpm;
    //ToDo: need syncing with conditional variable here + timeout
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));    
  }
}

void BpmTab::on_midi_message_send(bool note_on_off) {
  //ToDo: add midi message to ring buffer
  _value = 127;
  _timestamp = _client.getJackFrameTime();
  _note_on_off = note_on_off;
}

void BpmTab::process(int samples) {
  // midi part
  jack_nframes_t last_frame_time;
  QtJack::MidiBuffer port_buffer = _midi_out.buffer(samples);
  port_buffer.clearEventBuffer();
  last_frame_time = _client.getJackTime();
  unsigned int printvalue = _value;
  int timestamp  = _timestamp;

  int t = _timestamp + samples - last_frame_time;

  if ((t > 0) && (t < samples)) {

    //note_on [0x91,0x69,0x3f]
    //note_off [0x81,0x09,0x3f]
    
    unsigned char midiData[3];
    if (_note_on_off) {
      midiData[0] = 0x91;
      midiData[1] = 0x69;
      midiData[2] = 0x3f; // & printvalue;
    } else {
      midiData[0] = 0x81;
      midiData[1] = 0x69;
      midiData[2] = 0x3f; // & printvalue;
    }
    port_buffer.writeEvent(t, midiData, 3);
    emit jack_tick();
  }
  //audio part
  _audio_in_port.buffer(samples).push(_audio_ring_buffer);
}

void BpmTab::audio_process_fct() {
    
    while(alive)
    {
        std::unique_lock<std::mutex> lock(audio_mutex);
        // ToDo: timeout shoul be a class member 
        int timeInMillisec = 1000; 
        while(audio_chunk_cv.wait_for(lock,std::chrono::milliseconds(timeInMillisec))==std::cv_status::timeout){
            if(!alive) return;
            //do stuff
            int num_elements = _audio_ring_buffer.numberOfElementsAvailableForRead();
        }
    }    
}
void BpmTab::on_tab_button() {
    printf("button tabed\n");
    std::chrono::steady_clock::time_point new_timestamp = std::chrono::steady_clock::now();
    if(first_tab) {
        count = 1;
        first_tab = false;
        printf("first\n");
    } else {
        unsigned int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(new_timestamp - last_timestamp).count();
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
