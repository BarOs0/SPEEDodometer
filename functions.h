// Eeprom maintenance for variables
void save_radius(uint8_t rad){
    eeprom_update_byte((uint8_t*)0x04, rad);
}

uint8_t load_radius(){
    return eeprom_read_byte((uint8_t*)0x04);
}

void save_distance(uint32_t dist) {
    eeprom_update_dword((uint32_t*)0x00, dist);
}

uint32_t load_distance() {
    return eeprom_read_dword((uint32_t*)0x00);
}

// Initialization function
void init(){
    // Turning off TX/RX (USART off for D0 and D1 ports)
    asm volatile (
        "ldi r16, 0\n\t"         
        "sts %a0, r16\n\t"        
        :
        : "i" (_SFR_MEM_ADDR(UCSR0B))
        : "r16"
    );

    // LCD initialization
    LCD_Initalize(); 
    LCD_Clear(); 

    // Greetings
    LCD_GoTo(0, 0); 
    LCD_WriteText("SPEEDodometer"); 
    LCD_GoTo(0, 1); 
    LCD_WriteText("DOBRZYCKI&OSIAK"); 
    _delay_ms(1000);

    // Set up interrupts, ports and pull-ups 

    // INT0 HALL
    DDRD &= ~(1 << PD2);

    // INT1 BUTTON
    DDRD &= ~(1 << PD3);
    PORTD |= (1 << PD3);

    // Set up the interrupt for the Hall sensor 
    EICRA |= (1 << ISC01); // Trigger INT0 on falling edge
    EIMSK |= (1 << INT0); // Enable INT0

    // Set up the interrupt for the menu button
    EICRA |= (1 << ISC11); // Trigger INT1 on falling edge
    EIMSK |= (1 << INT1); // Enable INT1

    // set up timer 1s interupt
    TCCR1A = 0; // Set Timer1 to normal mode
    TCCR1B = (1 << WGM12) | (1 << CS12); // Set prescaler to 256
    OCR1A = 62500; // Set compare value for 1 second
    TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt

    // set up timer 16ms interrupt
    TCCR0A = 0; // Set Timer0 to normal mode
    TCCR0B = (1 << WGM02) | (1 << CS02) | (1 << CS00); // Set prescaler to 1024
    OCR0A = 255; // Set compare value for 16ms seconds
    TIMSK0 |= (1 << OCIE0A); // Enable Timer0 compare interrupt

    // PORTC(0,1) as input
    DDRC &= ~(1 << PC0);
    DDRC &= ~(1 << PC1);
    PORTC |= (1 << PC0);
    PORTC |= (1 << PC1);

    // Loading data 
    distance = load_distance();
    radius = load_radius();

    sei(); // Enable global interrupts
    return;
}

// Function to display text on the LCD
void LCD_Write(uint8_t line, char text1[16], char text2[16]=" ", float number = -1){
    LCD_GoTo(0, line);
    LCD_WriteText("                "); // Clear the line
    LCD_GoTo(0, line);
    LCD_WriteText(text1); // Write the first part of the text
    if (number == -1){
        return;
    }
    char n2s[16];
    itoa(number, n2s, 10);
    LCD_WriteText(n2s);
    uint8_t number_dec = number * 10 - (uint8_t)number * 10; // Convert number to string
    if (number_dec != 0){
        itoa(number_dec, n2s, 10);
        LCD_WriteText(",");
        LCD_WriteText(n2s);
    }
    LCD_WriteText(text2); // Write the second part of the text
    return;
}

// Menu in order to reset odo and set up new radius of the wheel
void menu(){

    LCD_Write(0, "> Radius: ", "cm ", radius); // Show current radius [cm]
    LCD_Write(1, "< Res ODO: ", "km ", distance / (100*1000.0)); // Show current odo [km]

    while(1){ // Non interrupt check for options
   
        if ((PINC & (1 << PC0)) == 0){ // if PINC0 is 0 reset odo
            _delay_ms(200);
            while ((PINC & (1 << PC0)) == 0); // hold the button
                distance = 0;
                save_distance(distance);
                LCD_Write(1, "< Res ODO: ", "km ", distance / (100*1000.0));
        }
        else if ((PINC & (1 << PC1)) == 0){ // if PINC1 is 0 increment radius by 10 (range 10 - 40 [cm])
            _delay_ms(200);
            while ((PINC & (1 << PC1)) == 0);
                radius += 1;
                if (radius > 40){
                    radius = 10;
                }
                save_radius(radius);
                LCD_Write(0, "> Radius: ", "cm ", radius);
        }
        else if ((PIND & (1 << PD3)) == 0){ // if PIND3 is 0 return 
            _delay_ms(200);
            while ((PIND & (1 << PD3)) == 0);
            return;
        }
    }
}
