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

#ifndef AVRG_QUEUE_HPP_
#define AVRG_QUEUE_HPP_

#include <queue>

class AvrgQueue {
 public:
  AvrgQueue(unsigned int max_elements);
  ~AvrgQueue();

  double get_avrg();
  void add_value(double new_value);
  bool empty();
  void clear();
  void setMax(unsigned int new_max);
  void holdLast();
 private:
  std::queue<double> values;
  unsigned int max_elements_;
};

#endif // AVRG_QUEUE_HPP_
