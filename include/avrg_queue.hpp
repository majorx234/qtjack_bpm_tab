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
