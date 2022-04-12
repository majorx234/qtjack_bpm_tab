#include <chrono>
#include <stdio.h>
#include <QString>
#include "bpm_tab.h"

BpmTab::BpmTab(QWidget *parent)
    : QWidget(parent)
    , Processor(_client)
    , bpm_tab_ui(new Ui::BpmTab)
    , first_tab(true)
    , max_wait(60000)
    , bpm(0)
    , count(0)
    , avrg_queue(8)  
{
    bpm_tab_ui->setupUi(this);
    connect(this, SIGNAL(setBpm(QString)), this->bpm_tab_ui->bpmLabel, SLOT(setText(QString)));
    connect(this->bpm_tab_ui->tabButton, &QPushButton::clicked, this, &BpmTab::on_tab_button);
}

BpmTab::~BpmTab() {
}

void BpmTab::setupJackClient() {
}

void BpmTab::process(int samples) {
}

void BpmTab::on_tab_button() {
    printf("button tabed\n");
    std::chrono::steady_clock::time_point new_timestamp = std::chrono::steady_clock::now();
    if(first_tab) {
        bpm = 0;
        count = 1;
        first_tab = false;
        printf("first\n");
    } else {
        unsigned int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(new_timestamp - last_timestamp).count();
        avrg_queue.add_value(milliseconds);
        printf("ms : %d\n",milliseconds );
        if (milliseconds > max_wait) {
            last_timestamp = new_timestamp;
            bpm = 0;
            count = 1;
            avrg_queue.holdLast();
            return;
        }
        double avrg_milliseconds = avrg_queue.get_avrg();
        double bpm_avg = 60000 / avrg_milliseconds;
        bpm = int(bpm_avg * 100 + 0.5)/ 100.0;
        count++;
        emit setBpm(QString::number(bpm));
    }
    last_timestamp = new_timestamp;
    return;
}
