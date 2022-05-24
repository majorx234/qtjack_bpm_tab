#ifndef SUPER_CICULAR_BUFFER_HPP_
#define SUPER_CICULAR_BUFFER_HPP_

#include <cstring>
#include <stdlib.h>

#include <stdio.h> // printf

template <class Type>
class SuperCircularBuffer
{
public:
  SuperCircularBuffer(unsigned int max_chunks, unsigned int chunk_size) 
  :  _chunks(0)
  ,  _max_chunks(2*max_chunks)
  , _chunk_size(chunk_size)
  , _data(nullptr)
  {
    _data = (Type*)std::malloc(2 * _max_chunks * chunk_size * sizeof(Type));
    std::memset(_data, 0, 2 * _max_chunks * _chunk_size * sizeof(Type));

  };
  ~SuperCircularBuffer() {
    if(_data)
      delete _data;
  };
  void add_chunk(Type* data) {
    if(_chunks < _max_chunks/2){
      size_t offset = (_chunks%_max_chunks) * _chunk_size;
      std::memcpy(_data + offset, data, _chunk_size);
    }
    else{
      size_t offset1 = (_chunks%(_max_chunks/2)) * _chunk_size;

      size_t offset2 = (_chunks%(_max_chunks/2) + (_max_chunks/2)) 
                       * _chunk_size;
      std::memcpy(_data + offset1,data, _chunk_size);
      std::memcpy(_data + offset2,data, _chunk_size);                 
    }
    _chunks++;
  };
  Type* access_data(){
    size_t offset1 = 0;
    if(_chunks >= _max_chunks/2)
      offset1 = (_chunks%(_max_chunks/2)) * _chunk_size;
    return _data + offset1;
  };
private:
  unsigned int _chunks;
  unsigned int _max_chunks;
  unsigned int _chunk_size;
  Type* _data;
};

#endif // SUPER_CICULAR_BUFFER_HPP_