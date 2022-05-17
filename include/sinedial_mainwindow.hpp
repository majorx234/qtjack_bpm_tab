///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of QtJack.                                           //
//    Copyright (C) 2014-2015 Jacob Dawid <jacob@omg-it.works>               //
//                  2022 <majorx234@googlemail.com>                          //
//    QtJack is free software: you can redistribute it and/or modify         //
//    it under the terms of the GNU General Public License as published by   //
//    the Free Software Foundation, either version 3 of the License, or      //
//    (at your option) any later version.                                    //
//                                                                           //
//    QtJack is distributed in the hope that it will be useful,              //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//    GNU General Public License for more details.                           //
//                                                                           //
//    You should have received a copy of the GNU General Public License      //
//    along with QtJack. If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//    It is possible to obtain a closed-source license of QtJack.            //
//    If you're interested, contact me at: jacob@omg-it.works                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef SINEDIAL_MAINWINDOW_HPP_
#define SINEDIAL_MAINWINDOW_HPP_

#include <atomic>
#include <thread>

// QtJack includes
#include <Client>
#include <Processor>
#include <RingBuffer>

// Qt includes
#include <QMainWindow>
#include "ui_sinedial.h"

namespace Ui {
class Sinedial;
}

class SineDialMainWindow : public QMainWindow, public QtJack::Processor
{
    Q_OBJECT

public:
    explicit SineDialMainWindow(QWidget *parent = 0);
    ~SineDialMainWindow();

    void setupJackClient();
    void process(int samples) override;
    void sine_wave_generate_function();
protected slots:
    void on_twist();

private:
    Ui::Sinedial *sinedial_ui;

    QtJack::Client _client;
    QtJack::AudioPort _audio_out[2];
    QtJack::AudioRingBuffer *_audio_ring_buffer[2];
    QAbstractSlider *slider_base_freq;
    QAbstractSlider *slider_mod_freq;
    std::atomic<unsigned int> _value_base_freq;
    std::atomic<unsigned int> _value_mod_freq;
    std::atomic<int> _timestamp;
    unsigned int sample_rate;

    std::thread sine_wave_generate_thread;
    unsigned int generated_samples_per_tick;
    unsigned int samples_offset;
    QtJack::AudioSample* left;
    QtJack::AudioSample* right;
};

#endif // SINEDIAL_MAINWINDOW_HPP_
