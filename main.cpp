/*
SPEEDodometer
Authors: Michał Dobrzycki, Bartłomiej Osiak
Date: 21.05.2025
Chip: ATMEGA328P
AGH University of Krakow
 */

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include <stdlib.h>
#include <stdint.h>

// Botland referecne to this library :)
#include "modules/HD44780.c"

// Set up the microcontroller global variables
uint8_t counter = 0; // Counter for the number of pulses from the Hall sensor
uint8_t display_counter = 0; // Counter for the number of interrupts for the LCD display update
uint8_t speed = 0; // [m/s]
float speed_tab[5] = {0,0,0,0,0}; // [m/s]
uint32_t distance = 0; // [cm]
uint8_t radius = 10; // default [cm]
const float pi = 3.14159; 

// Our own libraries
#include "modules/functions.h"
#include "modules/interrupts.h"

int main(void)
{
    init(); // Initialize the LCD
    while (1) 
    {
		
    }
}

