/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */

#ifndef RT_IRQ_HELPER_H_
#define RT_IRQ_HELPER_H_


struct irq{
    static int cnt;

    static void enable();
    static void disable();
    struct isr_scope_lock{
        isr_scope_lock();
        ~isr_scope_lock();
    };

    static isr_scope_lock scope_lock();
};


#endif /* RT_IRQ_HELPER_H_ */
