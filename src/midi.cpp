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

#include "midi.h"

Midi::Midi(uint16_t _buffer_size, void (*_reader)(uint8_t*, uint16_t), void (*_writer)(uint8_t*, uint16_t)){
    buffer_size = _buffer_size;
    input_buffer = new uint8_t[buffer_size];
    reader = _reader;
    writer = _writer;
}

void Midi::process_midi(uint8_t _data){
    // if incoming byte is status
    if (_data & 0x80)
    {
        uint8_t command = _data & 0xF0;
        uint8_t channel = _data & 0x0F;
        switch (command)
        {
        case 0x80: // Note-off, note, velocity
        case 0x90: // Note-on, note, velocity
        case 0xA0: // Aftertouch, note, touch
        case 0xB0: // CC, controller, value
        case 0xE0: // Pitch bend, pitch_lsb, pitch_msb
            expect = 2;
            break;
        case 0xC0: // Patch change, instrument
        case 0xD0: // Channel pressure, pressure
            expect = 1;
            break;
        case 0xF0:
            switch (channel)
            {
            case 0x00: // Sys-ex start
                expect = -1;
                break;
            case 0x01: // MIDI time code quarter frame, frame
            case 0x03: // Song select, song
                expect = 1;
                break;
            case 0x02: // Song position pointer, position_lsb, position_msb
                expect = 2;
                break;
            case 0x04: // Undefined
            case 0x05: // Undefined
            case 0x06: // Tune request
                expect = 0;
                break;
            case 0x07: // Sys-ex end
                input_buffer[length++] = _data;
                reader(input_buffer, length);
                length = 0;
                expect = 0;
                return;
            case 0x08: // Timing Clock
            case 0x09: // Undefined
            case 0x0A: // Start
            case 0x0B: // Continue
            case 0x0C: // Stop
            case 0x0D: // Undefined
            case 0x0E: // Active Sensing
            case 0x0F: // Realtime messages
                expect = 0;
                break;
            default:
                break;
            }
        default:
            break;
        }
        length = 0;
        input_buffer[length++] = _data;
        // if command dont have parameter
        if (length > expect)
        {
            reader(input_buffer, length);
        }
    }
    // if incoming byte is data
    else
    {
        // if we dont have enough bytes for message
        if (length <= expect)
        {
            input_buffer[length++] = _data;
        }
        // if we have enough bytes for message
        if(length > expect)
        {
            reader(input_buffer, length);
            // dirty running status hack
            length = 1;
        }
    }
}

void Midi::process_midi(uint8_t* _buffer, uint8_t _length){
    for (auto i = 0; i < _length; i++)
    {
        process_midi(_buffer[i]);
    }
}

void Midi::send_raw(uint8_t* _buffer, uint16_t _length){
    writer(_buffer, _length);
}

void Midi::send_note_off(uint8_t _note, uint8_t _velocity, uint8_t _channel){
    output_buffer[0] = 0x80 | (_channel & 0x0F);
    output_buffer[1] =  _note & 0x7F;
    output_buffer[2] =  _velocity & 0x7F;
    writer(output_buffer, 3);
}

void Midi::send_note_on(uint8_t _note, uint8_t _velocity, uint8_t _channel){
    output_buffer[0] = 0x90 | (_channel & 0x0F);
    output_buffer[1] =  _note & 0x7F;
    output_buffer[2] =  _velocity & 0x7F;
    writer(output_buffer, 3);
}

void Midi::send_aftertouch(uint8_t _note, uint8_t _touch, uint8_t _channel){
    output_buffer[0] = 0xA0 | (_channel & 0x0F);
    output_buffer[1] =  _note & 0x7F;
    output_buffer[2] =  _touch & 0x7F;
    writer(output_buffer, 3);
}

void Midi::send_control_change(uint8_t _control, uint8_t _value, uint8_t _channel){
    output_buffer[0] = 0xB0 | (_channel & 0x0F);
    output_buffer[1] =  _control & 0x7F;
    output_buffer[2] =  _value & 0x7F;
    writer(output_buffer, 3);
}

void Midi::send_clock(){
    output_buffer[0] = 0xF8;
    writer(output_buffer, 1);
}