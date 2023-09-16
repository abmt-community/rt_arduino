/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#ifndef FIFO_H_
#define FIFO_H_

#include <abmt/blob.h>

template<unsigned int N>
class fifo: public abmt::blob{
public:
	char buffer[N];
	volatile uint8_t bytes_used;

	fifo(){
		data = buffer;
		size = N;
		bytes_used = 0;
	}

	void push_back(const void* src_data, uint32_t data_size){
		uint32_t bytes_to_copy = data_size;
		if(bytes_to_copy + bytes_used > size){
			bytes_to_copy = size - bytes_used;
		}
		memcpy(data+bytes_used, src_data, bytes_to_copy);
		bytes_used += bytes_to_copy;
	}

	void pop_front(uint32_t nbytes){
		if(nbytes != 0){
			if(bytes_used - nbytes == 0){
				bytes_used = 0;
				return;
			}
			memmove(data, data+nbytes, bytes_used - nbytes);
			bytes_used -= nbytes;
		}
	}

	uint8_t bytes_free(){
		return size - bytes_used;
	}

	void flush(){
		bytes_used = 0;
	}

};


#endif /* FIFO_H_ */
