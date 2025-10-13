// Hall sensor interrupt
ISR(INT0_vect) {
    uint32_t timer1_value = TCNT1; // Read the current timer value x * 16us
    TCNT1 = 0; // Reset the timer
    if (counter >= 5) {
        counter = 0; // Reset the counter if it exceeds 5
    }
    uint32_t current_distance = (2 * pi * radius); // Calculate distance in cm
    distance += current_distance; // Update distance
    speed_tab[counter] = (current_distance*10000)/(timer1_value * 16.0); // Store the speed in the first element of the speed array [m/s]
    EIFR |= (1 << INTF0); // Clear the interrupt flag
    counter++; // Increment the counter for each hall sensor pulse
}

// Timer interrupt (speed)
ISR(TIMER1_COMPA_vect) {
    counter = 0;
    for (uint8_t i = 0; i < 5; i++) {
        speed_tab[i] = 0;
    }
}

// Timer interrupt (display)
ISR(TIMER0_COMPA_vect){
    display_counter++; // Increment the display counter every 16ms
    if (display_counter > 30) { // Every 30 interrupts, reset the display counter
        display_counter = 0;
        float average_speed = 0; // Calculate the average speed
        for (uint8_t i = 0; i < 5; i++) {
            average_speed += speed_tab[i]; 
        }
        average_speed /= 5; // Average speed in m/s
        float distance_display = distance / (100*1000.0); // Convert to km
        LCD_Write(0, "ODO: ", " km", distance_display); // Display the distance on the LCD
        LCD_Write(1, "Speed: ", " km/h", average_speed*3.6); // Display the speed on the LCD
        save_distance(distance);
    }
    
}

// Menu interrupt
ISR(INT1_vect)
{
    _delay_ms(200); // Wait 20 ms
    while ((PIND & (1 << PD3))==0); // hold the button
    menu();
    EIFR |= (1 << INTF1); // Clear the interrupt flag
}
