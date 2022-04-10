#ifndef BPM_TAB_H_ 
#define BPM_TAB_H_ 

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

protected slots:
    void on_tab_button();
private:
    Ui::BpmTab *bpm_tab_ui;    
    QtJack::Client _client;
    QtJack::MidiPort _midi_out;
    QtJack::MidiBuffer *_midi_out_buffer; //not used yet
};

#endif // BPM_TAB_H_