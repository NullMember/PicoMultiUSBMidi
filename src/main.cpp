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

    Description:
    Multiple USB MIDI Device Example
    This example is to show how to use Raspberry Pi Pico to 
    implement multiple USB MIDI cables.
    8 USB MIDI cables are exposed by Raspberry Pi Pico and 
    can be used to route MIDI messages between USB MIDI cables.
*/

#include <pico/stdlib.h>

#include <bsp/board.h>
#include <tusb.h>

#include "midi.h"

#define USBMIDICNT CFG_TUD_MIDI
#define TOTALMIDICNT USBMIDICNT
#define ITFNUM 0
#define MIDIBUFLEN 1024

#define MIDISTART 0
#define USBMIDISTART MIDISTART
#define USBMIDIEND (USBMIDISTART + USBMIDICNT)
#define MIDIEND USBMIDIEND

typedef struct
{
    uint8_t id;
    uint8_t route_id;
    uint8_t buffer[MIDIBUFLEN];
    int len;
    int writeindex;
} t_midi;

t_midi midi_ports[TOTALMIDICNT];

void connect_output(t_midi * output_device, t_midi * input_device){
    output_device->route_id = input_device->id;
}

void connect_cross(t_midi * device_1, t_midi * device_2){
    device_1->route_id = device_2->id;
    device_2->route_id = device_1->id;
}

void disconnect_output(t_midi * device){
    device->route_id = -1;
}

void usb_midi_task(uint8_t, t_midi *);
void midi_write(t_midi *);

int main()
{
    // Init USB Device
    board_init();
    tusb_init();
    // Initialize MIDI data
    for (size_t i = MIDISTART; i < MIDIEND; i++)
    {
        midi_ports[i].id = i;
        midi_ports[i].route_id = -1;
        for (size_t j = 0; j < MIDIBUFLEN; j++)
        {
            midi_ports[i].buffer[j] = 0;
        }
        midi_ports[i].len = MIDIBUFLEN;
    }

    // Connect First MIDI Cable's Output to Second MIDI Cable's Input
    connect_output(&midi_ports[0], &midi_ports[1]);
    // Perform main loop
    while(true)
    {
        if(tud_task_event_ready()){
            // Perform USB Device Task
            tud_task();
            // Perform MIDI Tasks
            for (size_t i = USBMIDISTART; i < USBMIDIEND; i++)
            {
                usb_midi_task(i, &midi_ports[i]);
            }
        }
    }

    return 0;
}

void midi_write(t_midi * midi){
    // Write data to routed MIDI cable
    tud_midi_n_stream_write(ITFNUM, midi->route_id, midi->buffer, midi->writeindex);
    midi->writeindex = 0;
}

void usb_midi_task(uint8_t cable_num, t_midi *midi)
{
    // Check if there is data available
    uint32_t available = tud_midi_n_available(ITFNUM, cable_num);
    if (available)
    {
        // Check if there is enough space in the buffer
        if(midi->writeindex + available < midi->len)
        {
            // Read data from USB MIDI cable
            tud_midi_n_stream_read(ITFNUM, cable_num, (midi->buffer + midi->writeindex), midi->len);
            midi->writeindex += available;
            // Check if there is a route to send the data to
            if (midi->route_id != -1)
            {
                // Send data to the route
                midi_write(midi);
            }
        }
        else{
            // If there is not enough space in the buffer (probably because the cable is not connected)
            // Reset the write index
            midi->writeindex = 0;
        }
    }
}