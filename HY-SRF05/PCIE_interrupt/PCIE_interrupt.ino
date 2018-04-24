unsigned long sonar_start = 0;
unsigned long current_time = 0;
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
  start_sonar();
}

void start_sonar() {
  //B11111111
  //B00000000
  PORTB &= B11111101;       //Set trig to low
  delayMicroseconds(2);
  PORTB |= B00000010;       //trig to high, start pulse
  delayMicroseconds(10);    //pluse 10microsec
  PORTB &= B11111101;       //Set trig to low
  // sonar_start = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

ISR(PCINT0_vect){
  current_time = micros();
  //Channel 10=========================================
  if(PINB & B00000100 ){                                       //Is input 10 high?
    if(start_timer == 0){                                    //Input 10 changed from 0 to 1
      start_timer = 1;                                       //Remember current input state
      sonar_start = current_time;                              //Set sonar_start to current_time
    }
  }
  else if(start_timer == 1){                                //Input 10 is not high and changed from 1 to 0
    start_timer = 0;                                        //Remember current input state
    duration = current_time - sonar_start;                  //Sonar duration is current_time - sonar_start
    if (duration < 0)
    {
      duration = 4294967295 + duration;
    }
    distance = (duration*3.4)/2;                            //Calculation distance
    if (distance < 1100 && distance >0) {
      PORTB |= B00001000;     //set led to high
    } else {
      PORTB &= B11110111;     //set led to low
    }
    Serial.println(distance);
    start_sonar();
  }
}
