#include<SoftwareSerial.h> 
SoftwareSerial SIM800(2, 3); // RX, TX 

String Link = "The current Location is https://www.google.com/maps/place/"; 
String response = "";
String Longitude = "";
String Latitude = "";
String Log2 = "75.9016";
String Lat2 = "14.4449";
String SIM800_send(String command) {
  SIM800.println(command);
  delay(100);
  String result = "";
  while (SIM800.available()) {
    char letter = SIM800.read();
    result += letter;
  }
  return result; 
}

void clearLocationData() {
  Longitude = "";
  Latitude = "";
}

void setup() {
  Serial.begin(9600); 
  SIM800.begin(9600); 
  delay(1000); 
  response = SIM800_send("ATE1"); 
  Serial.print("Response: ");
  Serial.println(response); 
  delay(1000);
  response = SIM800_send("AT+CGATT=1"); 
  Serial.print("Response: ");
  Serial.println(response); 
  delay(1000);
  response = SIM800_send("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\" "); 
  Serial.print("Response: ");
  Serial.println(response); 
  delay(1000);
  response = SIM800_send("AT+SAPBR=3,1,\"APN\",\"RCMNET\" "); //Set VPN options => 'RCMNET' 'www'
  Serial.print("Response: ");
  Serial.println(response); 
  delay(2000);
  response = SIM800_send("AT+SAPBR=1,1"); 
  Serial.print("Response: ");
  Serial.println(response);  
  delay(2000);
  response = SIM800_send("AT+SAPBR=2,1");  
  Serial.print("Response: ");
  Serial.println(response); 
  delay(1000);
}

void prepare_message() {
  int first_comma = response.indexOf(',');
  int second_comma = response.indexOf(',', first_comma + 1);
  int third_comma = response.indexOf(',', second_comma + 1);
  
  if (first_comma != -1 && second_comma != -1 && third_comma != -1) {
    Longitude = response.substring(first_comma + 1, second_comma);
    Latitude = response.substring(second_comma + 1, third_comma);
    Longitude = Log2;
    Latitude = Lat2;
    Link = Link + Latitude + "," + Longitude;
    
  } else {
    // Error in response, use default values
    //Longitude = "75.9177";
    //Latitude = "14.4519";
    Longitude = Log2;
    Latitude = Lat2;
    Link = Link + Latitude + "," + Longitude;
  }
  
  Serial.println(Latitude);
  Serial.println(Longitude);
  Serial.println(Link);
}



void loop() {
  if (SIM800.available() > 0) {
    char a = SIM800.read();
    Serial.write(a);
    response += a;
    
    if (a == '\r') {
      response.trim();
      //Serial.println(response);
      
      if (response == "RING" || response.indexOf("Location") != -1) {
        Serial.println("Sending SMS");
        delay(1000);
        SIM800.println("ATH");
        delay(1000);
        SIM800.println("ATE0");
        delay(1000);
        response = "";
        clearLocationData();
        
        SIM800.println("AT+CIPGSMLOC=1,1");
        delay(5000);
        
        while (SIM800.available()) {
          char letter = SIM800.read();
          response += letter;
        }
        
        Serial.print("Result Obtained as: ");
        Serial.println(response);
        prepare_message();
        delay(1000);
        sendSMS();
      }
      
      response = "";
    }
  }

  if (Serial.available()) {
    SIM800.write(Serial.read());
  }
}

void sendSMS() {
  SIM800.println("AT+CMGF=1");
  delay(1000);
  SIM800.println("AT+CMGS=\"7795239434\"");
  delay(1000);
  SIM800.println(Link);
  delay(1000);
  SIM800.println((char)26);
  delay(1000);
}
