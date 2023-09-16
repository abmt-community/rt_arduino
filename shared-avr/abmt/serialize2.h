/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#ifndef UTIL_EXAMPLES_SERIALIZE2_H_
#define UTIL_EXAMPLES_SERIALIZE2_H_

#include <cstdint>
#include <cstddef>
#include <string>
#include <abmt/blob.h>

// I'm not proud of this!
#define ABMT_DEF_MEMBER(X) {#X, &this->X, this}
#define ABMT_DEF_PTR_MEMBER(P, X) {#X, &P->X, P}

namespace abmt{
namespace serialize{
class type;

template<class C> type define_type(C* ptr = 0);

class member;

struct member_list{
	size_t length;
	const member* list;

	member_list() = default; // @suppress("Class members should be properly initialized")

	template<int N>
	member_list(const member(&ptr)[N] ){
		length = N;
		list = ptr;
	}

	const member* begin();
	const member* end();
};

struct blob_type{
	size_t (*get_size)    (void* obj);
	void   (*obj_to_data) (void* obj, void* data, size_t size);
	void   (*data_to_obj) (void* obj, void* data, size_t size);
};

struct string_type: public blob_type{

};

struct scene_2d_type: public blob_type{
	scene_2d_type(blob_type r);
};

struct scene_3d_type: public blob_type{
	scene_3d_type(blob_type r);
};

template <typename T>
class obj_has_define_type
{
    typedef char one[1];
    typedef char two[2];

    template <typename C> static one& test( decltype(&C::define_type) ) ;
    template <typename C> static two& test(...);

public:
    enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

template <typename T>
class obj_serializes_to_json
{
    typedef char one[1];
    typedef char two[2];

    template <typename C> static one& test( decltype(&C::to_json_str)) ;
    template <typename C> static two& test(...);

public:
    enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

template <typename T>
class obj_serializes_to_string
{
    typedef char one[1];
    typedef char two[2];

    template <typename C> static one& test( decltype(&C::to_string) ) ;
    template <typename C> static two& test(...);

public:
    enum { value = sizeof(test<T>(0)) == sizeof(one) };
};

enum class type_id: uint8_t{
	// default:
	UNDEFINED = 0,
	// base types
	BOOL,
	CHAR,
	UINT8,
	INT8,
	UINT16,
	INT16,
	UINT32,
	INT32,
	UINT64,
	INT64,
	FLOAT,
	DOUBLE,

	// special:
	OBJECT,
	STRING,
	RAW_DATA,
	SCENE_2D,
	SCENE_3D,
	JSON_STRING,       // object has to_json_str()/from_json_str() methods
	SERIALIZED_STRING, // object has to_string()/from_string() methods


	// object based special types
	MAN_CTRL = 100,

	// only used in definition / helper
	OBJECT_END = 255
};


bool type_id_is_value(type_id t);
bool type_id_is_num(type_id t);
bool type_id_is_obj(type_id t);
bool type_id_is_binary(type_id t);

/// Type is a container for something you want to serialize. It holds the information
/// how to serialize the object you created the type from, and a pointer to this object (void* obj).
class type{
public:

	type_id id;

	void* obj;

	union serialize_type_info{
		member_list attr;
		blob_type rexp;
		size_t size;
	};

	serialize_type_info serialize_info = {};

	type(); // undefined

	type(bool& o);
	type(char& o);
	type(uint8_t& o);
	type(int8_t& o);
	type(uint16_t& o);
	type(int16_t& o);
	type(uint32_t& o);
	type(int32_t& o);
	type(uint64_t& o);
	type(int64_t& o);
	type(float& o);
	type(double& o);
	type(std::string& o);

	type(member_list lst);
	type(abmt::blob& blk);

	template<int N>
	type(const member(&ptr)[N] ){
		id = type_id::OBJECT;
		serialize_info.attr.length = N;
		serialize_info.attr.list = ptr;
		obj = 0;
	}

	template<class C>
	type(C& o){
		C* zero_ptr_for_offsets = 0;
		*this = define_type(zero_ptr_for_offsets);
		obj = &o;
	}



	//do not call directly: call via template above and create_type;
	type(blob_type r);
	type(string_type s);
	type(scene_2d_type s);
	type(scene_3d_type s);

	type(const abmt::serialize::type& e) = default;
	type(abmt::serialize::type& e) = default; // very important! copy-assingment would be overwriten by template

	template<typename T>
	type& operator=(T& rhs){ // "&" is important here! You dont want a copy here
		*this = type(rhs);
		return *this;
	}

	size_t get_def_size();
	size_t get_def(void* def_buff, size_t def_size); // returns bytes written to def_buff (used internally)


	size_t get_size();
	void   serialize(void* data, size_t data_size);
	void   deserialize(void* data, size_t data_size);
	void   deserialize(void* data, size_t data_size, void* def_buff, size_t def_size);

	double to_double(void* data_ptr, size_t data_len);
	void   from_double(double d);

private:
	// returns bytes copied from data to this.obj;
	size_t copy(void* data, size_t data_size, bool reverse = false);
};


struct member{
	const char* name;
	type t;
	size_t offset;

	/// Expects that member is referenced to zero so that the value of a pointer to member
	/// represents the offset to the start of the object that it belongs to.
	/// See examples [flex tape meme]...
	template<typename T>
	member(const char* name_str, T& member):name(name_str),t(type(member)){
		offset = (size_t)&member;
	}

	template<typename T>
	member(const char* name_str, T* member, void* obj):name(name_str),t(type(*member)){
		offset = (size_t)member - (size_t)obj;
	}

};

// default type: undefined
template<class C> type define_type(C* ptr){
	return type();
}

} // namespace serialize
} // namespace abmt

#endif /* UTIL_EXAMPLES_SERIALIZE2_H_ */
