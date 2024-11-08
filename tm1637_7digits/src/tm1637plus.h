/*
C++ version for the ESP-IDF by Francois Blanchette.
https://github.com/cfrankb/esp32-tm1637plus-cpp

MicroPython TM1637 quad 7-segment LED display driver
https://github.com/mcauser/micropython-tm1637

MIT License
Copyright (c) 2016 Mike Causer
Copyright (c) 2024 Francois Blanchette

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <stdint.h>

void tm1637_init(void);
void tm1637_set_brightness(const uint8_t val);
void tm1637_show(const char *s, __bit colon);
void tm1637_set_raw(const uint8_t pos, const uint8_t data);
void tm1637_set_char(const uint8_t pos, const char ch, const uint8_t dot);