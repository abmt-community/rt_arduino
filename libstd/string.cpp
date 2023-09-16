/**
 * Author: Hendrik van Arragon, 2023
 * SPDX-License-Identifier: MIT
 */
#include <string>
#include <stdio.h>

namespace std{

std::string to_string( int value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%d",value);
    return buff;
}

std::string to_string( long value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%d",value);
    return buff;
}

std::string to_string( long long value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%d",value);
    return buff;
}

std::string to_string( unsigned value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%d",value);
    return buff;
}

std::string to_string( unsigned long value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%d",value);
    return buff;
}

std::string to_string( unsigned long long value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%d",value);
    return buff;
}

std::string to_string( float value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%f",value);
    return buff;
}

std::string to_string( double value ){
    char buff[20];
    snprintf(buff, sizeof(buff),"%f",value);
    return buff;
}

}//namespace std