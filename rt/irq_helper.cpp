/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#include <avr/interrupt.h>
#include "irq_helper.h"


void irq::enable(){
	irq::cnt--;
	if(cnt <= 0){
		sei();
	}
}

void irq::disable(){
	cli();
	irq::cnt++;

}
irq::isr_scope_lock::isr_scope_lock(){
	irq::disable();
}
irq::isr_scope_lock::~isr_scope_lock(){
	irq::enable();
}


irq::isr_scope_lock irq::scope_lock(){
	return {};
}

int irq::cnt = 0;
