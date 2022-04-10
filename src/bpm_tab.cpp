#include "bpm_tab.h"
#include <stdio.h>

BpmTab::BpmTab(QWidget *parent)
    : QWidget(parent)
    , Processor(_client)
    , bpm_tab_ui(new Ui::BpmTab)
{
	bpm_tab_ui->setupUi(this);
}

BpmTab::~BpmTab() {
}

void BpmTab::setupJackClient() {
}

void BpmTab::process(int samples) {
}

void BpmTab::on_tab_button() {
}
