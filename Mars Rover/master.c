#include "lcd.h"
#include "open_interface.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <avr/sleep.h>
#include "master.h"

#ifndef CLOCK_COUNT
	#define CLOCK_COUNT 15625 // TODO - Edit this to be equal to the number of Timer Increments in 1 second
#endif

#ifndef CHECK_COUNT
	#define CHECK_COUNT  3125 // TODO - Edit this to be equal to the number of Timer Increments in 200 ms
#endif

int inter = 0;//intermediate value

int prescale = 1; //0 = 64, 1 = 8
#if(prescale)
	int top = 40000;//50 Hz @ prescale 64: 5000, 8:40000
#else	
	int top = 5000;
#endif

//for scan()
typedef struct
{	unsigned distance;//element for distance of object
	unsigned center;//element for location of center in degrees
	unsigned offset;//the array element for degrees wide
	double width;//element for width of the object
}object;

object objects[12];//object array

//robot 1

//robot 2
int servoMax= 595;
int servo180= 580;
int servo0= 145;
int servoMin= 130;
double onedeg = 2.45;
//not tested
#define PWR 1.094 //not tested //the Excel spreadsheet power value for IR
#define WHITE 600 //IR sensor white reading
#define CLIFF 5 //IR sensor cliff reading
#define ZERO 987       //the bumper distance reading
#define SEVEN 641      //7cm reading
#define TEN 553        //10cm
#define THIRTEEN 482   //13cm
#define SEVENTEEN 415  //17cm
#define TWENTY 377     //20cm
#define TWENTYFOUR 330 //24cm
#define THIRTYTWO 268  //32cm
#define FORTYONE 220   //41cm
#define FIDDY 182      //50cm
#define EIGHTY 125     //80cm

//robot 3

//robot 4
// int servoMax= 550;
// int servo180= 535;
// int servo0= 125;
// int servoMin= 110;
// #define PWR 1.094 //not tested //the Excel spreadsheet power value for IR
// double onedeg = 2.25;
// #define ZERO 987       //the bumper distance reading
// #define SEVEN 641      //7cm reading
// #define TEN 553        //10cm
// #define THIRTEEN 482   //13cm
// #define SEVENTEEN 415  //17cm
// #define TWENTY 377     //20cm
// #define TWENTYFOUR 330 //24cm
// #define THIRTYTWO 268  //32cm
// #define FORTYONE 220   //41cm
// #define FIDDY 182      //50cm
// #define EIGHTY 125     //80cm

//robot 5

//robot 6

//robot 7

// int servoMax= 530;
// int servo180= 510;
// int servo0= 95;
// int servoMin= 75;
// double onedeg = 2.25;
// 
// #define PWR 1.094 //not tested //the Excel spreadsheet power value for IR
// 
// #define WHITE 233;//not tested yet
// #define CLIFF 50;//IR sensor cliff reading
// 
// #define ZERO 930       //the bumper distance reading
// #define SEVEN 598      //7cm reading
// #define TEN 515        //10cm
// #define THIRTEEN 450   //13cm
// #define SEVENTEEN 390  //17cm
// #define TWENTY 350     //20cm
// #define TWENTYFOUR 310 //24cm
// #define THIRTYTWO 250  //32cm
// #define FORTYONE 200   //41cm
// #define FIDDY 175      //50cm
// #define EIGHTY 127     //80cm

//robot 8

//robot 9

//robot 10

// int servoMax= 575;
// int servo180= 560;
// int servo0= 125;
// int servoMin= 110;
// double onedeg = 2.55;
// 
// #define PWR 1.094 //not tested //the Excel spreadsheet power value for IR from one robot
// #define WHITE 600 //IR sensor white reading
// #define CLIFF 5 //IR sensor cliff reading
// 
// #define ZERO 1023      //the bumper distance reading
// #define SEVEN 768      //7cm reading
// #define TEN 652        //10cm
// #define THIRTEEN 550   //13cm
// #define SEVENTEEN 464  //17cm
// #define TWENTY 409     //20cm
// #define TWENTYFOUR 373 //24cm
// #define THIRTYTWO 311  //32cm
// #define FORTYONE 240   //41cm
// #define FIDDY 207      //50cm
// #define EIGHTY 172     //80cm

//robot 11

//robot 12

//robot 13

//robot 14

//robot 15

//robot 16

//robot 17
// int servoMax= 560;
// int servo180= 545;
// int servo0= 110;
// int servoMin= 95;
// double onedeg = 2.30;
// #define PWR 1.094 //not tested //the Excel spreadsheet power value for IR
// //not tested
// #define ZERO 987       //the bumper distance reading
// #define SEVEN 641      //7cm reading
// #define TEN 553        //10cm
// #define THIRTEEN 482   //13cm
// #define SEVENTEEN 415  //17cm
// #define TWENTY 377     //20cm
// #define TWENTYFOUR 330 //24cm
// #define THIRTYTWO 268  //32cm
// #define FORTYONE 220   //41cm
// #define FIDDY 182      //50cm
// #define EIGHTY 125     //80cm

char TX_string[50];

char push;//pushbutton states
char* servdir = "inc";//string for inc/dec display
static int last_val = 180;//last servo value

void finish_song_light(){

	unsigned char notes[] = {55,57,59,61, 55,57,59,61, 56,58,60,62, 56,58,60,62};
	unsigned char duration[] = {9,9,9,9,9,9,9,9,8,8,8,8,8,8,8,8};

	unsigned char notes2[] = {57,59,61,63, 57,59,61,63, 58,60,62,64, 58,60,62,64};
	unsigned char duration2[] = {8,8,8,8,8,8,8,8,7,7,7,7,7,7,7,7};

	unsigned char notes3[] = {59,61,63,65, 60,62,64,66, 61,63,65,67, 62,64,66,68};
	unsigned char duration3[] = {7,7,7,7,7,7,7,7,6,6,6,6,6,6,6,6};

	unsigned char notes4[] = {69,70,71,72, 60,69,61,70, 62,71,63,72, 55,60,65,70};
	unsigned char duration4[] = {6,6,6,6,6,6,6,6,5,5,5,5,5,5,5,5};


	oi_load_song(0, 16,notes, duration);
	oi_load_song(1, 16,notes2, duration2);
	oi_load_song(2, 16,notes3, duration3);
	oi_load_song(3, 16,notes4, duration4);

	oi_play_song(0);

	wait_ms(1600);

	oi_play_song(1);

	wait_ms(1600);

	oi_play_song(2);

	wait_ms(1600);

	oi_play_song(3);

	uint8_t play_led = 2;
	uint8_t advance_led = 1;

	int i;
	//int flag=0;
	int cFlag=0;

	for(i=0; i<50;i++)
	{	int j;
		if(cFlag==0)
		{	for (j=0; j<255; j++)
			{	uint8_t power_color = j;
				uint8_t power_intensity = 0b11111111;
				oi_set_leds(play_led, advance_led, power_color, power_intensity);
				//flag=0;
				wait_ms(5);
			}
			cFlag=1;
		}
		else
		{	for (j=255; j>0; j--)
			{	uint8_t power_color = j;
				uint8_t power_intensity = 0b11111111;
				oi_set_leds(play_led, advance_led, power_color, power_intensity);
				//flag=0;
				wait_ms(5);
			}
			cFlag=0;
		}
	}
}

void Servo_Init()
{	TCCR3A = 0xE3;  //0b11100011 set on A, clear on B
	if(prescale)
	TCCR3B = 0xDA; //prescale 8
	else
	TCCR3B = 0xDB; //prescale 64
	TCCR3C = 0x00;
	ETIMSK = 0x20; //timer 3 input capture and overflow interrupts( 20 for IC, 04 for OVR)
	DDRE |= 0x10; //set pin 4 as output
	if(prescale)
	{	servo0 = servo0 * 8; //880 start count
		servo180 = servo180 * 8; //3520 end count
		onedeg = onedeg * 8;
	}
}
static int x;
int dif; //difference between current angle and last angle

unsigned int servo_set(double angle)
{	return Servo((int)angle);	
}

int Servo(int degrees)
{	OCR3A = top;
	x = ((int)((degrees * onedeg))+servo0);//calculate the pulse width from the passed angle
	if(x >= (servo180))
		x=servo180;
	if(x <= (servo0))
		x=servo0;
	OCR3B = x;//pulse width
	if((ETIFR & 0x10) == 0x10)
	{	ETIFR &= 0xEF; //reset OCF for channel A			
	}
	dif = abs(last_val - x)/5;
	if((ETIFR & 0x08) == 0x08)
	{	ETIFR &= 0xF7; //reset OCF for channel B
		wait_ms(dif);
	}
	//Returns the OCR value of the current run's pulse width
	last_val = x;
	return last_val;
}

volatile unsigned int LET = 0; //Last Event Time
volatile unsigned int event_time = 0;
volatile unsigned char roll = 0; //overflow count
volatile unsigned long del = 0; // the time difference between current and last event
volatile char capt = 0; //the capture state, 0 is the rising capture
double dist = 0; //the distance measured from the sensor
char start = 0; //keep track of sonar start

ISR (TIMER1_CAPT_vect)
{	if(LET)
	{	event_time = ICR1; // read current event time from input 1
		del = (roll * 65536) + event_time - LET; //get the difference
		LET = 0;
		TCCR1B |= 0x40;//watch for rising edge again
	}
	else
	{	LET = ICR1; //since this is first capture, save time for later compare
		roll = 0; //reset roll for next use
		TCCR1B &= 0xbf; //watch for falling edge
	}
}

ISR (TIMER1_OVF_vect)
{	roll++;
}

void Sonar_init()
{	TCCR1A = 0x00;//Compare output mode 0, Wave generate mode 0
	TCCR1B = 0xC3;//Cancel noise, select rising edge, Wave 0, prescale (64)
	TCCR1C = 0x00;//no Forced output compare
	//TIMSK |= 0x24; //Output compare 2 stuff disabled, Timer 1 interrupts enabled (Input capture, Overflow), Output compare 1 stuff disabled
}

void Sonar_start()
{	DDRD |= 0x10; // set PD4 as output
	PORTD |= 0x10; // set PD4 to high
	wait_us(40); // wait 30 microsec (min time)
	PORTD &= 0xEF; // set PD4 to low
	DDRD &= 0xEF; // set PD4 as input
	//cli(); //clear flags and interrupts
	sei(); //allow flags and interrupts again
}

int count = 0;

double ping_pulse()
{	return ping_read();
}

double ping_read()
{	Sonar_start(); // send the starting pulse to PING
	while ((TIFR & _BV(ICF1)) == 0); //while the ping hasn't returned, wait.
	TIFR |= _BV(ICF1); // clear ICF1
	LET = ICR1; // read current event time from input 1
	roll = 0;
	TCCR1B &= 0xbf; //watch for falling edge
	while ((TIFR & _BV(ICF1)) == 0)//while ping time being sent hasn't stopped, wait, or roll over counter.
	{	if((TIFR & _BV(TOV1)) == 1)
		{	roll++;
			TIFR |= _BV(TOV1); // clear TOV1
		}
	}
	event_time = ICR1; // read current event time from input 1
	del = (roll * 65536) + event_time - LET; //get the difference
	TCCR1B |= 0x40;//watch for rising edge again
	TIFR |= _BV(ICF1); // clear ICF1
	dist = ((del * 34000.0) / (2.0 * 2000000.0/8))-15; //get the distance(double)
//	lprintf("Distance: %3.3f cm\ndelta: %3.3f", dist,del); //in centimeters
	roll = 0;
	wait_us(50);/*
	if (dist > 200.0)
	{	if (count > 2)
		{	count = 0;
			dist = 199.5;
		}	
		else
		{	dist = ping_read();
			count++;
		}
	}*/
	return dist;
}

// Initialize the timers
void timer_init(void) {
	// set up timer 1: WGM1 bits = 0100, CS = 101, set OCR1A, set TIMSK
	TCCR1A = 0b00000000;		// WGM1[1:0]=00
	TCCR1B = 0b00001101;		// WGM1[3:2]=01, CS=101
	OCR1A = CLOCK_COUNT - 1; 	// counter threshold for clock
	TIMSK = _BV(OCIE1A);		// enable OC interrupt, timer 1, channel A

	// set up timer 3: WGM1 bits = 0100, CS = 101, set OCR3A, set TIMSK
	TCCR3A = 0b00000000;		// WGM1[1:0]=00
	TCCR3B = 0b00001101;		// WGM1[3:2]=01, CS=101
	OCR3A = CHECK_COUNT - 1; 	// counter threshold for checking push button
	ETIMSK = _BV(OCIE3A);		// enable OC interrupt, timer 3, channel A

	sei();
}

volatile unsigned char charread;
volatile char TXReady = 0;

//clear a string without a lot of main() code
void clrstr(char* str)
{	int length = strlen(str);
	for(int i = 0; i < length; i++)
	{	str[i] = '\0';
	}
}

void ADC_init()
{	ADMUX = _BV(REFS1) | _BV(REFS0);// REFS=11, ADLAR=0, MUX don’t care
	ADMUX |= (0b10 & 0x1F);
	ADCSRA = _BV(ADEN) | (7<<ADPS0);// ADEN=1, ADFR=0, ADIE=0, ADPS=111, others don’t care, page 246 of user guide
}

unsigned int ADC_read()
{	ADCSRA |= _BV(ADSC);
	while (ADCSRA & _BV(ADSC));
	unsigned int ret= ADCL + (ADCH << 8);
	return ret;
}

unsigned int IR_read()
{	unsigned int val = 0;
	unsigned int dis = 0;
	//int iter= 0;//iteration count, for inf loop detect
	// average 3 readings
	for(int count =0;count<2;count++)
	{	val += ADC_read();
		wait_ms(23);//23 is the min time for ADC to setup and decode
	}
	val = val / 2;
	//get the approximate distance.(Excel equations take just as long to calculate!)
	if (val>(SEVEN))
	dis = 7-(((val-SEVEN)*7)/(ZERO-SEVEN));//delta = 320
	
	else if ((val>(TEN))&&(val<(SEVEN)))
	dis = 10-(((val-TEN)*4)/(SEVEN-TEN));
	
	else if ((val>(THIRTEEN))&&(val<(TEN)))
	dis = 13-(((val-THIRTEEN)*4)/(TEN-THIRTEEN));
	
	else if ((val>(SEVENTEEN))&&(val<(THIRTEEN)))
	dis = 17-(((val-SEVENTEEN)*5)/(THIRTEEN-SEVENTEEN));
	
	else if ((val>(TWENTY))&&(val<(SEVENTEEN)))
	dis = 20-(((val-TWENTY)*4)/(SEVENTEEN-TWENTY));
	
	else if ((val>(TWENTYFOUR))&&(val<(TWENTY)))
	dis = 24-(((val-TWENTYFOUR)*5)/(TWENTY-TWENTYFOUR));
	
	else if ((val>(THIRTYTWO))&&(val<(TWENTYFOUR)))
	dis = 32-(((val-THIRTYTWO)*9)/(TWENTYFOUR-THIRTYTWO));
	
	else if ((val>(FORTYONE))&&(val<(THIRTYTWO)))
	dis = 41-(((val-FORTYONE)*9)/(THIRTYTWO-FORTYONE));
	
	else if ((val>(FIDDY))&&(val<(FORTYONE)))
	dis = 50-(((val-FIDDY)*9)/(FORTYONE-FIDDY));
	
	else if (val<(FIDDY))
	dis = (unsigned int) 80 -(((val-EIGHTY)*31)/(FIDDY-EIGHTY));
	
	dis += 6.0;//distance from the sensor to the bumper
	val = 0;
	return dis;
}

ISR (USART0_RX_vect) 
{	charread = UDR0;
}

unsigned char USART_Receive( void )
{	//while ((UCSR0A & (0b10000000)) == 0);// Wait for data to be received
	while(!(UCSR0A & (1<<RXC0)));
	char chrec = UDR0;
	lprintf("%c",chrec);
	UDR0 = 0;
	return chrec;// Get and return received data from buffer
}

void USART_Transmit( unsigned char data )
{	while ( ( UCSR0A & (0b00100000)) == 0 );// Wait for empty transmit buffer
	UDR0 = data;// Put data into buffer, sends the data
}

void USART_Init( unsigned int ubrr )
{	UBRR0H = (unsigned char)(ubrr>>8); UBRR0L = (unsigned char)ubrr;// Set baud rate
	UCSR0A = 0b00000010;// Enable double speed mode
	UCSR0C = 0b00001110;// Set frame format: 8 data bits, 2 stop bits
	UCSR0B = 0b00011000;// Enable receiver and transmitter
}

int getchH;
char getchL;

char* LCDDisp = "       "; //save 8 bytes for LCDDisp string
int read = 0; //the number read, this is for angle fixing, mainly

void move_forward(oi_t*sensor_data, int travel)	// this will move the robot forward the indicated distance
{	move(sensor_data, travel, 250);// move forward; half speed
}

/*I felt this was good for reverse, no checks needed to be done*/
void move_backward(oi_t*sensor_data, int travel) // this will move the robot forward the indicated distance
{	int sum = travel;
	oi_set_wheels(-150, -150); // move backward; half speed
	while (sum > 0)
	{	// Query a list of sensor values
		oi_byte_tx(OI_OPCODE_SENSORS);
		// Send the sensor packet ID
		oi_byte_tx(19);//19 is distance

		// Read angle sensor data
		getchH = oi_byte_rx();
		getchL = oi_byte_rx();
		sensor_data->distance = (getchH << 8) + getchL;
		sum += sensor_data->distance;
	}
	oi_set_wheels(0, 0); // stop
}

void move(oi_t *sensor_data, int mm, int speed)
{	char info = 0;
	unsigned int check = 0;
	if ((mm < 0) && (speed > 0)) //if reverse, and positive speed, make negative to go reverse
	speed = -speed;
	
	oi_set_wheels(speed, speed); // move forward at user defined speed
	int sum = 0;
	mm = abs(mm);
	while (sum < mm) //while travel is not complete, check for distance
	{   oi_update_sensor(sensor_data);
	    sum += abs(sensor_data->distance);
		check = sensor_check(sensor_data);
		if (check) //if danger is detected back up 5 cm
		{	oi_set_wheels(0, 0);
			lprintf("Sensor:%4u\nL :%4u FL:%4u\nFR:%4uR :%4u", check, sensor_data->cliff_left_signal, sensor_data->cliff_frontleft_signal, sensor_data->cliff_frontright_signal, sensor_data->cliff_right_signal);
			sprintf(TX_string, "S%u%u%u%u%u%c", check, sensor_data->cliff_left_signal, sensor_data->cliff_frontleft_signal, sensor_data->cliff_frontright_signal, sensor_data->cliff_right_signal, 0);
			uprint(TX_string);
			move_backward(sensor_data, 50);
			info = 1;
			break;
		}
	}
	oi_set_wheels(0, 0); // stop
	if(!info)
	{	sprintf(TX_string, "S%u%u%u%u%u%c", check, sensor_data->cliff_left_signal, sensor_data->cliff_frontleft_signal, sensor_data->cliff_frontright_signal, sensor_data->cliff_right_signal, 0);
		uprint(TX_string);
	}
}

void turn_clockwise(oi_t*sensor_data, int degrees)	//turn clockwise by indicated angle
{	int sum = 0;
	oi_set_wheels(-50, 50); // slowly turn clockwise
	while (sum > -1*degrees*0.73)
	// robot 14 use 72% for calibration
	// robot 9 use 72% for calibration
	// robot 13 use 73%
	{	oi_update(sensor_data);
		sum += sensor_data->angle;
	}
	oi_set_wheels(0, 0); // stop
}

void turn_counterclockwise(oi_t*sensor_data, int degrees)	//turn clockwise by indicated angle
{	int sum = 0;
	oi_set_wheels(50, -50); // slowly turn clockwise
	while (sum < degrees*0.73)
	// robot 14 use 72% for calibration
	// robot 9 use 72% for calibration
	// robot 13 use 73%
	{	oi_update(sensor_data);
		sum += sensor_data->angle;
	}
	oi_set_wheels(0, 0); // stop
}

double data_ping[181]; //ping data array
unsigned int data_IR[181]; //IR data array
double data_ping2[181]; //ping2
unsigned int data_IR2[181]; //IR2
int array[12][4];//array {degrees, location, distance, length}
volatile int filter[181];

void env_scan(void)
{	//Print set of distance measurements at each angle
	for(int i=0;i<181;i++)//fills array one
	{	Servo(i);
		lprintf("iter:%3u", i);
		data_ping[i] = ping_read();
		if (data_ping[i] < 90)
			data_IR[i] = IR_read();
		else
			data_IR[i] = 100;
	}
}

int detect = 0;//did the IR detect as well
int acnt = 0; //array counter
int ObjDeg = 0; //the angular length of object in degrees.
void analyze_env()
{	acnt = 0;
	for(int i=0;i<181;i++) //Use IR for for edge detection and Ping for accurate distance
	{	if ((data_IR[i]<80))
		{	filter[i] = 1;
			if(!detect)
			{	detect = 1;
				ObjDeg = i;
			}	
		}
		else 
		{	filter[i] = 0;
			if(detect)
			{	objects[acnt].offset = i - ObjDeg;
				objects[acnt].center = i - ((objects[acnt].offset)/2);
				objects[acnt].distance = (data_ping[objects[acnt].offset]+data_ping2[objects[acnt].offset])/2; //average the distance, for better accuracy
				objects[acnt].width = objects[acnt].distance * 2.0 * tan((double) (objects[acnt].offset * 3.1415926) / 360.0);
				acnt++;
			}
			detect = 0;
		}
	}
}

int object_process() // finds smallest object and measures its distance from the ping sensor
{	int small_object = 180;
	double location = -1;
	
	for(int i = 0;i<acnt;i++)
	{	if(small_object > (objects[acnt].width))
		{	small_object = (objects[acnt].width);
			location = (objects[acnt].center);
		}
	}

	USART_Transmit('D');
	for(int j=0;j<181;j++)
	{	sprintf(TX_string,"%d", data_IR[j]);
		uprint(TX_string);
	}
	USART_Transmit(0);
	return location;
}


void handle_cmd(oi_t*sensor_data)
{	unsigned char cmd = USART_Receive();	
	USART_Transmit(cmd);
	USART_Transmit(NL);
	USART_Transmit(CR);
	
	switch (cmd)
	{	case 'F':
		move_forward(sensor_data,50);
		break;
		case 'B':
		move_backward(sensor_data,50);
		break;
		case 'R':
		turn_clockwise(sensor_data,5);
		break;
		case 'L':
		turn_counterclockwise(sensor_data,5);
		break;
		case 'P':
		env_scan();
		analyze_env();
		object_process();
		break;
		case 'J':
		finish_song_light();
		break;
		case 'S'://standby
		break;
		case 'O'://object array data
			short_ping();
		break;
		case 'Z'://object array data
			cmd = 0;
			while(cmd != 'Z')
			{	cmd = USART_Receive();
				USART_Transmit(cmd);
				USART_Transmit(NL);
				USART_Transmit(CR);
			}
		break;
		
	}
}

unsigned int sensor_check(oi_t*sensor_data)
{	unsigned int sensors = 0; 
	/* bits - if a bit is 1 it signals danger
		0: Bumper Left
		1: Bumper Right 
		2: Wheel drop Left
		3: Wheel drop caster
		4: Wheel drop Right 
		5: Cliff Left
		6: Cliff Front-Left
		7: Cliff Front-right
		8: Cliff Right
	*/
	if (sensor_data->bumper_left)
	{	sensors |= (1<<0);
	}
	
	if (sensor_data->bumper_right)
	{	sensors |= (1<<1);
	}
	
	if (sensor_data->wheeldrop_left)
	{	sensors |= (1<<2);
	}
	
	if (sensor_data->wheeldrop_caster)
	{	sensors |= (1<<3);
	}
	
	if (sensor_data->wheeldrop_right)
	{	sensors |= (1<<4);
	}
	
	if (((sensor_data->cliff_left_signal) < (CLIFF))||(sensor_data->cliff_left_signal > (WHITE)))
	{	sensors |= (1<<5);
	}
	
	if ((sensor_data->cliff_frontleft_signal < CLIFF)||(sensor_data->cliff_frontleft_signal > WHITE))
	{	sensors |= (1<<6);
	}
	
	if ((sensor_data->cliff_frontright_signal < CLIFF)||(sensor_data->cliff_frontright_signal > WHITE))
	{	sensors |= (1<<7);
	}
	
	if ((sensor_data->cliff_right_signal < CLIFF)||(sensor_data->cliff_right_signal > WHITE))
	{	sensors |= (1<<8);
	}
	
	return sensors;
}

// Global variables used for interrupt driven delay functions
volatile unsigned int timer2_tick;
void timer2_start(char unit);
void timer2_stop();

// Blocks for a specified number of milliseconds
void wait_ms(unsigned int time_val) {
	//Seting OC value for time requested
	//Clock is 16 MHz. At a prescaler of 64, 250 timer ticks = 1ms.
	OCR2=250; 				
	timer2_tick=0;
	timer2_start(0);
	while(timer2_tick < time_val);//Waiting for time

	timer2_stop();
}

// Blocks for a specified number of microseconds
void wait_us(unsigned int time_val) {
	OCR2=16;//Seting OC value for time requested. Clock is 16 MHz. At a prescaler of 1, 16 timer ticks = 1us.
	timer2_tick=0;
	timer2_start(1);
	while(timer2_tick < time_val);//Waiting for time
	timer2_stop();
}

// Start timer2
void timer2_start(char unit) {
	timer2_tick=0;
	if ( unit == 0 ) { 		//unit = 0 is for slow 
        TCCR2=0b00001011;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 64
        TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	if (unit == 1) { 		//unit = 1 is for fast
		TCCR2=0b00001001;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 1
		TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	if (unit == 8) { 		//unit = 1 is for fast
        TCCR2=0b00001010;	//WGM:CTC, COM:OC2 disconnected,pre_scaler = 8
        TIMSK|=0b10000000;	//Enabling O.C. Interrupt for Timer2
	}
	sei();
}

// Stop timer2
void timer2_stop() {
	TIMSK&=0b01111111;		//Disabling O.C. Interrupt for Timer2
	TCCR2&=0b01111111;		//Clearing O.C. settings
}

// Interrupt handler (runs every 1 ms or 1 us, depending on the original call)
ISR (TIMER2_COMP_vect) 
{	timer2_tick++;
}

void oi_update_sensor(oi_t *self) 
{	int i;

	// Clear the receive buffer
	while (UCSR1A & (1 << RXC))
	i = UDR1;

	// Query a list of sensor values
	oi_byte_tx(OI_OPCODE_SENSORS);
	// Send the sensor packet ID
	oi_byte_tx(7);
	
	//wait_us(30);//waits are to help with potential USART errors
	
	// Read all the sensor data
	self->bumper_right = oi_byte_rx();
	
	oi_byte_tx(OI_OPCODE_SENSORS);
	oi_byte_tx(2);
	char sensor[15];
	for (i = 0; i < 6; i++) {
		// read each sensor byte
		(sensor[i]) = oi_byte_rx();
	}
	//wait_us(30);
	self->distance = (sensor[2] << 8) + sensor[3];
	self->angle = (sensor[4] << 8) + sensor[5];
	oi_byte_tx(OI_OPCODE_SENSORS);
	oi_byte_tx(4);
	for (i = 0; i < 14; i++) 
	{	// read each sensor byte
		(sensor[i]) = oi_byte_rx();
	}
	//wait_us(30);
	/*sensor = (char *) self;*/
	self->wall_signal              = (sensor[0] << 8) + sensor[1];
	self->cliff_left_signal        = (sensor[2] << 8) + sensor[3];
	self->cliff_frontleft_signal   = (sensor[4] << 8) + sensor[5];
	self->cliff_frontright_signal  = (sensor[6] << 8) + sensor[7];
	self->cliff_right_signal       = (sensor[8] << 8) + sensor[9];
	// Fix byte ordering for multi-byte members of the struct
	
	//wait_ms(30); // reduces USART errors that occur when continuously transmitting/receiving
}

char ch;
void uprint(char string[50])
{	int ucnt = 0;
	while(ucnt<strlen(string))
	{	ch = string[ucnt];
		USART_Transmit(ch);
		ucnt++;
	}
}

void servtst()
{	Servo(0);
	wait_ms(5000);
	Servo(45);
	wait_ms(5000);
	Servo(90);
	wait_ms(5000);
	Servo(135);
	wait_ms(5000);
	Servo(180);
	wait_ms(5000);
}

void init_all(oi_t *self)
{	lcd_init();
	Servo_Init();
	Sonar_init();
	ADC_init();
	USART_Init(MYUBRR);
	oi_init(self);
	oi_init(self);
}

void short_ping()
{	for(int i=0;i<acnt;i++)
	{	sprintf(TX_string, "Array %d\n\r", acnt);
		uprint(TX_string);
		sprintf(TX_string, "deg loc dis len %d\n\r", acnt);
		uprint(TX_string);
		sprintf(TX_string, "%3d %3d %3d %3.3f\n\r", objects[acnt].offset,	objects[acnt].center,	objects[acnt].distance, objects[acnt].width);
		uprint(TX_string);
	}
}