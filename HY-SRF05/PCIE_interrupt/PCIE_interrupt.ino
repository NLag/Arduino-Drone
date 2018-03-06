unsigned long timing_start = 0;
unsigned long current = 0;
byte start_timer = 0;
unsigned long duration = 0;
double distance = 0;

void setup() {
  // put your setup code here, to run once:
  // trig - 9
  // echo - 10
  // led - 11
  Serial.begin(9600);
  DDRB |= B00001010;        //Configure digital port 9,11 as output.
  PCICR |= (1 << PCIE0);    //Set PCIE0 to enable PCMSK0 scan.
  PCMSK0 |= (1 << PCINT2);  //Set PCINT0 (digital input 10) to trigger an interrupt on state change.
  startsonar();
}

void startsonar() {
  //B11111111
  //B00000000
  PORTB &= B11111110;       //Set trig to low
  delayMicroseconds(2);
  PORTB |= B00000001;       //trig to high, start pulse
  delayMicroseconds(10);    //pluse 10microsec
  PORTB &= B11111110;       //Set trig to low
  timing_start = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

ISR(PCINT0_vect){
  current = micros();
  if ((start_timer == 0) && digitalRead(9)) {
    start_timer = 1;
    timing_start = current;
  } else if((start_timer == 1) && (digitalRead(9)==0)){
    start_timer = 0;
    if (current < timing_start) {
      duration = 4294967295 - timing_start;
      duration += current;
    } else {
      duration = current - timing_start;
    }
    distance = duration*0.34/2;
    Serial.print(duration);
    Serial.print("micros ");
    Serial.print(distance);
    Serial.print("mm ");
    Serial.println();
    if (distance < 1100 && distance >0) {
      PORTB |= B00000100;     //set led to high
    } else {
      PORTB &= B11111011;     //set led to low
    }
    startsonar();  
  } 
}
