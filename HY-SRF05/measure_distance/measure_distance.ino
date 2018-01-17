    const int trig = 8;     // chân trig của HC-SR04
    const int echo = 9;     // chân echo của HC-SR04
    const int led = 10;
    void setup()
    {
        Serial.begin(9600);     // giao tiếp Serial với baudrate 9600
        pinMode(trig,OUTPUT);   // chân trig sẽ phát tín hiệu
        pinMode(echo,INPUT);    // chân echo sẽ nhận tín hiệu
    }
     
    void loop()
    {
        unsigned long duration; // biến đo thời gian
        float distance;           // biến lưu khoảng cách
        
        /* Phát xung từ chân trig */
        digitalWrite(trig,0);   // tắt chân trig
        delayMicroseconds(2);
        digitalWrite(trig,1);   // phát xung từ chân trig
        delayMicroseconds(10);   // xung có độ dài 5 microSeconds
        digitalWrite(trig,0);   // tắt chân trig
        
        /* Tính toán thời gian */
        // Đo độ rộng xung HIGH ở chân echo. 
        duration = pulseIn(echo,HIGH,10000);
        // Tính khoảng cách đến vật.
        if (duration<=0) {
          Serial.println("Echo time oud!");  
        }
        else {
          distance = duration*0.34/2;
          
          /* In kết quả ra Serial Monitor */
          Serial.print(duration);
          Serial.print("micros ");
          Serial.print(distance);
          Serial.println("mm");  
        }
        if (distance < 1000 && distance >0) {
          digitalWrite(led,1);
        } else {
          digitalWrite(led,0);
        }
    }
