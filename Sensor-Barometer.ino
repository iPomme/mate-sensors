// This #include statement was automatically added by the Particle IDE.
#include <Spark-Websockets.h>

// Relais
#define relaisPin D5 //10

// Explosion Button D4
#define explosionPin D4 //10


//Global
// This #include statement was automatically added by the Particle IDE.
//#include <OneWire.h>
#include <Wire.h>

// This #include statement was automatically added by the Particle IDE.
#include <SHT1x.h>



#define dataPin D2 //7
#define clockPin D3 //8
SHT1x sht1x(dataPin, clockPin);


// This #include statement was automatically added by the Particle IDE.
#include <BMP180.h>
BMP180 pressure;


//web socket
WebSocketClient client;


void onMessage(WebSocketClient client, char* message) {
  Serial.print("Received: ");
  Serial.println(message);
}



#define ALTITUDE 1655.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters

double actualPressure; // actual pressure
double centerspot; // tendence of last 100 readings
byte  plog[500];
int  plogindex;


byte  ledblinkstate;
bool alarmSent = false;


#define UPDATES_PER_SECOND 100

byte data = 170; //value to transmit, binary 10101010
byte mask = 1; //our bitmask
byte bitDelay = 100;

struct MyEEObject{
  double minPressure;
  double maxPressure;
};




void setup()
{
  pinMode(relaisPin, OUTPUT); 
  pinMode(explosionPin, INPUT_PULLUP);
 
  for(int i=0;i<100;i++){ plog[i]=0;}

  
  delay( 3000 ); // power-up safety delay
  Serial.begin(9600);
  Serial.println("REBOOT");

  //Wire.begin();
  
    // Initialize the sensor (it is important to get calibration values stored on the device).

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while(1); // Pause forever.
  }
  client.onMessage(onMessage);
  client.connect("172.20.10.2",8086);
  
}

int count = 1000;

void loop()
{
 
 if( digitalRead(explosionPin)==HIGH)
 {
    if(alarmSent==false)
    {
     alarmSent = true;
     client.send("BA-EXPLOSION:1");
     Serial.println("BA-EXPLOSION:1");
    }
 }
 else
 {
     alarmSent = false;
 }
 
if(count-- <= 0)
{
 count =1000;
 // Get a new pressure reading:
 actualPressure = getPressure();
 Serial.print("Pressure: ");
 Serial.println(actualPressure);
 
 char buf1[20];
 sprintf(buf1, "BA-Pressure:%011.6f", actualPressure);
 client.send(buf1);
 
  // Temperature to color (0-30)
  double t1 = getTemperature();
  Serial.print("Temperature: ");
  Serial.println(t1);

  char buf2[20];
  sprintf(buf2, "BA-Temperature:%011.6f", t1);
  client.send(buf2);
 

  float humidity = sht1x.readHumidity();
  if(humidity<50)
  {  
    digitalWrite(relaisPin, HIGH);
  }
  else
  {
    digitalWrite(relaisPin, LOW);
  } 
  
 Serial.print("Humidity: ");
 Serial.println(humidity);
 //Serial.print("Pressure: ");
 //Serial.println(actualPressure);

  char buf3[20];
  sprintf(buf3, "BA-Humidity:%011.6f", humidity);
  client.send(buf3);
  client.monitor();
 }
}  



double getPressure()
{
  char status;
  double Temperature,Pressure,p0,a;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(Temperature);
    if (status != 0)
    {
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Use '&P' to provide the address of P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(Pressure,Temperature);
        if (status != 0)
        {
          return(Pressure);
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}

double getTemperature()
{
  char status;
  double T;

  // You must first get a temperature measurement to perform a pressure reading.
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
     return(T);
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
}




