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

    if (val >= 455 * 9)
    {
        GPIO_ptr->data = 1023; // 0011 1111 1111
    }
    else if (val >= 455 * 8)
    {
        GPIO_ptr->data = 511; // 0001 1111 1111
    }
    else if (val >= 455 * 7)
    {
        GPIO_ptr->data = 255; // 1111 1111
    }
    else if (val >= 455 * 6)
    {
        GPIO_ptr->data = 127; // 0111 1111
    }
    else if (val >= 455 * 5)
    {
        GPIO_ptr->data = 63; // 0011 1111
    }
    else if (val >= 455 * 4)
    {
        GPIO_ptr->data = 31; // 0001 1111
    }
    else if (val >= 455 * 3)
    {
        GPIO_ptr->data = 15; // 1111
    }
    else if (val >= 455 * 2)
    {
        GPIO_ptr->data = 7; // 0111
    }
    else if (val >= 455)
    {
        GPIO_ptr->data = 3; // 0011
    }
    else if (val >= 1)
    {
        GPIO_ptr->data = 1; // 0001
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

    GPIO_ptr->control = 0x3FF; // 0011 1111 1111 (set ports 0-9 to output, 10-11 to input)

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
