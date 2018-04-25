///////////////////////////////////////////////////////////////////////////////////////
//Terms of use
///////////////////////////////////////////////////////////////////////////////////////
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
///////////////////////////////////////////////////////////////////////////////////////
//Safety note
///////////////////////////////////////////////////////////////////////////////////////
//Always remove the propellers and stay away from the motors unless you 
//are 100% certain of what you are doing.
///////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>               //Include the Wire.h library so we can communicate with the gyro
#include <EEPROM.h>             //Include the EEPROM.h library so we can store information onto the EEPROM

//Declaring Global Variables
byte last_channel_1, last_channel_2, last_channel_3, last_channel_4, last_channel_5;
byte lowByte, highByte, type, gyro_address, error, clockspeed_ok;
byte channel_1_assign, channel_2_assign, channel_3_assign, channel_4_assign, channel_5_assign;
byte roll_axis, pitch_axis, yaw_axis;
byte receiver_check_byte, gyro_check_byte;
volatile int receiver_input_channel_1, receiver_input_channel_2, receiver_input_channel_3, receiver_input_channel_4, receiver_input_channel_5;
int center_channel_1, center_channel_2, center_channel_3, center_channel_4, center_channel_5;
int high_channel_1, high_channel_2, high_channel_3, high_channel_4, high_channel_5;
int low_channel_1, low_channel_2, low_channel_3, low_channel_4, low_channel_5;
int address, cal_int;
unsigned long timer, timer_1, timer_2, timer_3, timer_4, timer_5, current_time;
float gyro_pitch, gyro_roll, gyro_yaw;
float gyro_roll_cal, gyro_pitch_cal, gyro_yaw_cal;


//Setup routine
void setup(){
  pinMode(12, OUTPUT);
  //Arduino (Atmega) pins default to inputs, so they don't need to be explicitly declared as inputs
  
  //use PCINT8,9,10,11 - pin A0 A1 A2 A3
  PCICR |= (1 << PCIE1);    // set PCIE1 to enable PCMSK1 scan
  PCMSK1 |= (1 << PCINT8);  // set PCINT8 (digital input A0) to trigger an interrupt on state change
  PCMSK1 |= (1 << PCINT9);  // set PCINT9 (digital input A1)to trigger an interrupt on state change
  PCMSK1 |= (1 << PCINT10);  // set PCINT10 (digital input A2)to trigger an interrupt on state change
  PCMSK1 |= (1 << PCINT11);  // set PCINT11 (digital input A3)to trigger an interrupt on state change
  
  //use PCINT0 - pin 8 for AUX channel
  PCICR |= (1 << PCIE0);    // set PCIE0 to enable PCMSK0 scan
  PCMSK0 |= (1 << PCINT0);  // set PCINT0 (digital input 8)to trigger an interrupt on state change channel 5-Hov Throt

  //use pin 9,10,11 for sonar
  // 9 for trig, 10 for echo, 11 for led
  DDRB |= B00001010;        //Configure digital port 9,11 as output.
  PCICR |= (1 << PCIE0);    //Set PCIE0 to enable PCMSK0 scan.
  PCMSK0 |= (1 << PCINT2);  //Set PCINT0 (digital input 10) to trigger an interrupt on state change.

  Wire.begin();             //Start the I2C as master
  Serial.begin(57600);      //Start the serial connetion @ 57600bps
  delay(250);               //Give the gyro time to start 

  //Show the YMFC-3D V2 intro
  intro();
  
  Serial.println(F(""));
  Serial.println(F("==================================================="));
  Serial.println(F("System check"));
  Serial.println(F("==================================================="));
  Serial.println(F("Checking I2C clock speed."));

  Wire.setClock(400000);
  TWBR = 12;                      //Set the I2C clock speed to 400kHz for communicate with MPU-6050
  
  #if F_CPU == 16000000L          //If the clock speed is 16MHz include the next code line when compiling
    clockspeed_ok = 1;            //Set clockspeed_ok to 1
  #endif                          //End of if statement

  if(TWBR == 12 && clockspeed_ok){
    Serial.println(F("I2C clock speed is correctly set to 400kHz."));
  }
  else{
    Serial.println(F("I2C clock speed is not set to 400kHz. (ERROR 8)"));
    error = 1;
  }
  

  if(error == 0){
    Serial.println(F(""));
    Serial.println(F("==================================================="));
    Serial.println(F("Transmitter setup"));
    Serial.println(F("==================================================="));
    Serial.print(F("Checking for valid receiver signals."));
    //Wait 10 seconds until all receiver inputs are valid
    wait_for_receiver();
    Serial.println(F(""));
  }
  //Quit the program in case of an error
  if(error == 0){
    delay(2000);
    Serial.println(F("Place all sticks and subtrims in the center position within 5 seconds."));
    for(int i = 4;i > 0;i--){
      delay(1000);
      Serial.print(i);
      Serial.print(" ");
    }
    Serial.println(" ");
  }

  delay(1000);
}

//Main program
void loop(){

    //Store the central stick positions
    // center_channel_1 = receiver_input_channel_1;
    // center_channel_2 = receiver_input_channel_2;
    // center_channel_3 = receiver_input_channel_3;
    // center_channel_4 = receiver_input_channel_4;
    // center_channel_5 = receiver_input_channel_5;
    // Serial.println(F(""));
    // Serial.println(F("Center positions stored."));
//    Serial.print(F("Digital input A0 = "));
//    Serial.println(receiver_input_channel_1);
//    Serial.print(F("Digital input A1 = "));
//    Serial.println(receiver_input_channel_2);
//    Serial.print(F("Digital input A2 = "));
//    Serial.println(receiver_input_channel_3);
//    Serial.print(F("Digital input A3 = "));
//    Serial.println(receiver_input_channel_4);
    Serial.print(F("Digital input 8 = "));
    Serial.println(receiver_input_channel_5);
    
    

  // }
  // if(error == 0){  
  //   Serial.println(F("Move the throttle stick to full throttle and back to center"));
  //   //Check for throttle movement
  //   check_receiver_inputs(3);
  //   Serial.print(F("Throttle is connected to digital input A"));
  //   Serial.println((channel_3_assign & 0b00000111) - 1);
  //   if(channel_3_assign & 0b10000000)Serial.println(F("Channel inverted = yes"));
  //   else Serial.println(F("Channel inverted = no"));
  //   wait_sticks_zero();
    
  //   Serial.println(F(""));
  //   Serial.println(F(""));
  //   Serial.println(F("Move the roll stick to simulate left wing up and back to center"));
  //   //Check for throttle movement
  //   check_receiver_inputs(1);
  //   Serial.print(F("Roll is connected to digital input A"));
  //   Serial.println((channel_1_assign & 0b00000111) - 1);
  //   if(channel_1_assign & 0b10000000)Serial.println(F("Channel inverted = yes"));
  //   else Serial.println(F("Channel inverted = no"));
  //   wait_sticks_zero();
  // }
  // if(error == 0){
  //   Serial.println(F(""));
  //   Serial.println(F(""));
  //   Serial.println(F("Move the pitch stick to simulate nose up and back to center"));
  //   //Check for throttle movement
  //   check_receiver_inputs(2);
  //   Serial.print(F("Pitch is connected to digital input A"));
  //   Serial.println((channel_2_assign & 0b00000111) - 1);
  //   if(channel_2_assign & 0b10000000)Serial.println(F("Channel inverted = yes"));
  //   else Serial.println(F("Channel inverted = no"));
  //   wait_sticks_zero();
  // }
  // if(error == 0){
  //   Serial.println(F(""));
  //   Serial.println(F(""));
  //   Serial.println(F("Move the yaw stick to simulate nose right and back to center"));
  //   //Check for throttle movement
  //   check_receiver_inputs(4);
  //   Serial.print(F("Yaw is connected to digital input A"));
  //   Serial.println((channel_4_assign & 0b00000111) - 1);
  //   if(channel_4_assign & 0b10000000)Serial.println(F("Channel inverted = yes"));
  //   else Serial.println(F("Channel inverted = no"));
  //   wait_sticks_zero();
  // }
  // if(error == 0){
  //   Serial.println(F(""));
  //   Serial.println(F(""));
  //   Serial.println(F("Move the AUX to max and back to center"));
  //   //Check for AUX movement
  //   check_receiver_inputs(5);
  //   Serial.print(F("AUX is connected to digital input "));
  //   Serial.println((channel_5_assign & 0b00000111) + 3);
  //   if(channel_5_assign & 0b10000000)Serial.println(F("Channel inverted = yes"));
  //   else Serial.println(F("Channel inverted = no"));
  //   wait_sticks_zero();
  // }
  // if(error == 0){
  //   Serial.println(F(""));
  //   Serial.println(F(""));
  //   Serial.println(F("Gently move all the sticks simultaneously to their extends"));
  //   Serial.println(F("When ready put the sticks back in their center positions"));
  //   //Register the min and max values of the receiver channels
  //   register_min_max();
  //   Serial.println(F(""));
  //   Serial.println(F(""));
  //   Serial.println(F("High, low and center values found during setup"));
  //   Serial.print(F("Digital input A0 values:"));
  //   Serial.print(low_channel_1);
  //   Serial.print(F(" - "));
  //   Serial.print(center_channel_1);
  //   Serial.print(F(" - "));
  //   Serial.println(high_channel_1);
  //   Serial.print(F("Digital input A1 values:"));
  //   Serial.print(low_channel_2);
  //   Serial.print(F(" - "));
  //   Serial.print(center_channel_2);
  //   Serial.print(F(" - "));
  //   Serial.println(high_channel_2);
  //   Serial.print(F("Digital input A2 values:"));
  //   Serial.print(low_channel_3);
  //   Serial.print(F(" - "));
  //   Serial.print(center_channel_3);
  //   Serial.print(F(" - "));
  //   Serial.println(high_channel_3);
  //   Serial.print(F("Digital input A3 values:"));
  //   Serial.print(low_channel_4);
  //   Serial.print(F(" - "));
  //   Serial.print(center_channel_4);
  //   Serial.print(F(" - "));
  //   Serial.println(high_channel_4);
  //   Serial.print(F("Digital input A4 values:"));
  //   Serial.print(low_channel_5);
  //   Serial.print(F(" - "));
  //   Serial.print(center_channel_5);
  //   Serial.print(F(" - "));
  //   Serial.println(high_channel_5);
  //   Serial.println(F("Move stick 'nose up' and back to center to continue"));
  //   check_to_continue();
  // }
  // while(1);
}

//Search for the gyro and check the Who_am_I register
byte search_gyro(int gyro_address, int who_am_i){
  Wire.beginTransmission(gyro_address);
  Wire.write(who_am_i);
  Wire.endTransmission();
  Wire.requestFrom(gyro_address, 1);
  timer = millis() + 100;
  while(Wire.available() < 1 && timer > millis());
  lowByte = Wire.read();
  address = gyro_address;
  return lowByte;
}

//Check if a receiver input value is changing within 30 seconds
void check_receiver_inputs(byte movement){
  byte trigger = 0;
  int pulse_length;
  timer = millis() + 30000;
  while(timer > millis() && trigger == 0){
    delay(250);
    if(receiver_input_channel_1 > 1750 || receiver_input_channel_1 < 1250){
      trigger = 1;
      receiver_check_byte |= 0b00000001;
      pulse_length = receiver_input_channel_1;
    }
    if(receiver_input_channel_2 > 1750 || receiver_input_channel_2 < 1250){
      trigger = 2;
      receiver_check_byte |= 0b00000010;
      pulse_length = receiver_input_channel_2;
    }
    if(receiver_input_channel_3 > 1750 || receiver_input_channel_3 < 1250){
      trigger = 3;
      receiver_check_byte |= 0b00000100;
      pulse_length = receiver_input_channel_3;
    }
    if(receiver_input_channel_4 > 1750 || receiver_input_channel_4 < 1250){
      trigger = 4;
      receiver_check_byte |= 0b00001000;
      pulse_length = receiver_input_channel_4;
    }
    if(receiver_input_channel_5 > 1750 || receiver_input_channel_5 < 1250){
      trigger = 5;
      receiver_check_byte |= 0b00010000;
      pulse_length = receiver_input_channel_5;
    } 
  }
  if(trigger == 0){
    error = 1;
    Serial.println(F("No stick movement detected in the last 30 seconds!!! (ERROR 2)"));
  }
  //Assign the stick to the function.
  else{
    if(movement == 3){
      channel_3_assign = trigger;
      if(pulse_length < 1250)channel_3_assign += 0b10000000;
    }
    if(movement == 1){
      channel_1_assign = trigger;
      if(pulse_length < 1250)channel_1_assign += 0b10000000;
    }
    if(movement == 2){
      channel_2_assign = trigger;
      if(pulse_length < 1250)channel_2_assign += 0b10000000;
    }
    if(movement == 4){
      channel_4_assign = trigger;
      if(pulse_length < 1250)channel_4_assign += 0b10000000;
    }
    if(movement == 5){
      channel_5_assign = trigger;
      if(pulse_length < 1250)channel_5_assign += 0b10000000;
    }
  }
}

void check_to_continue(){
  byte continue_byte = 0;
  while(continue_byte == 0){
    if(channel_2_assign == 0b00000001 && receiver_input_channel_1 > center_channel_1 + 150)continue_byte = 1;
    if(channel_2_assign == 0b10000001 && receiver_input_channel_1 < center_channel_1 - 150)continue_byte = 1;
    if(channel_2_assign == 0b00000010 && receiver_input_channel_2 > center_channel_2 + 150)continue_byte = 1;
    if(channel_2_assign == 0b10000010 && receiver_input_channel_2 < center_channel_2 - 150)continue_byte = 1;
    if(channel_2_assign == 0b00000011 && receiver_input_channel_3 > center_channel_3 + 150)continue_byte = 1;
    if(channel_2_assign == 0b10000011 && receiver_input_channel_3 < center_channel_3 - 150)continue_byte = 1;
    if(channel_2_assign == 0b00000100 && receiver_input_channel_4 > center_channel_4 + 150)continue_byte = 1;
    if(channel_2_assign == 0b10000100 && receiver_input_channel_4 < center_channel_4 - 150)continue_byte = 1;
    if(channel_2_assign == 0b00000101 && receiver_input_channel_5 > center_channel_5 + 150)continue_byte = 1;
    if(channel_2_assign == 0b10000101 && receiver_input_channel_5 < center_channel_5 - 150)continue_byte = 1;
    delay(100);
  }
  wait_sticks_zero();
}

//Check if the transmitter sticks are in the neutral position
void wait_sticks_zero(){
  byte zero = 0;
  while(zero < 31){
    if(receiver_input_channel_1 < center_channel_1 + 20 && receiver_input_channel_1 > center_channel_1 - 20)zero |= 0b00000001;
    if(receiver_input_channel_2 < center_channel_2 + 20 && receiver_input_channel_2 > center_channel_2 - 20)zero |= 0b00000010;
    if(receiver_input_channel_3 < center_channel_3 + 20 && receiver_input_channel_3 > center_channel_3 - 20)zero |= 0b00000100;
    if(receiver_input_channel_4 < center_channel_4 + 20 && receiver_input_channel_4 > center_channel_4 - 20)zero |= 0b00001000;
    if(receiver_input_channel_5 < center_channel_5 + 20 && receiver_input_channel_5 > center_channel_5 - 20)zero |= 0b00010000;
    delay(100);
  }
}

//Checck if the receiver values are valid within 10 seconds
void wait_for_receiver(){
  byte zero = 0;
  timer = millis() + 10000;
  while(timer > millis() && zero < 31){
    if(receiver_input_channel_1 < 2100 && receiver_input_channel_1 > 900)zero |= 0b00000001;
    if(receiver_input_channel_2 < 2100 && receiver_input_channel_2 > 900)zero |= 0b00000010;
    if(receiver_input_channel_3 < 2100 && receiver_input_channel_3 > 900)zero |= 0b00000100;
    if(receiver_input_channel_4 < 2100 && receiver_input_channel_4 > 900)zero |= 0b00001000;
    if(receiver_input_channel_5 < 2100 && receiver_input_channel_5 > 900)zero |= 0b00010000;
    delay(500);
    Serial.print(F("."));
  }
  if(zero == 0){
    error = 1;
    Serial.println(F("."));
    Serial.println(F("No valid receiver signals found!!! (ERROR 1)"));
  }
  else Serial.println(F(" OK"));
}

//Register the min and max receiver values and exit when the sticks are back in the neutral position
void register_min_max(){
  byte zero = 0;
  low_channel_1 = receiver_input_channel_1;
  low_channel_2 = receiver_input_channel_2;
  low_channel_3 = receiver_input_channel_3;
  low_channel_4 = receiver_input_channel_4;
  low_channel_5 = receiver_input_channel_5;
  while(receiver_input_channel_1 < center_channel_1 + 20 && receiver_input_channel_1 > center_channel_1 - 20)delay(250);
  Serial.println(F("Measuring endpoints...."));
  while(zero < 31){
    if(receiver_input_channel_1 < center_channel_1 + 20 && receiver_input_channel_1 > center_channel_1 - 20)zero |= 0b00000001;
    if(receiver_input_channel_2 < center_channel_2 + 20 && receiver_input_channel_2 > center_channel_2 - 20)zero |= 0b00000010;
    if(receiver_input_channel_3 < center_channel_3 + 20 && receiver_input_channel_3 > center_channel_3 - 20)zero |= 0b00000100;
    if(receiver_input_channel_4 < center_channel_4 + 20 && receiver_input_channel_4 > center_channel_4 - 20)zero |= 0b00001000;
    if(receiver_input_channel_5 < center_channel_5 + 20 && receiver_input_channel_5 > center_channel_5 - 20)zero |= 0b00010000;

    if(receiver_input_channel_1 < low_channel_1)low_channel_1 = receiver_input_channel_1;
    if(receiver_input_channel_2 < low_channel_2)low_channel_2 = receiver_input_channel_2;
    if(receiver_input_channel_3 < low_channel_3)low_channel_3 = receiver_input_channel_3;
    if(receiver_input_channel_4 < low_channel_4)low_channel_4 = receiver_input_channel_4;
    if(receiver_input_channel_5 < low_channel_5)low_channel_5 = receiver_input_channel_5;
    
    if(receiver_input_channel_1 > high_channel_1)high_channel_1 = receiver_input_channel_1;
    if(receiver_input_channel_2 > high_channel_2)high_channel_2 = receiver_input_channel_2;
    if(receiver_input_channel_3 > high_channel_3)high_channel_3 = receiver_input_channel_3;
    if(receiver_input_channel_4 > high_channel_4)high_channel_4 = receiver_input_channel_4;
    if(receiver_input_channel_5 > high_channel_5)high_channel_5 = receiver_input_channel_5;
    delay(100);
  }
}

//This routine is called every time input 8 changed state
ISR (PCINT0_vect) {
  current_time = micros();
  //Channel 5=========================================
  if(PINB & B00000001 ){                                       //Is input 8 high?
    if(last_channel_5 == 0){                                   //Input 8 changed from 0 to 1
      last_channel_5 = 1;                                       //Remember current input state
      timer_5 = current_time;                                  //Set timer_4 to current_time
    }
  }
  else if(last_channel_5 == 1){                                //Input 8 is not high and changed from 1 to 0
    last_channel_5 = 0;                                        //Remember current input state
    receiver_input_channel_5 = current_time - timer_5;         //Channel 5 is current_time - timer_5
  }
}

//This routine is called every time input A0, A1, A2 or A3 changed state
ISR(PCINT1_vect){
  current_time = micros();
  //Channel 1=========================================
  if(PINC & B00000001){                                        //Is input A0 high?
    if(last_channel_1 == 0){                                   //Input A0 changed from 0 to 1
      last_channel_1 = 1;                                      //Remember current input state
      timer_1 = current_time;                                  //Set timer_1 to current_time
    }
  }
  else if(last_channel_1 == 1){                                //Input A0 is not high and changed from 1 to 0
    last_channel_1 = 0;                                        //Remember current input state
    receiver_input_channel_1 = current_time - timer_1;         //Channel 1 is current_time - timer_1
  }
  //Channel 2=========================================
  if(PINC & B00000010){                                       //Is input A1 high?
    if(last_channel_2 == 0){                                   //Input A1 changed from 0 to 1
      last_channel_2 = 1;                                      //Remember current input state
      timer_2 = current_time;                                  //Set timer_2 to current_time
    }
  }
  else if(last_channel_2 == 1){                                //Input A1 is not high and changed from 1 to 0
    last_channel_2 = 0;                                        //Remember current input state
    receiver_input_channel_2 = current_time - timer_2;         //Channel 2 is current_time - timer_2
  }
  //Channel 3=========================================
  if(PINC & B00000100 ){                                       //Is input A2 high?
    if(last_channel_3 == 0){                                   //Input A2 changed from 0 to 1
      last_channel_3 = 1;                                      //Remember current input state
      timer_3 = current_time;                                  //Set timer_3 to current_time
    }
  }
  else if(last_channel_3 == 1){                                //Input A2 is not high and changed from 1 to 0
    last_channel_3 = 0;                                        //Remember current input state
    receiver_input_channel_3 = current_time - timer_3;         //Channel 3 is current_time - timer_3

  }
  //Channel 4=========================================
  if(PINC & B00001000 ){                                       //Is input A3 high?
    if(last_channel_4 == 0){                                   //Input A3 changed from 0 to 1
      last_channel_4 = 1;                                      //Remember current input state
      timer_4 = current_time;                                  //Set timer_4 to current_time
    }
  }
  else if(last_channel_4 == 1){                                //Input A3 is not high and changed from 1 to 0
    last_channel_4 = 0;                                        //Remember current input state
    receiver_input_channel_4 = current_time - timer_4;         //Channel 4 is current_time - timer_4
  }
}

//Intro subroutine
void intro(){
  Serial.println(F("==================================================="));
  delay(200);
  Serial.println(F(""));
  Serial.println(F("Your"));
  delay(200);
  Serial.println(F("  Multicopter"));
  delay(200);
  Serial.println(F("    Flight"));
  delay(200);
  Serial.println(F("      Controller"));
  delay(200);
  Serial.println(F(""));
  Serial.println(F("YMFC-AL Setup Program"));
  Serial.println(F(""));
  Serial.println(F("==================================================="));
  delay(200);
  Serial.println(F("For support and questions: www.brokking.net"));
  Serial.println(F(""));
  Serial.println(F("Have fun!"));
}
