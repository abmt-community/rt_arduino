 /**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

 #include <stdlib.h>

void* operator new(size_t size) {
  return malloc(size);
}

void* operator new(size_t size_,void* ptr_){
	return ptr_;
}

void* operator new[](size_t size) {
  return malloc(size);
}

void operator delete(void* ptr_, size_t size_){
    free(ptr_);
}

void operator delete(void* ptr_){
    free(ptr_);
}

void operator delete[](void* ptr) {
    free(ptr);
}

void operator delete[](void* ptr, size_t size_) {
    free(ptr);
}

extern "C" void __cxa_pure_virtual() { while (1); }