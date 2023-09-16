/**
 * Author: Hendrik van Arragon, 2022
 * SPDX-License-Identifier: MIT
 */

/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#ifndef MODEL_ADAPTER_TINY_H_
#define MODEL_ADAPTER_TINY_H_

#include <stdint.h>
#include <abmt/rt/model.h>
#include <abmt/rt/com.h>
#include <rt/fifo.h>

#include "s2p_uart.h"

struct daq_element{
	uint16_t signal_index;
	uint8_t daq_index;
};

template<typename T>
struct dyn_list {
	unsigned int length = 0;
	unsigned int pos = 0; // helper for push_back
	T* array = 0;

	dyn_list(){
		length = 0;
		array = 0;
	}

	dyn_list(dyn_list& c) = delete; // could mixup underling reference_counts


	void clear(){
		if(array != 0){
			delete[] array;
		}
		length = 0;
		pos = 0;
	}

	void push_back(T elem){
		if( pos < length){
			array[pos] = elem;
			pos++;
		}
	}

	unsigned int size(){
		return length;
	}

	void realloc(){
		if(length > 0){
			array = new T[length];
		}
	}

	T& operator[](unsigned int pos) const {
		if(pos > length -1){
			abmt::die("operator[] out of range");
		}
		return array[pos];
	}

	T* begin(){
		return array;
	}
	T* end(){
		return array + length;
	}

	~dyn_list(){
		clear();
	}

};

#define FIFO_SIZE 60
#define S2P_PKG_SIZE 11

//using daq_list_t = std::vector<daq_element>;
using daq_list_t = dyn_list<daq_element>;
using maa_fifo_t = fifo<FIFO_SIZE>;

class model_adatper_tiny{
public:

	abmt::rt::model* mdl = 0;
	daq_list_t* daq_lists = 0;
	dyn_list<uint16_t> paq_list;

	const uint16_t max_def_size = FIFO_SIZE - S2P_PKG_SIZE;
	uint16_t def_idx = 0;
	bool connected = false;
	maa_fifo_t blk;

	s2p_uart connection;

	model_adatper_tiny();

	void set_model(abmt::rt::model* m);

	virtual void on_hello(uint16_t version);

	virtual void on_request_signal_def();

	virtual void on_request_parameter_def();

	virtual void on_set_daq_list(abmt::blob& lst);

	virtual void on_set_paq_list(abmt::blob& lst);

	virtual void on_set_parameter(abmt::blob& data);

	virtual void on_command(std::string cmd);

	void send_model_online();

	void handle_data(uint32_t id, abmt::blob& blk);

	virtual void send(abmt::rt::cmd id, const void* data = 0, uint32_t size = 0);

	virtual void send(abmt::rt::cmd id, maa_fifo_t& blk);

	virtual void log(std::string msg);

	virtual void log_err(std::string msg);


	void save_parameters();
	void clear_daq_lists();

	virtual ~model_adatper_tiny();
	void send_daq(uint32_t raster);
	void send_daq_part(uint32_t raster, uint8_t start_idx, uint8_t len);

	void send_all_parameters();

};

extern model_adatper_tiny* adapter;

#endif /* MODEL_ADAPTER_AVR_H_ */
