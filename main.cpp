/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <string>

#include "rt/irq_helper.h"
#include "rt/uart_com.h"
#include "rt/model_adapter_tiny.h"
#include "rt/single_thread_task.h"
#include "rt/cpu_ticks.h"

#include "model/model.h"

void abmt::log(std::string s){
	adapter->log(s);
}
void abmt::log_err(std::string s){
	adapter->log_err(s);
}
void abmt::die(std::string s){
	abmt::log_err(s);
	abmt::log_err("reset!");
	wdt_enable(WDTO_250MS);
	while(1){
		// reset done by wdt
	};
}

void abmt::die_if(bool condition, std::string msg){
	if(condition){
		abmt::die(msg);
	}
}

void setup_timer(){
	TCCR1B |= (1 << CS11) | (1 << CS10);
	TCCR1B |= (1 << WGM12);
    OCR1A = 249;
    TIMSK1 |= (1 << OCIE1A);
}

//abmt::time _now = 0;
uint16_t _now_ms = 0;
uint32_t _now_sec = 0;
ISR(TIMER1_COMPA_vect) {
	_now_ms++;
	if(_now_ms >= 1000){
		_now_ms = 0;
		_now_sec++;
	}
}

abmt::time abmt::time::now(){
	irq::disable();
	auto res_ms = _now_ms;
	auto res_sec = _now_sec;
	irq::enable();
	return abmt::time::ms(res_ms) + abmt::time::sec(res_sec);
}

uint16_t idle_ticks = 0;
uint16_t current_idle_ticks = 0;

int main ()
{
	setup_timer();
	uart_init(1000000);
	
	model_adatper_tiny _adapter;
	adapter = &_adapter;

	model m;
	adapter->set_model(&m);

	single_thread_task **raster_array = new single_thread_task*[m.rasters.length];
	for (size_t raster_index = 0; raster_index < m.rasters.length; raster_index++) {
		raster_array[raster_index] = new single_thread_task(m.rasters[raster_index], raster_index, adapter);
	}

	auto now = abmt::time::now();
	auto next_online_send = now + abmt::time::ms(500);
	
	while(1) {
		adapter->connection.check_new_data();

		now = abmt::time::now();
		for (size_t raster_index = 0; raster_index < m.rasters.length; raster_index++) {
			auto next_run = raster_array[raster_index]->next_run;
			if (next_run <= now) {
				raster_array[raster_index]->run();
				now = abmt::time::now();
			}
		}
		
		current_idle_ticks++;
		if( now >= next_online_send ) {
			next_online_send = now + abmt::time::sec(1);
			adapter->send_model_online();
			idle_ticks = current_idle_ticks;
			current_idle_ticks = 0;
		}
    }
	return 0;
}
