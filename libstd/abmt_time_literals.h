/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */
 #pragma once

#include <abmt/time.h>

inline abmt::time operator""s(unsigned long long int sec)
{
    return abmt::time::sec(sec);
}

inline abmt::time operator""ms(unsigned long long int sec)
{
    return abmt::time::ms(sec);
}

inline abmt::time operator""us(unsigned long long int sec)
{
    return abmt::time::us(sec);
}
