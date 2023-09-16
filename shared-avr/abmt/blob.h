/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 * 
 * 2023: Modified for avr (remved shared_blob)
 */

#ifndef SHARED_ABMT_BLOB_MBU_H_
#define SHARED_ABMT_BLOB_MBU_H_

#include <string>
#include <memory>
#include <abmt/os.h>
#include <abmt/const_array.h>

namespace abmt{

/**
 * Helper for conversion form a block of memory(datapointer + size) to another type.
 */

class blob;

class blob{
public:
	char* data;
	uint32_t size;

	blob(){
		data = 0;
		size = 0;
	}

	blob(void* ptr, uint32_t s):data((char*)ptr),size(s){

	}

	// when len = 0 then the subblock contains the rest of the block;
	blob sub_blob(uint32_t offset, uint32_t len = 0){
		if(len == 0){
			if(offset > size){
				abmt::die("e:offset");
			}
			len = size-offset;
		}
		if(offset+len > size){
			abmt::die("e:size");
		}
		return blob(((char*)data + offset), len);
	}

	void set_mem(const void* src, uint32_t len, uint32_t offset){
		if(len+offset > size){
			abmt::die("e:size2");
		}
		memcpy((char*) data + offset,src,len);
	}

	void get_mem(void* dst, uint32_t len, uint32_t offset){
		if(len+offset > size){
			abmt::die("e:size3");
		}
		memcpy(dst,(char*) data + offset,len);
	}

	template<typename T>
	inline T get(uint32_t offset){
		if(sizeof(T) + offset > size) abmt::die("e");
		T res;
		// it is done that way because of the arm architecture and it's stupid alingment rules
		memcpy(&res,(data + offset),sizeof(T));
		return res;
	}

	template<typename T>
	inline void set(T&& obj, uint32_t offset){
		if(sizeof(T) + offset > size) abmt::die("e");
		memcpy(data + offset, &obj, sizeof(T));
	}

	/// Like get without size check.
	template<typename T>
	inline T get2(uint32_t offset){
		T res;
		// it is done that way because of the arm architecture and it's stupid alingment rules
		memcpy(&res, data + offset, sizeof(T));
		return res;
	}

	/// Like set without size check.
	template<typename T>
	inline void set2(T&& data, uint32_t offset){
		memcpy(data + offset, &data, sizeof(T));
	}

	std::string str(uint32_t offset = 0, uint32_t len = 0){
		if(len == 0){
			if(offset > size){
				abmt::die("e:size4");
			}
			len = size-offset;
		}
		if(offset+len > size){
			abmt::die("e:size5");
		}
		return std::string((char*)data +offset , len);
	}

	/// Compares only the pointer and the size. Not the content!
	bool operator==(blob& rhs){
		if(data == rhs.data && size == rhs.size){
			return true;
		}
		return false;
	}

	operator std::string(){
		return str();
	}

	virtual ~blob(){

	}
};

} // namespace abmt

#endif /* _MBU_H_ */
