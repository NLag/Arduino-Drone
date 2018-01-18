void setup() {
  // put your setup code here, to run once:
  DDRD |= B10000000;
  DDRB |= B00000010;
  PCICR |= (1 << PCIE0);                                                    //Set PCIE0 to enable PCMSK0 scan.
  PCMSK0 |= (1 << PCINT0);                                                  //Set PCINT0 (digital input 8) to trigger an interrupt on state change.
}

void loop() {
  // put your main code here, to run repeatedly:

}

ISR(PCINT0_vect){
  
}
