#ifndef BPM_TAB_H_ 
#define BPM_TAB_H_ 

#include <chrono>

#include <QWidget>
#include "ui_bpm_tab.h"

// QtJack includes
#include <Client>
#include <Processor>
#include <RingBuffer>

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
protected slots:
    void on_tab_button();
private:
    Ui::BpmTab *bpm_tab_ui;    
    QtJack::Client _client;
    QtJack::MidiPort _midi_out;
    QtJack::MidiBuffer *_midi_out_buffer; //not used yet
    std::chrono::steady_clock::time_point last_timestamp;
    bool first_tab;
    unsigned int max_wait;
    unsigned int count;
    double bpm = 0;
};

#endif // BPM_TAB_H_