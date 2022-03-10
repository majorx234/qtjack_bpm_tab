///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of QtJack.                                            //
//    Copyright (C) 2014-2015 Jacob Dawid <jacob@omg-it.works>               //
//                  2022 <majorx234@googlemail.com>                          //
//    QtJack is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by   //
//    the Free Software Foundation, either version 3 of the License, or      //
//    (at your option) any later version.                                    //
//                                                                           //
//    QtJack is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//    GNU General Public License for more details.                           //
//                                                                           //
//    You should have received a copy of the GNU General Public License      //
//    along with QtJack. If not, see <http://www.gnu.org/licenses/>.          //
//                                                                           //
//    It is possible to obtain a closed-source license of QtJack.             //
//    If you're interested, contact me at: jacob@omg-it.works                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef MIDIDIAL_MAINWINDOW_H
#define MIDIDIAL_MAINWINDOW_H

// QtJack includes
#include <Client>
#include <Processor>
#include <RingBuffer>

// Qt includes
#include <QMainWindow>
#include "ui_mididial.h"

namespace Ui {
class Mididial;
}

class MidiDialMainWindow : public QMainWindow, public QtJack::Processor
{
    Q_OBJECT

public:
    explicit MidiDialMainWindow(QWidget *parent = 0);
    ~MidiDialMainWindow();

    void setupJackClient();

protected slots:
    void on_twist();

private:
    Ui::Mididial *mididial_ui;

    QtJack::Client _client;
    QAbstractSlider *slider;
};

#endif // MIDIDIAL_MAINWINDOW_H
