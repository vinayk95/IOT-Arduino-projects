// esp8266_test.ino
//
// Plot LM35 data on thingspeak.com using an Arduino and an ESP8266 WiFi
// module.
#include <SoftwareSerial.h>
#include <stdlib.h>

// LED
int ledPin = 13;
// LM35 analog input
int lm35Pin = A0;

// replace with your channel's thingspeak API key
String apiKey = "9IOFWL9G8D8AWVXZ";


// connect 10 to TX of Serial USB
// connect 11 to RX of serial USB
SoftwareSerial ser(2, 3); // RX, TX

// this runs once
void setup() {
// initialize the digital pin as an output.
pinMode(ledPin, OUTPUT);

// enable debug serial
Serial.begin(115200);
// enable software serial
ser.begin(115200);

// reset ESP8266
ser.println("AT+RST");
delay(1000);
ser.println("AT+CWMODE=3");
delay(1000);
ser.println("AT+CIPMODE=0");
delay(1000);
ser.println("AT+CIPMUX=1");
delay(1000);
}
// the loop
void loop() {

// blink LED on board
digitalWrite(ledPin, HIGH);
delay(200);
digitalWrite(ledPin, LOW);

// read the value from LM35.
// read 10 values for averaging.
int val = 0;
for(int i = 0; i < 10; i++) {
val += 5;
delay(500);
}

// convert to temp:
// temp value is in 0-1023 range
// LM35 outputs 10mV/degree C. ie, 1 Volt => 100 degrees C
// So Temp = (avg_val/1023)*5 Volts * 100 degrees/Volt
float temp = val*50.0f/1023.0f;

// convert to string
char buf[16];
String strTemp = dtostrf(temp, 4, 1, buf);

Serial.println(strTemp);

// TCP connection
String cmd = "AT+CIPSTART=4,\"TCP\",\"184.106.153.149\",80";
//String cmd = "AT+CIPSTART=4,\"TCP\",\"192.168.0.103\",8081";
ser.println(cmd);
delay(1000);

//if(ser.find("Error")){
//Serial.println("AT+CIPSTART error");
return;
}

// prepare GET string
String getStr = "GET /update?api_key=";
getStr += apiKey;
getStr +="&field2=";
getStr += String(strTemp);
getStr += "\r\n\r\n";

// send data length
cmd = "AT+CIPSEND=4,";
cmd+= String(getStr.length());
ser.println(cmd);

if(ser.find(">")){
    ser.print(getStr);
}
else{
    ser.println("AT+CIPCLOSE");
    // alert user
    Serial.println("AT+CIPCLOSE");
  }
    
  // thingspeak needs 15 sec delay between updates
  delay(16000);  
}
