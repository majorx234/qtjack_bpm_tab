///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of QtJack.                                            //
//    Copyright (C) 2014-2015 Jacob Dawid <jacob@omg-it.works>               //
//                                                                           //
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

#ifndef MAIN_WINDOW_TEST_HPP_
#define MAIN_WINDOW_TEST_HPP_

#include <QMainWindow>
#include "wave_widget.hpp"

class MainWindowTest : public QMainWindow
{
  Q_OBJECT
public:
  explicit MainWindowTest(QWidget *parent = 0);
  ~MainWindowTest();

private:
    int timerId;
    WaveWidget wave_widget;
protected:
  void timerEvent(QTimerEvent *event);
};

#endif // MAIN_WINDOW_TEST_HPP_
