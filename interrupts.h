// Hall sensor interrupt
ISR(INT0_vect) {
    counter++; // Increment the counter
    EIFR |= (1 << INTF0); // Clear the interrupt flag
}

// Timer interrupt 
ISR(TIMER1_COMPA_vect) {

    uint32_t current_distance = (2 * pi * radius) * counter; // Calculate distance in cm
    distance += current_distance; // Update distance
    float distance_display = distance / (100*1000.0); // Convert to km
    speed = current_distance / 100 * (3600.0/1000.0); // Calculate speed in km/h

    save_distance(distance);

    counter = 0; 

    LCD_Write(0, "ODO: ", " km", distance_display); // Display the speed on the LCD
    LCD_Write(1, "Speed: ", " km/h", speed); // Display the speed on the LCD
}

// Menu interrupt
ISR(INT1_vect)
{
    _delay_ms(200); // Wait 20 ms
    while ((PIND & (1 << PD3))==0); // hold the button
    menu();
    EIFR |= (1 << INTF1); // Clear the interrupt flag
}