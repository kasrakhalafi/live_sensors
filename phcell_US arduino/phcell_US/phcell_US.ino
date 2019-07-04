#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <SimpleDHT.h>



int pinDHT11 = 14; //D4
int photocell = A0; //D3
//ultra sonic
const int trigPin = 5;  //D1
const int echoPin = 4;  //D2


SimpleDHT11 dht11;

int ledPin=5;

void setup() {

  byte temperature = 0;
  byte humidity = 0;
  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input



  Serial.begin(115200);      
  //Serial connection
  WiFi.begin("Amin", "00000000");   //WiFi connection

  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion

    delay(500);
    Serial.println("Waiting for connection");

  }

}
//bool toggle=false;
void loop() {

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    
    
    DynamicJsonBuffer JSONbuffer;
    JsonObject& JSONencoder = JSONbuffer.createObject(); 
    
    
    JSONencoder["sensorName"] = "myTemper1";
    JSONencoder["sensorType"] = "Temperature & Light & UltraSonic";

    JsonArray& values = JSONencoder.createNestedArray("values"); //JSON array
    JsonArray& timestamps = JSONencoder.createNestedArray("timestamps"); //JSON array
    JsonArray& condition = JSONencoder.createNestedArray("condition"); //Contion json array
    JsonArray& distance = JSONencoder.createNestedArray("distance"); //distance json array


    byte temperature;
    byte humidity;
    
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
        Serial.print("Read DHT11 failed, err="); Serial.println(err);delay(1000);
        return;
      }

    String cond;
    float photocellReading = analogRead(photocell);
    float voltage= photocellReading * (5/1023.00);
    if(voltage<.3){
      cond="night";
      digitalWrite(ledPin, HIGH);
      }
    else{
      cond="day";
      digitalWrite(ledPin, LOW);
    }


    long duration;
    int dist;
    
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    
    // Calculating the distance
    dist= duration/29.1/2;
    Serial.print("Distance is ");
    Serial.println(dist);

    values.add((int)temperature); //Add value to array
    timestamps.add(String(millis()+0)); //Add value to array
    condition.add(cond);
    distance.add(dist);

    values.add((int)humidity); //Add value to array
    timestamps.add(String(millis()+1)); //Add value to array
    condition.add(cond);
    distance.add(dist);
 
    


    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
    Serial.println(JSONmessageBuffer);

    HTTPClient http;    //Declare object of class HTTPClient

    http.begin("http://192.168.43.246:8080/postjson");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    int httpCode = http.POST(JSONmessageBuffer);   //Send the request
    String payload = http.getString();                                        //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload

    http.end();  //Close connection

  } else {

    Serial.println("Error in WiFi connection");

  }

    delay(1000);  //Send a request every 1 second
}
