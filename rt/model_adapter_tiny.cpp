/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#include "model_adapter_tiny.h"

#include <abmt/version.h>
#include <abmt/os.h>
#include <abmt/blob.h>

using namespace std;
using namespace abmt::rt;

model_adatper_tiny* adapter;

inline void on_s2p_new_pack(size_t id, void* data, uint32_t size){
	abmt::blob blk(data,size);
	adapter->handle_data(id,blk);
}

model_adatper_tiny::model_adatper_tiny(){
	connection.on_new_pack = on_s2p_new_pack;
}

void model_adatper_tiny::set_model(abmt::rt::model* m){
	mdl = m;
	if(daq_lists != 0){
		delete[] daq_lists;
	}
	daq_lists = new daq_list_t[mdl->rasters.length];
}


void model_adatper_tiny::on_hello(uint16_t version){
	if(version != ABMT_VERSION){
		log_err("e:version");
	}
	connected = true;
	def_idx = 0;
};


void model_adatper_tiny::on_request_signal_def(){
	uint32_t siglal_def_size = 0;
	uint32_t signals_to_send = 0;
	uint32_t bytes_to_send = 0;
	for(uint32_t i = def_idx; i < mdl->signals.length; i++){
		auto s = mdl->signals[i];
		auto sz = s.type.get_def_size();
		siglal_def_size += sz;
		auto name_len = strlen(s.name);
		if(name_len > 30){
			name_len = 30;
		}
		siglal_def_size += name_len + 1; // 1 for len_byte
		if(siglal_def_size > max_def_size){
			break;
		}
		bytes_to_send = siglal_def_size;
		signals_to_send++;
	}

	// auto blk = abmt::blob_shared(bytes_to_send);
	blk.bytes_used = bytes_to_send;

	char* ptr = (char*)blk.data;
	uint32_t pos = 0;
	for(uint32_t i = def_idx; i <  def_idx + signals_to_send; i++){
		auto s = mdl->signals[i];
		auto name_len = strlen(s.name);
		if(name_len > 30){
			name_len = 30;
		}
		*(uint8_t*)ptr = (uint8_t)name_len;
		++ptr; ++pos;
		memcpy(ptr,s.name,name_len);
		pos += name_len;
		ptr += name_len;
		uint32_t def_size = s.type.get_def(ptr,blk.size - pos);
		pos += def_size;
		ptr += def_size;
	}
	def_idx += signals_to_send;
	send(abmt::rt::cmd::ack_signal_def,blk);
	if(bytes_to_send == 0){
		def_idx = 0; // reset counter
	}
};

void model_adatper_tiny::on_request_parameter_def(){
	uint32_t param_def_size = 0;
	uint32_t params_to_send = 0;
	uint32_t bytes_to_send = 0;
	//for(auto p: mdl->parameters){
	for(uint32_t i = def_idx; i < mdl->parameters.length; i++){
		auto& p = mdl->parameters[i];
		auto sz = p.parameter->get_type().get_def_size();
		param_def_size += sz;
		auto name_len = strlen(p.name);
		if(name_len > 30){
			name_len = 30;
		}
		param_def_size += name_len + 1; // 1 for len_byte
		if(param_def_size > max_def_size){
			break;
		}
		bytes_to_send = param_def_size;
		params_to_send++;
	}

	// auto blk = abmt::blob_shared(bytes_to_send);
	blk.bytes_used = bytes_to_send;

	char* ptr = (char*)blk.data;
	uint32_t pos = 0;
	for(uint32_t i = def_idx; i < def_idx + params_to_send ; i++){
		auto& p = mdl->parameters[i];
		auto name_len = strlen(p.name);
		if(name_len > 30){
			name_len = 30;
		}
		*(uint8_t*)ptr = (uint8_t)name_len;
		++ptr; ++pos;
		memcpy(ptr,p.name,name_len);
		pos += name_len;
		ptr += name_len;
		uint32_t def_size = p.parameter->get_type().get_def(ptr,blk.size - pos);
		pos += def_size;
		ptr += def_size;
	}
	def_idx += params_to_send;
	send(abmt::rt::cmd::ack_parameter_def,blk);
	if(bytes_to_send == 0){
		def_idx = 0; // reset counter
	}
};

void model_adatper_tiny::on_set_daq_list(abmt::blob& lst){
	for(uint32_t i = 0; i < mdl->rasters.length; ++i){
		daq_lists[i].clear();
	}

	auto num_elements = lst.size/sizeof(uint32_t);
	for(uint32_t i = 0; i < num_elements; ++i){
		auto idx = lst.get<uint32_t>(i*sizeof(uint32_t));
		if(idx < mdl->signals.length){
			auto& signal = mdl->signals[idx];
			daq_lists[signal.raster_index].length++;
		}
	}

	for(uint32_t i = 0; i < mdl->rasters.length; ++i){
		daq_lists[i].realloc();
	}

	for(uint32_t i = 0; i < num_elements; ++i){
		auto idx = lst.get<uint32_t>(i*sizeof(uint32_t));
		if(idx < mdl->signals.length){
			auto& signal = mdl->signals[idx];
			daq_lists[signal.raster_index].push_back({idx,i});
		}
	}

	send(abmt::rt::cmd::ack_set_daq_list);
};

void model_adatper_tiny::on_set_paq_list(abmt::blob& lst){
	paq_list.clear();
	auto num_elements = lst.size/sizeof(uint32_t);
	paq_list.length = num_elements;
	paq_list.realloc();
	for(uint32_t i = 0; i < num_elements; ++i){
		auto idx = lst.get<uint32_t>(i*sizeof(uint32_t));
		if(idx < mdl->parameters.length){
			paq_list.push_back(idx);
		}
	}
	send(abmt::rt::cmd::ack_set_paq_list);

	if(paq_list.size() > 0){
		send_all_parameters();
	}
};

void model_adatper_tiny::on_set_parameter(abmt::blob& data){
	if(data.size < 8){
		return;
	}
	auto idx = data.get<uint32_t>(0);
	//auto len = data.as<uint32_t>(4);
	if(mdl->parameters.length > idx){
		auto& param =  mdl->parameters[idx].parameter;

		// write data to parameter
		auto exp =param->get_type();
		abmt::blob data_blk = data.sub_blob(8);
		exp.deserialize(data_blk.data,data_blk.size);

		// send parameter
		uint32_t size = exp.get_size();
		
		//auto blk = abmt::blob_shared(size + sizeof(abmt::rt::paq_data_hdr));
		blk.bytes_used = size + sizeof(abmt::rt::paq_data_hdr);

		abmt::rt::paq_data_hdr hdr;
		hdr.size = size;
		auto data_area = blk.sub_blob(sizeof(hdr),size);
		exp.serialize(data_area.data,data_area.size);


		// find list_idx
		bool found = false;
		for(uint32_t i = 0; i < paq_list.size(); ++i){
			if(paq_list[i] == idx){
				hdr.list_idx = i;
				found = true;
				break;
			}
		}
		blk.set(hdr, 0);
		if(found){
			send(abmt::rt::cmd::paq_data,blk);
		}
		save_parameters();
	}else{
		abmt::log_err("e:idx");
	}
};

void model_adatper_tiny::on_command(string cmd){
	if(cmd == "quit"){
		connected = false;
		clear_daq_lists();
		//on_quit();
	}
}

void model_adatper_tiny::send_model_online(){
		static uint32_t cnt = 0;
		send(abmt::rt::cmd::model_online,&cnt,sizeof(cnt));
		cnt++;
	}

void model_adatper_tiny::handle_data(uint32_t id, abmt::blob& blk){
	cmd c = (cmd)id;
	switch(c){
	case cmd::hello:
		on_hello(blk.get<uint16_t>(0));
		break;
	case cmd::request_signal_def:
		on_request_signal_def();
		break;
	case cmd::request_parameter_def:
		on_request_parameter_def();
		break;
	case cmd::set_daq_list:
		on_set_daq_list(blk);
		break;
	case cmd::set_paq_list:
		on_set_paq_list(blk);
		break;
	case cmd::set_prameter:
		on_set_parameter(blk);
		break;
	case cmd::command:
		on_command(blk.str());
		break;
	default:
		return;
	}
}

void model_adatper_tiny::send(cmd id, const void* data, uint32_t size){
	// auto lock = send_mtx.get_scope_lock();
	connection.send((uint8_t)id,data,size);
}

void model_adatper_tiny::send(cmd id, maa_fifo_t& blk){
	// auto lock = send_mtx.get_scope_lock();
	connection.send((uint8_t)id,blk.data, blk.bytes_used);
}

void model_adatper_tiny::log(std::string msg){
	uint32_t len = msg.length();
	if(len > 100){
		len = 100;
	}
	send(cmd::log,msg.c_str(),len);
}

void model_adatper_tiny::log_err(std::string msg){
	uint32_t len = msg.length();
	if(len > 100){
		len = 100;
	}
	send(cmd::log_err,msg.c_str(),len);
}


void model_adatper_tiny::save_parameters(){
	//on_save_parameters();
}

void model_adatper_tiny::clear_daq_lists(){
	for(uint32_t i = 0; i < mdl->rasters.length; ++i){
		daq_lists[i].clear();
	}
}

void model_adatper_tiny::send_daq(uint32_t raster){
	auto& daq_list = daq_lists[raster];
	if(daq_list.size() == 0){
		return;
	}
	uint8_t start_idx = 0;
	uint8_t n_sigs_to_send = 0;

	do{
		start_idx += n_sigs_to_send;
		n_sigs_to_send = 0;
		uint16_t bytes_to_send = 0;
		for(uint8_t i = start_idx; i < daq_list.size(); i++){
			auto& d = daq_list[i];
			auto sig_size = sizeof(abmt::rt::daq_data_hdr) + mdl->signals[d.signal_index].type.get_size();
			if(sig_size > blk.size){
				abmt::die("sig to big");
			}
			if(bytes_to_send + sig_size < blk.size ){
				n_sigs_to_send++;
				bytes_to_send += sig_size;
			}else{
				break;
			}
		}
		if(n_sigs_to_send > 0){
			send_daq_part(raster, start_idx, n_sigs_to_send);
		}
	} while(n_sigs_to_send != 0);
	
}

void model_adatper_tiny::send_daq_part(uint32_t raster, uint8_t start_idx, uint8_t len){
	auto now = abmt::time::now();
	auto& daq_list = daq_lists[raster];
	
	//auto blk = abmt::blob_shared(size_to_send);
	blk.bytes_used = blk.size;

	uint32_t mem_pos = 0;
	for(uint8_t i = start_idx; i < start_idx + len; i++){
		auto& d = daq_list[i];
		uint32_t signal_size =  mdl->signals[d.signal_index].type.get_size();

		abmt::rt::daq_data_hdr hdr = {d.daq_index, signal_size, now.ns_since_1970};
		blk.set(hdr, mem_pos);
		mem_pos += sizeof(hdr);

		auto data_area = blk.sub_blob(mem_pos,signal_size);
		mdl->signals[d.signal_index].type.serialize(data_area.data,data_area.size);
		mem_pos += signal_size;
	}

	blk.bytes_used = mem_pos;


	send(abmt::rt::cmd::daq_data,blk);
}

void model_adatper_tiny::send_all_parameters(){
	uint32_t size_to_send = 0;
	for(auto& idx: paq_list){
		auto param = mdl->parameters[idx].parameter;
		size_to_send += sizeof(abmt::rt::paq_data_hdr) + param->get_type().get_size();
	}


	//auto blk = abmt::blob_shared(size_to_send);
	blk.bytes_used = size_to_send;

	uint32_t mem_pos = 0;

	uint32_t entry_num = 0;
	for(auto& idx: paq_list){
		auto param = mdl->parameters[idx].parameter;

		auto exp = mdl->parameters[idx].parameter->get_type();
		uint32_t size = exp.get_size();

		abmt::rt::paq_data_hdr hdr = {entry_num, size};
		blk.set(hdr, mem_pos);
		mem_pos += sizeof(hdr);

		auto data_area = blk.sub_blob(mem_pos,size);
		exp.serialize(data_area.data,data_area.size);
		mem_pos += size;
	}

	send(abmt::rt::cmd::paq_data,blk);
}

model_adatper_tiny::~model_adatper_tiny(){
	if(daq_lists != 0){
		delete[] daq_lists;
	}
};
