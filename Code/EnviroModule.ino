#include <SoftwareSerial.h>     //Library for bluetooth connection
#include <dht.h>                //Library for the temperature - humidity sensor

SoftwareSerial BTserial(8, 9);  //Software serial pins 8=RX | 9=TX


#define gasPin A0               //Other gases Sensor connects to A0
#define dhtPin A1               //DHT Sensor connects to A1
#define coPin A2                //CO sensor connects to A2
#define soundPin A3             //Sound sensor connects to A3
#define dustPin A4              //Dust particles sensor connects to A4
#define ledPin 10               //Dust sensor led connects to digital pin 10

dht DHT;                        //Create a DHT object

//Code from Waveshare dust sensor wiki
#define COV_RATIO 0.2           //ug/mmm / mv
#define NO_DUST_VOLTAGE 400     //mv
#define SYS_VOLTAGE 5000           
float density, voltage;
int adcvalue;
int Filter(int m)
{
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0)
  {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++)
    {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }
  else
  {
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}

void setup() {
  pinMode(ledPin,OUTPUT);               //set led pin for dust sensor to output
  digitalWrite(ledPin, LOW);            //Dust sensor Led default closed
  BTserial.begin(9600);                 //Begin serial communication for bluetooth
  Serial.begin(9600);                   //Begin serial communication with pc for debugging                    
  Serial.print("BTserial started at "); //Debugging messages
  Serial.println(9600);                 //Debugging messages
  Serial.println(" ");                  //Debugging messages
}

void loop() {
  if (BTserial.available() > 0) {             //If there is incoming data in BTserial
    String command = BTserial.readString();   //Read the incoming data
    Serial.println(command);                  //Debugging messages
    if (command == "getdata") {               //If the data recirved is getdata command then
  
      //Temperature - humidity
      DHT.read11(dhtPin);                     //Read the data from the DHT sensor
      float temp = DHT.temperature;           //Get temperature from sensor  
      float humidity = DHT.humidity;          //Get humidity from sensor
      
      //Noise
      float noise = analogRead(soundPin);     //Read data from sensor

      //Particles
      digitalWrite(ledPin, HIGH);                         //Light up led
      delayMicroseconds(280);                           //Delay for few microseconds
      adcvalue = analogRead(dustPin);                   //Get the analog reading from dust sensor
      digitalWrite(ledPin, LOW);                          //turn off led
      adcvalue = Filter(adcvalue);                      //Filter analog value
      voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11; //convert to voltage
      //Convert to density
      if(voltage >= NO_DUST_VOLTAGE)
      {
        voltage -= NO_DUST_VOLTAGE;
        
        density = voltage * COV_RATIO;
      }
      else
        density = 0;
      
      //CO
      float co = analogRead(coPin); //Read data from sensor

      //Other gasses
      float gas = analogRead(gasPin); //Read data from sensor
   
      //Messages for debugging
      //Temperature
      Serial.print("temperature = ");
      Serial.print(temp); 
      Serial.println("C  ");
      //Humidity
      Serial.print("Current humidity = ");
      Serial.print(humidity);
      Serial.print("%  ");
      //Noise
      Serial.print("Noise:");
      Serial.println("noise"); //Still cannot find how to convert to db
      //Particles
      Serial.println("GP2Y1010AU0F readings");
      Serial.print("density = ");
      Serial.print(density);
      Serial.println("mg/m3");
      //CO
      Serial.print("CO:");
      Serial.println(co);     //Still cannot find how to convert to concentration
      //CO
      Serial.print("Other gases:");
      Serial.println(gas);     //Still cannot find how to convert to concentration
      
      //Send data to bluetooth
      BTserial.print(temp);
      BTserial.print("*");
      BTserial.print(humidity);
      BTserial.print("*");
      BTserial.print(noise);
      BTserial.print("*");
      BTserial.print(density);
      BTserial.print("*");
      BTserial.print(co);
      BTserial.print("*");
      BTserial.println(gas);
    }
  }
}
