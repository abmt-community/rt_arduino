/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#ifndef RUNTIME_INTERFACE_S2P_UART_H_
#define RUNTIME_INTERFACE_S2P_UART_H_

#include <abmt/blob.h>
#include "uart_com.h"

inline void default_s2p_new_pack_cb(size_t id, void* data, uint32_t size){

}

class s2p_uart{

public:
	enum {search_start,check_for_header,wait_for_pack} read_state = search_start;
	size_t current_id = 0;
	uint32_t current_pack_size = 0;

	void (*on_new_pack)(size_t id, void* data, uint32_t size);

	s2p_uart(){
		on_new_pack = default_s2p_new_pack_cb;
	}

	uint32_t handle_data(abmt::blob& buffer){
		if(buffer.data == 0){
			return buffer.size;
		}

		const size_t header_size = 8;
		const size_t crc_size = 2;

		uint32_t bytes_availible = buffer.size;
		uint32_t bytes_to_pop = 0;

		auto abort = [&]{
			++bytes_to_pop;
			read_state = search_start;
		};


		while( bytes_to_pop < buffer.size ){
			if(read_state == search_start){
				while(bytes_to_pop < buffer.size){
					if(buffer.data[bytes_to_pop] == 'U'){
						read_state = check_for_header;
						break;
					}
					++bytes_to_pop;
				}

			}

			bytes_availible = buffer.size - bytes_to_pop;
			if(read_state == check_for_header){
				if(bytes_availible >= header_size){
					uint8_t header_check = buffer.get<uint8_t>(bytes_to_pop + 0) ^ buffer.get<uint8_t>(bytes_to_pop + 1) ^ buffer.get<uint8_t>(bytes_to_pop + 2) ^ buffer.get<uint8_t>( bytes_to_pop +3);
					header_check        ^= buffer.get<uint8_t>(bytes_to_pop + 4) ^ buffer.get<uint8_t>(bytes_to_pop + 5) ^ buffer.get<uint8_t>(bytes_to_pop + 6);
					bool type_ok = buffer.get<uint8_t>(bytes_to_pop + 1) == 'A';
					bool hdr_chk_ok = buffer.get<uint8_t>(bytes_to_pop + 7) == header_check;
					if( type_ok && hdr_chk_ok ){
						current_pack_size = buffer.get<uint32_t>(bytes_to_pop + 3);
						current_id = buffer.get<uint8_t>(bytes_to_pop + 2);
						read_state = wait_for_pack;
					}else{
						abort();
					}
				}else{
					break;
				}
			}

			if(read_state == wait_for_pack){
				if(bytes_availible >= header_size + crc_size + current_pack_size){
					uint16_t crc = make_checksum(&buffer.data[bytes_to_pop + header_size], current_pack_size);
					uint16_t ctc_tmp = crc;
					uint16_t ctc_pack = buffer.get<uint16_t>(bytes_to_pop + header_size + current_pack_size );
					if(ctc_tmp == ctc_pack){
						on_new_pack(current_id,&buffer.data[bytes_to_pop + header_size],current_pack_size);
						bytes_to_pop += header_size + current_pack_size + crc_size;
						read_state = search_start;
					}else{
						abort();
					}
				}else{
					break;
				}
			}

		} // while

		return bytes_to_pop;
	}

	void check_new_data(){
		abmt::blob blk = {uart_in_buffer.data, uart_in_buffer.bytes_used };
		auto bytes_to_pop = handle_data(blk);
		uart_in_buffer.pop_front(bytes_to_pop);
	}

	/**
	 * Types:
	 * A: 32-Bit size
	 */
	void send(uint8_t id, const void* data, uint32_t size){
		static uint8_t seed = 0;

		char header[8]; // start + type + id + size + header_check
		header[0] = 'U';
		header[1] = 'A';
		header[2] = id;
		uint8_t header_check = header[0] ^ header[1] ^ header[2];
		char* size_as_char = (char*)&size;
		for(unsigned int i = 0; i < sizeof(size); ++i){
			header[3 + i] = size_as_char[i];
			header_check ^= size_as_char[i];
		}
		header[7] = header_check;
		uart_write(header,8);

		// Data
		uart_write(data,size);
		uint16_t crc = make_checksum(data, size);
		uart_write(&crc,2);
	}

	uint16_t make_checksum(const void* data_ptr, uint32_t len) {
		// Bis revision 508 waren hier noch weitere Checksummen zum Testen auskommentiert
		uint32_t sum1 = 0;
		uint32_t sum2 = 0;
		const uint8_t* data = (const uint8_t*)data_ptr;

		for( unsigned int index = 0; index < len;  ) {
			uint32_t blocklen = 5802;
			if(blocklen > len-index){
				blocklen = len-index;
			}
			for( unsigned j = 0; j < blocklen; ++j ) {
				sum1 = (sum1 + data[index+j]);
				sum2 = (sum2 + sum1);
			}
			index += blocklen;
			sum1 = sum1 % 255;
			sum2 = sum2 % 255;

		}
		return (sum2 << 8) | sum1;
	}
};

#endif /* RUNTIME_INTERFACE_s2p_uart_H_ */
