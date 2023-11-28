/*
    MIT License

    Copyright (c) 2023 Malik Enes Şafak

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

#ifndef MIDI_H
#define MIDI_H

#include "pico/stdlib.h"

class Midi
{
private:
    void (*reader)(uint8_t*, uint16_t) = nullptr;
    void (*writer)(uint8_t*, uint16_t) = nullptr;

    uint16_t buffer_size = 0;
    uint8_t* input_buffer = nullptr;
    uint8_t output_buffer[3];
    uint16_t length = 0;
    uint8_t expect = 0;
public:
    Midi(uint16_t _buffer_size, void (*_reader)(uint8_t*, uint16_t), void (*writer)(uint8_t*, uint16_t));
    ~Midi();

    void process_midi(uint8_t _data);
    void process_midi(uint8_t* _buffer, uint8_t _length);

    void send_raw(uint8_t* _buffer, uint16_t _length);
    void send_note_off(uint8_t _note, uint8_t _velocity = 0, uint8_t _channel = 0);
    void send_note_on(uint8_t _note, uint8_t _velocity, uint8_t _channel = 0);
    void send_aftertouch(uint8_t _note, uint8_t _touch, uint8_t _channel = 0);
    void send_control_change(uint8_t _control, uint8_t _value, uint8_t _channel = 0);
    void send_clock();
};

#endif // MIDI_H