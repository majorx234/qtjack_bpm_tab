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

#ifndef WAVE_WIDGET_H_
#define WAVE_WIDGET_H_

#include <QWidget>

class WaveWidget : public QWidget
{
  Q_OBJECT

public:
  WaveWidget(QWidget* parent);
  ~WaveWidget();

public slots:
  void setChunk(double limit_1high,double limit_1low,double limit_2high,double limit_2low);
  void testChunk();

private:
  double *wave_[4];
  int wave_ofs_;
  int wavecounter_;
  int frame_rate;

protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent *ev);
};


#endif // WAVE_WIDGET_H_
