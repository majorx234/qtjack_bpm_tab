#include <chrono>
#include <stdio.h>
#include <QString>
#include "bpm_tab.h"

BpmTab::BpmTab(QWidget *parent)
    : QWidget(parent)
    , Processor(_client)
    , bpm_tab_ui(new Ui::BpmTab)
    , first_tab(true)
    , max_wait(5000)
    , bpm(0)
    , count(0)
    , avrg_queue(8)  
{
    bpm_tab_ui->setupUi(this);
    setupJackClient();
    connect(this, SIGNAL(setBpm(QString)), this->bpm_tab_ui->bpmLabel, SLOT(setText(QString)));
    connect(this->bpm_tab_ui->tabButton, &QPushButton::clicked, this, &BpmTab::on_tab_button);
    connect(this->bpm_tab_ui->tabButton, &QPushButton::clicked, this, &BpmTab::on_midi_message_send);
    //cyclic_midi_msgs_sender = 
}

BpmTab::~BpmTab() {
}

void BpmTab::setupJackClient() {
  _client.connectToServer("BpmTab");
  _midi_out = _client.registerMidiOutPort("out_1");
  _midi_out_buffer = new QtJack::MidiBuffer(); //not used yet
  _client.setMainProcessor(this);
  _client.activate();
}

void BpmTab::midi_message_send() {
  //thread function
}
void BpmTab::on_midi_message_send() {
  _value = 127;
  _timestamp_note_on = _client.getJackFrameTime();
  double samplerate = _client.getJackSampleRate();
  //Jack_nframes_t nframes = _timestamp_note_on;
  //double ms = (nframes * 1000.0) / sameplerate;
  double offset = 100 *samplerate /1000.0;
  _timestamp_note_off = _timestamp_note_on + static_cast<int>(offset);
}

void BpmTab::process(int samples) {
  jack_nframes_t last_frame_time;
  QtJack::MidiBuffer port_buffer = _midi_out.buffer(samples);
  port_buffer.clearEventBuffer();
  last_frame_time = _client.getJackTime();
  unsigned int printvalue = _value;
  int timestamp_note_on  = _timestamp_note_on;
  int timestamp_note_off = _timestamp_note_off;

  int t_on = timestamp_note_on + samples - last_frame_time;
  int t_off = timestamp_note_off + samples - last_frame_time;
  if ((t_on > 0) && (t_on < samples)) {

    //note_on [0x91,0x69,0x3f]
    //note_off [0x81,0x09,0x3f]
    
    unsigned char midiData_on[3];
    midiData_on[0] = 0x91;
    midiData_on[1] = 0x69;
    midiData_on[2] = 0x3f; // & printvalue;
    port_buffer.writeEvent(t_on, midiData_on, 3);

    unsigned char midiData_off[3];
    midiData_off[0] = 0xb1;
    midiData_off[1] = 10;
    midiData_off[2] = 0x3f; // & printvalue;
    port_buffer.writeEvent(t_off, midiData_off, 3);
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
    }
    last_timestamp = new_timestamp;
    return;
}
