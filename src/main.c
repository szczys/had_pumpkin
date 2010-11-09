#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


#define rowDDR	DDRC
#define rowPort	PORTC
#define row0	(1<<PC0)
#define row1	(1<<PC1)
#define row2	(1<<PC2)
#define row3	(1<<PC3)
#define row4	(1<<PC4)
//#define rowMask ( row0 | row1 | row2 | row3 | row4 )
#define rowMask ( (1<<PC0) | (1<<PC1) | (1<<PC2) | (1<<PC3) | (1<<PC4) )

#define colDDR0	DDRB
#define colPort0 PORTB
#define colMask0 ( (1<<PB0) | (1<<PB1) | (1<<PB2) | (1<<PB3) | (1<<PB4) | (1<<PB5) | (1<<PB6) | (1<<PB7) )

#define colDDR1	DDRD
#define colPort1 PORTD
#define colMask1 ( (1<<PD2) | (1<<PD3) | (1<<PD4) | (1<<PD5) | (1<<PD6) | (1<<PD7) )

unsigned char row_track = 0;
volatile int buffer[5] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };

//NOTE: Messages must be all upper case (only ascii characters 32-132 are valid)
//It is helpful to end messages with a space so they look nice.
static const char PROGMEM message1[] = "HAPPY HALLOWEEN ";
static const char PROGMEM message2[] = "HACKADAY.COM ";
static const char PROGMEM message_boo[] = "BOO";

/*
* Font source:
* http://instruct1.cit.cornell.edu/courses/ee476/video/Video32v2.c
*/
static const char PROGMEM font[]={
        //Space
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //! THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //" THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //# THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //$ THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //% THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //& THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //' THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //( THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //) THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //* THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //+ THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //, THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //- THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //.
	0b00000000,
	0b00000000,
	0b00000000,
	0b11001100,
	0b11001100,
        //forward-slash THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	//0
        0b11101110,
	0b10101010,
	0b10101010,
	0b10101010,
	0b11101110,
	//1
	0b01000100,
	0b11001100,
	0b01000100,
	0b01000100,
	0b11101110,
	//2
	0b11101110,
	0b00100010,
	0b11101110,
	0b10001000,
	0b11101110,
	//3
	0b11101110,
	0b00100010,
	0b11101110,
	0b00100010,
	0b11101110,
	//4
	0b10101010,
	0b10101010,
	0b11101110,
	0b00100010,
	0b00100010,
	//5
	0b11101110,
	0b10001000,
	0b11101110,
	0b00100010,
	0b11101110,
	//6
	0b11001100,
	0b10001000,
	0b11101110,
	0b10101010,
	0b11101110,
	//7
	0b11101110,
	0b00100010,
	0b01000100,
	0b10001000,
	0b10001000,
	//8
	0b11101110,
	0b10101010,
	0b11101110,
	0b10101010,
	0b11101110,
	//9
	0b11101110,
	0b10101010,
	0b11101110,
	0b00100010,
	0b01100110,
	//:
	0b00000000,
	0b01000100,
	0b00000000,
	0b01000100,
	0b00000000,
        //; THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //< THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	//=
	0b00000000,
	0b11101110,
	0b00000000,
	0b11101110,
	0b00000000,
        //> THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //? THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
        //@ THIS IS JUST A PLACEHOLDER FOR THIS CHARACTER. FIXME
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000000,
	//A
	0b11101110,
	0b10101010,
	0b11101110,
	0b10101010,
	0b10101010,
	//B
	0b11001100,
	0b10101010,
	0b11101110,
	0b10101010,
	0b11001100,
	//C
	0b11101110,
	0b10001000,
	0b10001000,
	0b10001000,
	0b11101110,
	//D
	0b11001100,
	0b10101010,
	0b10101010,
	0b10101010,
	0b11001100,
	//E
	0b11101110,
	0b10001000,
	0b11101110,
	0b10001000,
	0b11101110,
	//F
	0b11101110,
	0b10001000,
	0b11101110,
	0b10001000,
	0b10001000,
	//G
	0b11101110,
	0b10001000,
	0b10001000,
	0b10101010,
	0b11101110,
	//H
	0b10101010,
	0b10101010,
	0b11101110,
	0b10101010,
	0b10101010,
	//I
	0b11101110,
	0b01000100,
	0b01000100,
	0b01000100,
	0b11101110,
	//J
	0b00100010,
	0b00100010,
	0b00100010,
	0b10101010,
	0b11101110,
	//K
	0b10001000,
	0b10101010,
	0b11001100,
	0b11001100,
	0b10101010,
	//L
	0b10001000,
	0b10001000,
	0b10001000,
	0b10001000,
	0b11101110,
	//M
	0b10101010,
	0b11101110,
	0b11101110,
	0b10101010,
	0b10101010,
	//N
	0b00000000,
	0b11001100,
	0b10101010,
	0b10101010,
	0b10101010,
	//O
	0b01000100,
	0b10101010,
	0b10101010,
	0b10101010,
	0b01000100,
	//P
	0b11101110,
	0b10101010,
	0b11101110,
	0b10001000,
	0b10001000,
	//Q
	0b01000100,
	0b10101010,
	0b10101010,
	0b11101110,
	0b01100110,
	//R
	0b11101110,
	0b10101010,
	0b11001100,
	0b11101110,
	0b10101010,
	//S
	0b11101110,
	0b10001000,
	0b11101110,
	0b00100010,
	0b11101110,
	//T
	0b11101110,
	0b01000100,
	0b01000100,
	0b01000100,
	0b01000100, 
	//U
	0b10101010,
	0b10101010,
	0b10101010,
	0b10101010,
	0b11101110, 
	//V
	0b10101010,
	0b10101010,
	0b10101010,
	0b10101010,
	0b01000100,
	//W
	0b10101010,
	0b10101010,
	0b11101110,
	0b11101110,
	0b10101010,
	//X
	0b00000000,
	0b10101010,
	0b01000100,
	0b01000100,
	0b10101010,
	//Y
	0b10101010,
	0b10101010,
	0b01000100,
	0b01000100,
	0b01000100,
	//Z
	0b11101110,
	0b00100010,
	0b01000100,
	0b10001000,
	0b11101110
	};




//Delay a number of milliseconds
void Delay_ms(int cnt)	//Function delays a give number of milliseconds.  Depends on F_CPU being defined
{
  while (cnt-->0) _delay_ms(1);
}

//Initialize the Timers
static inline void initTimers(void)	//Function used once to set up the timer
{
  TCCR1B |= 1<<WGM12 | 1<<CS10;		//Start timer1 in CTC mode with no prescaler
  TIMSK1 |= 1<<OCIE1A;			//Enable compare match interrupt
  OCR1A = 0x0BB0;			//Set compare value for 500 times per second
  sei();				//Enable global interrupts
}

//Setup the row drivers
void initRows(void)
{
  rowPort &= ~rowMask;	//Set row pins low
  rowDDR |= rowMask;	//Set row pins as outputs
}

//Setup the column drivers
void initCols(void)
{
  colPort0 &= ~colMask0;
  colPort1 &= ~colMask1;
  colDDR0 |= colMask0;
  colDDR1 |= colMask1;
}

//Scroll a letter into the displays.
//PARAMS:
//letter:
//delay: milliseconds to pause after each column
void scrollLetter(unsigned char letter, unsigned char delay)
{
  //Subtract 32 so that ASCII value matches font[] indexing
  // ie: A is ASCII value 65 but font[33]. 65-32=33
  letter -= 32;
  
  //Get font char from PROGMEM
  unsigned char thisLetter[5] = { 
        pgm_read_byte((char *)((int)font + (5 * letter) + 0)),
        pgm_read_byte((char *)((int)font + (5 * letter) + 1)),
        pgm_read_byte((char *)((int)font + (5 * letter) + 2)),
        pgm_read_byte((char *)((int)font + (5 * letter) + 3)),
        pgm_read_byte((char *)((int)font + (5 * letter) + 4))
  };

  //letters are 4 columns wide (inlcudes space between letters)
  for (unsigned char col=0; col<4 ; col++)
  {
    //diable interrupts
    cli();
    //Shift display buffer
    for (unsigned char i=0; i<5; i++) buffer[i] <<= 1;
    //write char data to buffer
    for (unsigned char i=0; i<5; i++) buffer[i] |= ((int)thisLetter[i] >> (7-col));
    //enable interrupts
    sei();
    Delay_ms(delay);
  }
}

//Clear the display by scrolling in spaces
void scrollClear(unsigned char delay)
{
  for (unsigned char i=0; i<4; i++) scrollLetter(' ',delay);
}

//Instantly clear the display
void clearScreen(void)
{
  cli();
  for (unsigned char i=0; i<5; i++) buffer[i] = 0x0000;
  sei();
}

//Sweep three rows back and forth
void sweepThree(unsigned char times)
{
  unsigned int sweeper = 0x0001;
  for (unsigned char count=times; count>0; count--)
  {
    while (sweeper < 0x2000)
    {
      cli();
      buffer[0] = 0x0000;
      buffer[1] = sweeper;
      buffer[2] = sweeper;
      buffer[3] = sweeper;
      buffer[4] = 0x0000;
      sei();
      sweeper <<= 1;
      Delay_ms(10);
    }
    while (sweeper > 0x0001)
    {
      cli();
      buffer[0] = 0x0000;
      buffer[1] = sweeper;
      buffer[2] = sweeper;
      buffer[3] = sweeper;
      buffer[4] = 0x0000;
      sei();
      sweeper >>= 1;
      Delay_ms(10);
    }
  }
}

//Sweep around the outside box of the display
void sweepBox(unsigned char times)
{
  clearScreen();
  unsigned char delay = 20;
  unsigned int sweeper = 0x0001;
  for(unsigned char count=times; count>0; count--)
  {
    
    //Sweep top row
    buffer[1] = 0x0000;
    while (sweeper < 0x2000)
    {
      cli();
      buffer[0] = sweeper;
      sei();
      sweeper <<= 1;
      Delay_ms(delay);
    }
    //Sweep side
    cli();
    buffer[0] = sweeper;
    sei();
    Delay_ms(delay);
    cli();
    buffer[0] = 0x0000;
    buffer[1] = sweeper;
    sei();
    Delay_ms(delay);
    cli();
    buffer[1] = 0x0000;
    buffer[2] = sweeper;
    sei();
    Delay_ms(delay);
    cli();
    buffer[2] = 0x0000;
    buffer[3] = sweeper;
    sei();
    Delay_ms(delay);
    //Sweep bottom row
    buffer[3] = 0x0000;
    while (sweeper > 0x0001)
    {
      cli();
      buffer[4] = sweeper;
      sei();
      sweeper >>= 1;
      Delay_ms(delay);
    }
    //Sweep other side
    cli();
    buffer[4] = sweeper;
    sei();
    Delay_ms(delay);
    cli();
    buffer[4] = 0x0000;
    buffer[3] = sweeper;
    sei();
    Delay_ms(delay);
    cli();
    buffer[3] = 0x0000;
    buffer[2] = sweeper;
    sei();
    Delay_ms(delay);
    cli();
    buffer[2] = 0x0000;
    buffer[1] = sweeper;
    sei();
    Delay_ms(delay);
  }
  clearScreen();
}


//Scroll in strings that are stored in PROGMEM
void Scroll_String_P(const char * myString, unsigned char delay)  //Function that reads a string out of memory and displays it
{
  while (pgm_read_byte(myString))
  {
    scrollLetter(pgm_read_byte(myString),delay);
    ++myString;
  }
}

int main(void)
{
  initRows();
  initCols();
  initTimers();


  for(;;)
  {
    sweepBox(5);
    Scroll_String_P(message1,200);
    scrollClear(200);
    Scroll_String_P(message2,200);
    scrollClear(200);
    Delay_ms(5000);
    Scroll_String_P(message_boo,0);
    Delay_ms(2000);
    clearScreen();
    sweepThree(25);
    clearScreen();
    Delay_ms(1000);
  }
}


ISR(TIMER1_COMPA_vect)	//Interrupt Service Routine handles multiplexing
{
  //Shutdown all rows
  rowPort &= ~rowMask;

  //Shutdown all columns
  colPort0 &= ~colMask0;
  colPort1 &= ~colMask1;
  
  //Set buffer data to columns
  colPort0 = (char)buffer[row_track];
  colPort1 |= ((char)(buffer[row_track] >> 6) & 0xFC); //Shift data and mask out lower bits (reserver for Rx and Tx)

  //Drive row
  rowPort |= (1<<(4-row_track));

  //Preload row for next interrupt
  if(++row_track == 5) row_track = 0;		//Row tracking
}
