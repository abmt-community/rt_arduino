/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */
#include <abmt/math/vec.h>

template<> abmt::serialize::type abmt::serialize::define_type(vec2* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
	};
	return (abmt::serialize::member_list)list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec3* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
	};
	return (abmt::serialize::member_list)list;
}

template<> abmt::serialize::type abmt::serialize::define_type(vec4* target){
	static const abmt::serialize::member list[] = {
		ABMT_DEF_PTR_MEMBER(target, x),
		ABMT_DEF_PTR_MEMBER(target, y),
		ABMT_DEF_PTR_MEMBER(target, z),
		ABMT_DEF_PTR_MEMBER(target, w),
	};
	return (abmt::serialize::member_list)list;
}



