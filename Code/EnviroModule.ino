//Βιβλιοθήκες
#include <SoftwareSerial.h>             //Για την σειριακή σύνδεση με το bluetooth από τα pin D8 και D9
#include <Wire.h>                       //Για την επικοινωνία μέσω I2C με τον αισθητήρα bme280 (θερμοκρασία, υγρασία, βαρομετρική πίεση κλπ)
#include <Adafruit_Sensor.h>            //Για την λειτουργία του αισθητήρα bme280 (θερμοκρασία, υγρασία, βαρομετρική πίεση κλπ)
#include <Adafruit_BME280.h>            //Για την λειτουργία του αισθητήρα bme280 (θερμοκρασία, υγρασία, βαρομετρική πίεση κλπ)

//Σταθερές
#define dustPin A2                      //Το pin του αισθητήρα μικροσωματιδίων - A2
#define soundPin A3                     //To pin του αισθητήρα ήχου -  A3
#define ledPin 10                       //Το pin για το φως του αισθητήρα μικροσωματιδίων - D10
#define RXPin 8                         //Το pin λήψης δεδομένων του Bluetooth - D8
#define TXPin 9                         //Το pin αποστολής δεδομένων του Bluetooth - D9
#define SEALEVELPRESSURE_HPA (1013.25)  //Η πίεση στο επίπεδο της θάλασσας (την βρήκαμε από το πρόγραμμα δοκιμής του αισθητήρα bme280)
#define COV_RATIO 0.2                   //Σταθερές που χρειάζονται στον αισθητήρα μικροσωματιδίων (τις βρήκαμε από το πρόγραμμα δοκιμής του αισθητήρα)
#define NO_DUST_VOLTAGE 400             //Σταθερές που χρειάζονται στον αισθητήρα μικροσωματιδίων (τις βρήκαμε από το πρόγραμμα δοκιμής του αισθητήρα)
#define SYS_VOLTAGE 5000                //Σταθερές που χρειάζονται στον αισθητήρα μικροσωματιδίων (τις βρήκαμε από το πρόγραμμα δοκιμής του αισθητήρα)

//Μεταβλητές
bool result_bme;                        //Λογική μεταβλητή που θα παίρνει την τιμή true αν ο αισθητήρας bme280 λειτουργεί, αλλιώς θα παίρνει την τιμή false
int adcvalue;                           //Μεταβλητή που θα χρειαστεί στον αισθητήρα μικροσωματιδίων (την βρήκαμε από το πρόγραμμα δοκιμής του αισθητήρα)
float density, voltage;                 //Μεταβλητές που θα χρειαστούν στον αισθητήρα μικροσωματιδίων (τις βρήκαμε από το πρόγραμμα δοκιμής του αισθητήρα)
float temperature;                      //Μεταβλητή που θα κρατάει την τιμή της θερμοκρασίας
float pressure;                         //Μεταβλητή που θα κρατάει την τιμή της βαρομετρικής πίεσης
float humidity;                         //Μεταβλητή που θα κρατάει την τιμή της υγρασίας
float altitude;                         //Μεταβλητή που θα κρατάει την τιμή του υψομέτρου
const int sampleWindow = 50;            //Χρονικό παράθυρο μιας μέτρησης ήχου
const int testWindow = 3000;            //Χρονικό παράθυρο μιας μέτρησης
unsigned int sample;                    //Δείγμα ήχου
float noise;                            //Μεταβλητή που θα κρατάει την τιμή του θορύβου

//Συνάρτηση μετατροπής που χρειάζεται από τον αισθητήρα μικροσωματιδίων (την πήραμε από το πρόγραμμα δοκιμής)
int Filter(int m) {
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

//Αντικείμενα
SoftwareSerial BTserial(RXPin, TXPin);  //Αντικείμενο επικοινωνίας Software Serial για το bluetooth pins 8=RX | 9=TX
Adafruit_BME280 bme;                    //Αντικείμενο bme280

void setup() {
  pinMode(ledPin,OUTPUT);               //Ορίζουμε το pin του led του αισθητήρα μικροσωματιδίων σε OUTPUT
  digitalWrite(ledPin, LOW);            //Κλείνουμε το led του αισθητήρα μικροσωματιδίων
  Serial.begin(9600);                   //Ξεκινάμε την σειριακή επικοινωνία με τον υπολογιστή (Αυτό χρειάζεται μόνο για τον έλεγχο του προγράμματος)          
  BTserial.begin(9600);                 //Ξεκινάμε την σειριακή επικοινωνία με το bluetooth
  Serial.print("BTserial started at "); //Μηνύματα στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
  Serial.println(9600);                 //Μηνύματα στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
  Serial.println(" ");                  //Μηνύματα στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
  result_bme = bme.begin();             //Ξεκινάμε τον αισθητήρα bme280 και αποθηκεύουμε στην λογική μεταβλητή τη κατάσταση του (true ή false)
  if (result_bme == false) {            //Ανάλογα με την κατάσταση του αισθητήρα bme280 γράφουμε στην σειριακή θύρα του υπολογιστή το κατάλληλο μήνυμα
    Serial.println("BME280 does not work! Check wiring!");
  } else {
    Serial.println("BME280 works fine!");  
  }
}

void loop() {
  if (BTserial.available() > 0) {             //Αν υπάρχει διαθέσιμο μήνυμα στην θύρα του bluetooth τότε
    String command = BTserial.readString();   //Διαβάζουμε το μήνυμα
    Serial.println(command);                  //Γράφουμε το μήνυμα και στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
    if (command == "getdata") {               //Αν το μήνυμα που πήραμε είναι το getdata τότε πρέπει να μαζέψουμε τα δεδομένα
      unsigned long startTestMillis= millis();//Η συλλογή δεδομένων θα γίνει για 3 δευτερόλεπτα
      double voltsSum = 0;                    //Άθροισμα τιμών ήχου (volts προς το παρόν)
      float temperatureSum = 0;               //Άθροισμα τιμών θερμοκρασίας
      float pressureSum = 0;                  //Άθροισμα τιμών πίεσης
      float humiditySum = 0;                  //Άθροισμα τιμών υγρασίας
      float altitudeSum = 0;                  //Άθροισμα τιμών υψομέτρου
      float densitySum = 0;
      int testCount = 0;                      //Μετρητής μετρήσεων
      while (millis() - startTestMillis < testWindow) { //Ξεκινάμε την συλλογή δεδομένων για 3 δευτερόλεπτα
        testCount = testCount+1;              //Αυξάνουμε τον μετρητή των τεστ
        
        unsigned long startMillis= millis();  //Ξεκινάμε το χρονικό παράθυρο για την συλλογή του ήχου
        unsigned int peakToPeak = 0;          
        unsigned int signalMax = 0;
        unsigned int signalMin = 1024;
        // Συλλογή ήχων για 50 χιλιοδευτερόλεπτα
        while (millis() - startMillis < sampleWindow) {
          sample = analogRead(A3);
          if (sample < 1024) {
            if (sample > signalMax) {
              signalMax = sample;
            } else if (sample < signalMin) {
              signalMin = sample;  // save just the min levels
            }
          }
        }
        peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
        double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
        voltsSum = voltsSum+volts;

        //Μαζεύουμε τα δεδομένα από τον αισθητήρα bme280
        if (result_bme == true) {
          temperature = bme.readTemperature();
          pressure = bme.readPressure()/100.0F;
          humidity = bme.readHumidity();
          altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
        }
        temperatureSum = temperatureSum + temperature;
        pressureSum = pressureSum + pressure;
        humiditySum = humiditySum + humidity;
        altitudeSum = altitudeSum + altitude;

        ////Μαζεύουμε τα δεδομένα από τον αισθητήρα μικροσωματιδίων
        digitalWrite(ledPin, HIGH);
        delayMicroseconds(280);
        adcvalue = analogRead(dustPin);
        digitalWrite(ledPin, LOW);
        adcvalue = Filter(adcvalue);
        voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;
        if(voltage >= NO_DUST_VOLTAGE)
        {
          voltage -= NO_DUST_VOLTAGE;
          density = voltage * COV_RATIO;
        }
        else
          density = 0;

        densitySum = densitySum + density;
      }
      
      //Στέλνουμε τα δεδομένα και στην σειριακή θύρα του υπολογιστή για έλεγχο δεδομένων
      if (result_bme == true) {
        //Temperature
        Serial.print("temperature:");                           
        Serial.print(temperatureSum/testCount);
        Serial.println("*C   ");
        //Pressure
        Serial.print("pressure:");
        Serial.print(pressureSum/testCount);
        Serial.println("hPa   ");
        //Humidity
        Serial.print("humidity:");
        Serial.print(humiditySum/testCount);
        Serial.println("%   ");
        //Altitude
        Serial.print("altitude:");
        Serial.print(altitudeSum/testCount);
        Serial.println("m");
      }
      //Noise
      Serial.print("noise Volts = ");
      Serial.println(voltsSum/testCount);
      //Particles
      Serial.println("GP2Y1010AU0F readings");
      Serial.print("density = ");
      Serial.print(densitySum/testCount);
      Serial.println("mg/m3");
      
      //Στέλνουμε τα δεδομένα στο κινητό τηλέφωνο μέσω bluetooth
      BTserial.print(temperatureSum/testCount);
      BTserial.print("*");
      BTserial.print(humiditySum/testCount);
      BTserial.print("*");
      BTserial.print(voltsSum/testCount);
      BTserial.print("*");
      BTserial.print(densitySum/testCount);
      BTserial.print("*");
      BTserial.print(pressureSum/testCount);
      BTserial.print("*");
      BTserial.println(altitudeSum/testCount);
    }
  }
}
