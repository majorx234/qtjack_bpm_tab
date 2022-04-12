#include "avrg_queue.h"

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

