const int trig = 8;     // chân trig của HC-SR04
const int echo = 3;     // chân echo của HC-SR04
const int led = 10;
unsigned long timing_start = 0;
unsigned long duration = 0;
unsigned long current = 0;
double distance = 0;
byte start_timer = 0;

void setup() {
  Serial.begin(9600);
  pinMode(trig,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(echo,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(echo), checksonar,CHANGE); //check tin hieu tu echo
  startsonar();
}

void loop() {
}

void startsonar() {
  digitalWrite(trig,0);   // tắt chân trig
  delayMicroseconds(2);
  digitalWrite(trig,1);   // phát xung từ chân trig
  delayMicroseconds(10);   // xung có độ dài 10 microSeconds
  digitalWrite(trig,0);   // tắt chân trig
  timing_start = micros();  
}

void checksonar() {
  current = micros();
  if ((start_timer == 0) && digitalRead(echo)) {
    start_timer = 1;
    timing_start = current;
  } else if((start_timer == 1) && (digitalRead(echo)==0)){
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
      digitalWrite(led,1);
    } else {
      digitalWrite(led,0);
    }
    startsonar();  
  } 
}


