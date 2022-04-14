#ifndef BPM_TAB_H_ 
#define BPM_TAB_H_ 

#include <chrono>
#include <thread>
#include <atomic>

#include <QWidget>
#include "ui_bpm_tab.h"

// QtJack includes
#include <Client>
#include <Processor>
#include <RingBuffer>

#include "avrg_queue.h"

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
signals:
    void setBpm(QString bpm_string);
  void trigger_midi_msg_send(bool);
protected slots:
    void on_tab_button();
    void on_midi_message_send(bool);
private:
    void midi_message_send();
    Ui::BpmTab *bpm_tab_ui;    
    QtJack::Client _client;
    QtJack::MidiPort _midi_out;
    QtJack::MidiBuffer *_midi_out_buffer; //not used yet
 
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
    std::atomic_int alive;
};

#endif // BPM_TAB_H_
