#include <avr/io.h>
#include <avr/interrupt.h>

// LDR related
const int sensor_pin = 0;
int sensor_value;
float v;
float R;
float L;
const float b = 5.8515;
const float m = -0.9355;
const float Raux = 10000;

// LED related
const int ledPin = 3;

// I/O
int value = 50;
int i;
char buffer[10];

// Interupts
volatile byte flag=0;
const int pin_switch=2;
volatile byte presence=1; //Simulates presence sensor 


// Interupt service routine

ISR(TIMER1_COMPA_vect){

  // Check interrupt?
  flag=1;  
}

void switch_isr(){

  presence = !presence;
}

void setup() {

  
  cli();
  // Clear registers
  TCCR1A = 0; // Operating mode - CTC
  TCCR1B = 0; // Prescale = 1 - 62.5ns resolution
  TCNT1 = 0; // Reset timer count
  
  // Timer limits on this count
  OCR1A = 5*16-1; // 5*16*62.5ns = 5000 ns

  TCCR1B |= (1<<WGM12);
  TCCR1B |= (1<<CS10)|(0<<CS12)|(0<CS11); // prescaler 1
  TIMSK1 |= (1<<OCIE1A); 

  // Contact switch
  pinMode(pin_switch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(pin_switch), switch_isr, FALLING);
  
  sei();
  Serial.begin(230400);
}

void loop() {


  // Read from Serial target LUX value
  i=0;
  while(Serial.available()){
    buffer[i]=Serial.read(); 
    i++;
    delay(5);
  }
  if(i>0){
    buffer[i]=0;
    value = atoi(buffer);  
    value = (value>255) ? 255 : value;
    value = (value<0) ? 0 : value;
  }

  // Compute luminosity
  sensor_value = analogRead(sensor_pin);
  v = sensor_value*5.0/1.0230;
  R = (5-v/1000.0)/(v/1000.0/Raux);
  L = pow(10, (log10(R)-b)/m );
  Serial.println(L);


  // Control
  if(flag){



    
    analogWrite(ledPin, presence*value);
    flag=0;
  }
  
  
}
