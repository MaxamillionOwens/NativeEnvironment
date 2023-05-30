//Pin usage is 4, 3, A0, A1
    #include <DHT.h>;
    #include <SoftwareSerial.h>;
    #include <TinyGPS++.h>;
    #include <SPI.h>;
    #include <LoRa.h>;

//DHT
//Temperature range alert is any temperature above 60 C or 140 F
//Humidity range warning if it falls below 15%
    #define DHTPIN A0
    #define DHTTYPE DHT22
    DHT DHT(DHTPIN, DHTTYPE);
    int Humidity;
    int Temperature;

//PIR
    int PIR = A1;
    int motion = 0;
    int check = 0;

//GPS
//static const int TXPin = 4, RXPin = 3;
    static const int TXPin = 4, RXPin = 3;
    static const uint32_t GPSBaud = 9600;
    TinyGPSPlus gps;
    SoftwareSerial GPS_Mod(RXPin, TXPin);

/////////////////////////////////////////////////////////////////////////SETUP//////////////////////////////////////////////////////////////////////////////////////
void setup(){
//PIR
    pinMode(led, OUTPUT);     
    pinMode(PIR, INPUT);
//GPS
    GPS_Mod.begin(GPSBaud);
//DHT22
    DHT.begin();
//LoRa Shield/HAT
    while(!Serial);
    if(!LoRa.begin(433E6)){
      Serial.println("LoRa connection failed");
      while(1);
    }
    Serial.begin(9600);
}
 /////////////////////////////////////////////////////////////////////////LOOP//////////////////////////////////////////////////////////////////////////////////////
void loop(){
  //PIR
    motion = digitalRead(PIR); 
    if (motion == HIGH) {
	    digitalWrite(led, HIGH);
	    if (check == 0) {
	        Serial.println("Motion Detected");
	        check = 1;
	    }
    }
//DHT22
    Humidity = DHT.readHumidity();
    Temperature= DHT.readTemperature();
    if(Humidity >25 && Temperature <60){
      delay(5000);
    } else {
      if(Humidity <=25){
          LoRa.beginPacket();
          if(Temperature >= 60){
            LoRa.println("LOW HUMIDITY WARNING");
	          LoRa.print("Humidity: ");
	          LoRa.print(Humidity);
            LoRa.println("%");
            LoRa.println("WILDFIRE ALERT");
	          LoRa.print("Temperature: ");
	          LoRa.print(Temperature);
	          LoRa.print(" Celsius, ");
	          Temperature = (Temperature * 9/5) + 32;
	          LoRa.print(Temperature);
	          LoRa.println(" Fahrenheit");
	          while (GPS_Mod.available() > 0){
		          if (gps.encode(GPS_Mod.read())){
			          location_GPS();
		          } 
	          }
          } else {
            LoRa.println("LOW HUMIDITY WARNING");
	          LoRa.print("Humidity: ");
	          LoRa.print(Humidity);
            LoRa.println("%");
            while (GPS_Mod.available() > 0){
		          if (gps.encode(GPS_Mod.read())){
			          location_GPS();
		          }  
	          }
            delay(5000);
          }
          LoRa.endPacket();
      } else if(Temperature >= 60){    
        LoRa.beginPacket();               ///////////Important due to if the sensor has dew or any form of liquid present that the heat hasn't evaporated
        LoRa.println("WILDFIRE ALERT");
	      LoRa.print("Temperature: ");
	      LoRa.print(Temperature);
	      LoRa.print(" Celsius, Temperature: ");
	      Temperature = (Temperature * 9/5) + 32;
	      LoRa.print(Temperature);
	      LoRa.println(" Fahrenheit");
	      while (GPS_Mod.available() > 0){
		      if (gps.encode(GPS_Mod.read())){
			      location_GPS();
		      }
	      }
        LoRa.endPacket();                                 
      }
    }
}


void location_GPS(){
  LoRa.print(F("Location: ")); 
	LoRa.print(gps.location.lat(), 4);
	LoRa.print(F(","));
	LoRa.println(gps.location.lng(), 4);
}