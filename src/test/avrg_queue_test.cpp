#include "avrg_queue.h"
#include <cstdio>

int main(int argc, char *argv[]) {
  AvrgQueue test_queue(8);
  for (int i = 0; i < 10; i++) {
    test_queue.add_value(i*1.0);
    printf("avrg: %d %f\n",i, test_queue.get_avrg());
  }
}

