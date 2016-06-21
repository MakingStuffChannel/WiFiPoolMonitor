#include <OneWire.h>

int DS18S20_Pin = 10; 
OneWire ds(DS18S20_Pin); 

float temperature = 0;
long lastMillis = 0;
long readTime = 1200000;


void setup(void) {
  float temperature = getTemp();
  
  Serial.begin(115200);
 
  delay(5000);  //wait for esp8266 to boot
  Serial.println("AT+CWMODE=3");  
  delay(1000);
  //Serial.println("AT+RST");    
  //delay(3000);
  Serial.println("AT+CIPMUX=1");
  delay(1000);
  Serial.println("AT+CWJAP=\"Your_Network_SSID\",\"Your_Network_Password\"");
  delay(10000);
}

void loop(void) {
  if(millis () > lastMillis)
  {
     lastMillis = millis() + readTime;
     temperature = getTemp();
     sendData(temperature);
  }
}

void sendData(float pTemp)
{
   Serial.println("AT+CIPSTART=4,\"TCP\",\"data.sparkfun.com\",80");  
   delay(1000);
   Serial.println("AT+CIPSEND=4,80");
   delay(1000);
   Serial.print("GET /input/Sparkfun_Public_Key?private_key=Sparkfun_Private_Key&temp_f=");
  Serial.println(((pTemp*1.8)+32));
   Serial.println("\r\n\r\n");
   delay(3000);
  Serial.println("AT+CIPCLOSE=4");
}

float getTemp(){
 

 byte data[12];
 byte addr[8];

 if ( !ds.search(addr)) {
   ds.reset_search();
   return -1000;
 }

 if ( OneWire::crc8( addr, 7) != addr[7]) {
   Serial.println("CRC is not valid!");
   return -1000;
 }

 if ( addr[0] != 0x10 && addr[0] != 0x28) {
   Serial.print("Device is not recognized");
   return -1000;
 }

 ds.reset();
 ds.select(addr);
 ds.write(0x44,1); 

 byte present = ds.reset();
 ds.select(addr);  
 ds.write(0xBE); 

 
 for (int i = 0; i < 9; i++) { // we need 9 bytes
  data[i] = ds.read();
 }
 
 ds.reset_search();
 
 byte MSB = data[1];
 byte LSB = data[0];

 float tempRead = ((MSB << 8) | LSB); 
 float TemperatureSum = tempRead / 16;
 
 return TemperatureSum;
 
}
