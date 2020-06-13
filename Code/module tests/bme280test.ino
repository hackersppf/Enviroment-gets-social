//Βιβλιοθήκη για την επικοινωνία I2C που χρειάζεται ο αισθητήρας bme280
#include <Wire.h>             
//Βιβλιοθήκες που χρειάζονται για τον αισθητήρα bme280
#include <Adafruit_Sensor.h>  
#include <Adafruit_BME280.h>  

//Δημιουργία αντικειμένου bme για να πάρουμε τις τιμές του αισθητήρα bme280
Adafruit_BME280 bme;          

//Εδώ βάζουμε την ατμοσφαιρική πίεση στο επίπεδο της θάλασσας (υψόμετρο μηδέν)
#define SEALEVELPRESSURE_HPA (1013.25)  


void setup() {
  //Ξεκινάμε την σειριακή θύρα
  Serial.begin(9600);
  //Δημιουργούμε μια μεταβλητή boolean (true ή false) με το όνομα result_bme όπου θα αποθηκεύουσε το αποτέλεσμα της επικοινωνίας του Arduino με τον αισθητήρα
  bool result_bme;
  result_bme = bme.begin();  
  //Αν η επικοινωνία αποτύχει τότε βγάζουμε μήνυμα λάθους αλλιώς βγάζουμε μήνυμα επιτυχίας
  if (result_bme == false) {
    Serial.println("Ο αισθητήρας BME280 δεν δουλεύει! Ελέγξτε καλωδιώσεις!");
  } else {
    Serial.println("Ο αισθητήρας BME280 δουλεύει!");  
  }
}

void loop() { 
  //Αν ο αισθητήρας έχει συνδεθεί σωστά εμφανίζουμε τα αποτελέσματα
  if (result_bme == true) {
    Serial.print("temperature:");
    Serial.print(bme.readTemperature());
    Serial.print("*C   ");
    Serial.print("pressure:");
    Serial.print(bme.readPressure()/100.0F);
    Serial.print("hPa   ");
    Serial.print("humidity:");
    Serial.print(bme.readHumidity());
    Serial.print("%   ");
    Serial.print("altitude:");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println("m");
    delay(40);
  }
}
