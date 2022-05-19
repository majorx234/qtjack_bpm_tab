///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//    This file is part of qtjack_bpm_tab                                    //
//    Copyright (C) 2022 <majorx234@googlemail.com>                          //
//                                                                           //
//    qtjack_bpm_tab is free software: you can redistribute it and/or modify //
//    it under the terms of the GNU General Public License as published by   //
//    the Free Software Foundation, either version 3 of the License, or      //
//    (at your option) any later version.                                    //
//                                                                           //
//    qtjack_bpm_tab is distributed in the hope that it will be useful,      //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of         //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          //
//    GNU General Public License for more details.                           //
//                                                                           //
//    You should have received a copy of the GNU General Public License      //
//    along with qtjack_bpm_tab. If not, see <http://www.gnu.org/licenses/>. //
//                                                                           //
//    It is possible to obtain a closed-source license of QtJack.            //
//    If you're interested, contact me at: majorx234@gmail.com               //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef WAVE_WIDGET_HPP_
#define WAVE_WIDGET_HPP_

#include <QWidget>

class WaveWidget : public QWidget
{
  Q_OBJECT

 public:
  WaveWidget(QWidget* parent);
  ~WaveWidget();

 public slots:
  void setChunk(double limit_1high,
                double limit_1low,
                double limit_2high,
                double limit_2low);
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


#endif // WAVE_WIDGET_HPP_
