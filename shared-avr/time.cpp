/**
 * Author: Hendrik van Arragon, 2021
 * SPDX-License-Identifier: MIT
 */

#include <abmt/time.h>

namespace abmt{

time::time(int64_t ns):ns_since_1970(ns){

}

time time::ns(int64_t t){
	return time(t);
}

time time::us(int64_t t){
	return time(t * 1000);
}

time time::ms(int64_t t){
	return time(t * 1000 * 1000);
}

time time::sec(int64_t t){
	return time(t * 1000 * 1000 * 1000);
}

time time::min(int64_t t){
	return time(t * 1000 * 1000 * 1000 * 60);
}

time time::hour(int64_t t){
	return time(t * 1000 * 1000 * 1000 * 60 * 60);
}

time time::day(int64_t t){
	return time(t * 1000 * 1000 * 1000 * 60 * 60 * 24);
}

time time::week(int64_t t){
	return time(t * 1000 * 1000 * 1000 * 60 * 60 * 24 * 7);
}

time time::year(int64_t t){
	return time(t * 1000 * 1000 * 1000 * 60 * 60 * 24 * 7 * 52);
}


int64_t time::ns(){
	return ns_since_1970;
}

int64_t time::us(){
	return ns_since_1970/(1000);
}

int64_t time::ms(){
	return ns_since_1970/(1000*1000);
}

int64_t time::sec(){
	return ns_since_1970/(1000*1000*1000);
}

int64_t time::min(){
	return ns_since_1970/(1000LL*1000*1000*60);
}

int64_t time::hour(){
	return ns_since_1970/(1000LL*1000*1000*60*24);
}

int64_t time::week(){
	return ns_since_1970/(1000LL*1000*1000*60*24*7);
}

int64_t time::year(){
	return ns_since_1970/(1000LL*1000*1000*60*24*7*52);
}

double time::sec2(){
	return (double)ns_since_1970/(1000*1000*1000);
}

time time::delta_to_now(){
	return now() - *this;
}

bool time::operator==(const time &t) { return ns_since_1970 == t.ns_since_1970; }
bool time::operator!=(const time &t) { return ns_since_1970 != t.ns_since_1970; }
bool time::operator< (const time &t) { return ns_since_1970 <  t.ns_since_1970; }
bool time::operator> (const time &t) { return ns_since_1970 >  t.ns_since_1970; }
bool time::operator<=(const time &t) { return ns_since_1970 <= t.ns_since_1970; }
bool time::operator>=(const time &t) { return ns_since_1970 >= t.ns_since_1970; }

time& time::operator+=(const time &t){
	ns_since_1970 = ns_since_1970 + t.ns_since_1970;
	return *this;
}

time& time::operator-=(const time &t){
	ns_since_1970 = ns_since_1970 - t.ns_since_1970;
	return *this;
}

time& time::operator*=(const time &t){
	ns_since_1970 = ns_since_1970 * t.ns_since_1970;
	return *this;
}

time& time::operator/=(const time &t){
	ns_since_1970 = ns_since_1970 / t.ns_since_1970;
	return *this;
}

time time::operator+(const time &t) const{
	time res(*this);
	res += t;
	return res;
}
time time::operator-(const time &t) const{
	time res(*this);
	res -= t;
	return res;
}
time time::operator*(const time &t) const{
	time res(*this);
	res *= t;
	return res;
}
time time::operator/(const time &t) const{
	time res(*this);
	res /= t;
	return res;
}



time operator*(const float& n,const time &rhs) {
	time res(rhs);
	res.ns_since_1970 = res.ns_since_1970 * n;
	return res;
}

} // namespace
