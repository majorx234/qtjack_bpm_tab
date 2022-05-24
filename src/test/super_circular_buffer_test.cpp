#include "super_circular_buffer.hpp"

#include <stdio.h>

int main ()
{
  int* buffer;

  unsigned int buf_size = 4;

  SuperCircularBuffer<int> int_buf(4,1);

  for(int i=0;i<20;i++) {
    int_buf.add_chunk(&i);
    buffer = int_buf.access_data();
      printf("\n");
      printf("%d\n",buffer[0]);
      printf("%d\n",buffer[1]);
      printf("%d\n",buffer[2]);
      printf("%d\n",buffer[3]);
      printf("------\n");
  }
  return 0;
}