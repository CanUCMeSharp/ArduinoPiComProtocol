#include <Wire.h>
#include <TimeLib.h> // https://www.pjrc.com/teensy/td_libs_Time.html
#include <DS1307RTC.h> // https://www.pjrc.com/teensy/td_libs_DS1307RTC.html
#define GET_VAR_NAME(var) (#var)
#define SERIAL_TX_BUFFER_SIZE 64

const int baud = 19200;
unsigned long time = 0;

struct DataMessage {
  int temp, light, loudness, wind, wDirection, uv, o2, humidity, pressure, co2, co, no2, smog, ph1, ph2, ph3;
  //temp, light, loudness, wind, wDirection, uv, o2, humidity, pressure, co2, co, no2, smog
};
time_t getTimeUnix(){
  return RTC.get();
}
String intToJson(String name, int i){
    int index = name.lastIndexOf('.') + 1;
    int length = name.length();
    name = name.substring(index, length);
    return "\"" + name + "\":" + "\"" + i + "\"";
  }
  String longToJson(String name, time_t i){ 
    int index = name.lastIndexOf('.') + 1;
    int length = name.length();
    name = name.substring(index, length);
    return "\"" + name + "\":" + "\"" + i + "\"";
  }
  String stringToJson(String name, String i){ 
    int index = name.lastIndexOf('.') + 1;
    int length = name.length();
    name = name.substring(index, length);
    return "\"" + name + "\":" + "\"" + i + "\"";
  }

DataMessage d;
    void send(){//DataMessage d){
      long checksum = 0;
      int *values[] = { &d.temp, &d.light, &d.loudness, &d.wind, &d.wDirection, &d.uv, &d.o2, &d.humidity, &d.pressure, &d.co2, &d.co, &d.no2, &d.smog, &d.ph1, &d.ph2, &d.ph3};
      String names[] = {GET_VAR_NAME(d.temp), GET_VAR_NAME(d.light), GET_VAR_NAME(d.loudness),GET_VAR_NAME(d.wind), GET_VAR_NAME(d.wDirection), GET_VAR_NAME(d.uv), GET_VAR_NAME(d.o2), GET_VAR_NAME(d.humidity), GET_VAR_NAME(d.pressure), GET_VAR_NAME(d.co2), GET_VAR_NAME(d.co), GET_VAR_NAME(d.no2), GET_VAR_NAME(d.smog), GET_VAR_NAME(d.ph1),GET_VAR_NAME(d.ph2),GET_VAR_NAME(d.ph3)};
      Serial.print("{");
      Serial.print(intToJson("type", 0) + ", ");
      for(int i = 0; i < sizeof(values) / sizeof(values[0]); i++){
        Serial.print(intToJson(names[i], *values[i]) + ", ");
        checksum = (checksum + *values[i]); //% MOD_Check;
      }
      Serial.print(longToJson("time",time));
      Serial.print(",");
      Serial.print(longToJson("checksum", checksum));
      Serial.println("}");
      getReturn();
    }
    void error(String text){
      Serial.print("{");
      Serial.print(intToJson("Type", 1) + ", ");
      Serial.print(stringToJson("message", text) + ", ");
      Serial.print(longToJson("time", time) + "}\n");
      //getReturn();
    }
    void getReturn(){
            String incomingString = Serial.readStringUntil(0x0A);
            long number = (atol(incomingString.c_str()));
            time = number;
            if(number == 0){
              return;
            }
            else if(number == 1){
              send();
            }
            
            // Serial.println(incomingString);
    }
    
void setup() {
  Serial.begin(57600);
  d.temp = 5000;
  d.light = random(500);
  d.loudness = random(500);
  d.wind = random(500);
  d.wDirection = random(500);
  d.uv = random(500);
  d.o2 = random(500);
  d.humidity = random(500);
  d.pressure = random(500);
  d.co2 = random(500);
  d.co = random(500);
  d.no2 = random(500);
  d.smog = random(500);
  d.ph1 = 0;
  d.ph2 = 0;
  d.ph3 = 0;
}

void loop() {
    send();
    delay(1000);
}

