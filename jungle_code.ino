//define output pins
#define BUZZ 0b00100000 // PA5
#define PORT_BUZZ PORTA
#define DDR_BUZZ DDRA

#define SERVO 0b00000100 //PB2
#define PORT_SERVO PORTB
#define DDR_SERVO DDRB

// define percentage probabiliteies for each case
#define ROKA 250 // 15
#define SMER 500 // 40
#define VSI 750 //40
//#define DEATH 5 //5

// define servo positions and step
#define SERVO_MAX_PWM 2500
#define SERVO_MIN_PWM 500
#define SERVO_STEP 0.9
#define SERVO_PULSES 1
#define CENTER_DEG 87
#define ROKA_DEG 147
#define SMER_DEG 120
#define VSI_DEG 57
#define DEATH_DEG 34

// define wait ime in seconds
#define DELAY_FACTOR 2
#define MIN_DELAY 90

#define BEEP_T1 500


float servo_pos;
const byte cases_pins[4] = {0b00001000, 0b00000100, 0b00000010, 0b00000001}; // PA0-3, pins for LEDs
const int cases_degs[4] = {ROKA_DEG, SMER_DEG, VSI_DEG, DEATH_DEG};

void setup()
{
  ACD_init();
  DDRA |= 0b00101111;
  DDRB |= SERVO;
  servo_to_deg(CENTER_DEG, 25);
}

void loop()
{
  int oneCase;
  for (int i=0; i<(MIN_DELAY); i++){
    delay(1000);
  }
  // delay((MIN_DELAY + (MAX_DELAY-MIN_DELAY)/1000*get_percent())*1000);
  oneCase = random_case();
  PORT_BUZZ |= BUZZ;
  delay(BEEP_T1);
  PORT_BUZZ &= ~BUZZ;
  delay(1000);
  move_servo(cases_degs[oneCase], SERVO_STEP, SERVO_PULSES);
  delay(500);
  PORTA |= cases_pins[oneCase];
  delay(5000);
  PORTA &= ~cases_pins[oneCase];
  move_servo(CENTER_DEG, SERVO_STEP, SERVO_PULSES);

  
  // int oneCase = random_case();
  // PORTA |= cases_pins[oneCase];
  // delay(100);
  // PORTA &= ~cases_pins[oneCase];
  // delay(100);

}


void move_servo(float deg, float step, int pulses){
  if (deg > servo_pos){
    while (servo_pos + step < deg){
      servo_to_deg(servo_pos + step, pulses);
    }
    servo_to_deg(deg, 10);
  }
  else{
    while (servo_pos - step > deg){
      servo_to_deg(servo_pos - step, pulses);
    }
    servo_to_deg(deg, 10);
  }

}

void servo_to_deg(float deg, int pulses){
  int pulseT = (deg*(SERVO_MAX_PWM-SERVO_MIN_PWM)/180)+SERVO_MIN_PWM; // calculate the pulse width
  for (int p = 0; p < pulses; p++){
    PORT_SERVO |= SERVO; //turn on pin
    delayMicroseconds(pulseT);
    PORT_SERVO &= ~SERVO; // turn off pin
    delay(20-int(SERVO_MAX_PWM/1000));
    delayMicroseconds(SERVO_MAX_PWM-pulseT);
  }

  servo_pos = deg;
}

void ACD_init()
{
  ADMUX |= 0b10000111; //define pin and vref, measuring at ADC7
  ADCSRA |= 0b10000101; //enable adc
  ADCSRB |= 0b00010000; //left adjust
}

uint16_t adc_read()
{
  ADCSRA |= 0b01000000; // start measurnig

  while (ADCSRA & 0b01000000); //while it is measuring
  // ADC = (ADCL | (ADCH << 8)); // combine both register for 10 bit accuracy
  return (ADCH);
}

unsigned int hash(unsigned int x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}


int get_percent(){
  unsigned int sum = 0b0;
  for (int i = 0; i < 16; i++){
    if (adc_read()>128){
      sum = sum + (0b1 << i);
    }
    delay(5+ sum%5);    
  }
  return hash(sum)%1000;
}

int random_case(){
  int sensorValue = get_percent();
  if ((sensorValue >= 0) && (sensorValue < ROKA)){
    sensorValue = 0;
  }
  else if ((sensorValue >= ROKA) && (sensorValue < SMER)){
    sensorValue = 1;
  }
  else if ((sensorValue >= SMER) && (sensorValue < VSI)){
    sensorValue = 2;
  }
  else{
    sensorValue = 3;
  }
  return sensorValue;
}

// int random_case_debug(){
//   int sensorValue = adc_read();
//   if ((sensorValue >= 0) && (sensorValue < 128)){
//     sensorValue = 0;
//   }
//   else{
//     sensorValue = 1;
//   }
//   return sensorValue;
// }