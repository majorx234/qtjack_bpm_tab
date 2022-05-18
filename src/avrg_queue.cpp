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

#include "avrg_queue.hpp"

AvrgQueue::AvrgQueue(unsigned int max_elements)
    : max_elements_(max_elements)
{
}

AvrgQueue::~AvrgQueue() {

}

double AvrgQueue::get_avrg() {
  if (!empty()) {
    double sum = 0;
    double average_value = values.front();
    for (size_t i = 1; i <= values.size(); ++i) {
      double elem = std::move(values.front());
      values.pop();
      sum += elem;
      values.push(std::move(elem));
      average_value = sum / static_cast<double>(i);
    }
    return average_value;
  } else
      return 0.0;
}

void AvrgQueue::add_value(double new_value) {
  if (values.size() < max_elements_) {
    values.push(new_value);
  } else {
    values.pop();
    values.push(new_value);
  }
}

bool AvrgQueue::empty() {
  return values.empty();
}

void AvrgQueue::clear() {
  while (!empty())
  {
    values.pop();
  }
}

void AvrgQueue::holdLast() {
  double tmp = get_avrg();
  clear();
  add_value(tmp);
}
