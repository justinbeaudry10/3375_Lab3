#include "address_map_arm.h";

// #define SW_BASE  0xFF200040
// #define JP1_BASE 0xFF200060
// #define ADC_BASE 0xFF204000

#define Mask_12_bits 0x00000FFF // ???

typedef struct _GPIO
{
    unsigned int data;    // I/O data bits
    unsigned int control; // Set 0 for input, 1 for output
} GPIO;

volatile int *SW_ptr = (int *)SW_BASE;    // Switches
volatile int *LED_ptr = (int *)JP1_BASE;  // LEDs
volatile *ADC_ptr = ADC_BASE;             // Analog to Digital converter
volatile GPIO *const GPIO_ptr = JP1_BASE; // GPIO ports

void DisplayLeds(int val)
{
    // Data will be 12 bits, which means max value is 4096
    // We have 10 leds, which will all be set to off if value is 0
    // They will all be set to on if value is 4096 (or 4095 just in case not completely accurate)
    // This leaves us with 9 leds, so we need to make 9 ranges
    // 4096 / 9 = ~455

    if (val >= 4095)
    {
        GPIO_ptr->data = 1023; // 1023 = 0b1111111111
    }
    else if (val >= 3640)
    {
        GPIO_ptr->data = 511; // 511 = 0b111111111
    }
    else if (val >= 3185)
    {
        GPIO_ptr->data = 255; // 255 = 0b11111111
    }
    else if (val >= 2730)
    {
        GPIO_ptr->data = 127; // 127 = 0b1111111
    }
    else if (val >= 2275)
    {
        GPIO_ptr->data = 63; // 63 = 0b111111
    }
    else if (val >= 1820)
    {
        GPIO_ptr->data = 31; // 31 = 0b11111
    }
    else if (val >= 1365)
    {
        GPIO_ptr->data = 15; // 15 = 0b1111
    }
    else if (val >= 910)
    {
        GPIO_ptr->data = 7; // 7 = 0b111
    }
    else if (val >= 455)
    {
        GPIO_ptr->data = 3; // 3 = 0b11
    }
    else if (val >= 1)
    {
        GPIO_ptr->data = 1; // 1 = 0b1
    }
    else
    {
        GPIO_ptr->data = 0; // 0 will set all leds to off
    }
}

int ReadSwitches(void)
{
    // Returns 1 (true) if switch 0 is flipped on
    return (*(SW_ptr)&1);
}

int main(void)
{

    // Mask used to make values more accurate?
    int mask = Mask_12_bits;

    // Channels
    volatile int ch0, ch1;

    GPIO_ptr->control = 0x3FF; // 0011 1111 1111 (set ports 0-9 to output, 10-11 to input)?

    // Variable to store data from ADC
    int data = 0;

    // Main while loop
    while (1)
    {

        // Reading data from first 2 channels
        ch0 = (*(ADC_ptr)&mask);
        ch1 = (*(ADC_ptr + 1) & mask);

        // If first switch is flipped, read from 2nd potentiometer, otherwise read from 1st potentiometer
        if (ReadSwitches())
        {
            // Get the data from channel 1
            data = ch1;
        }
        else
        {
            // Get the data from channel 2
            data = ch0;
        }

        // Displaying the data with LEDs
        DisplayLeds(data);
    }
