//Δοκιμή της μονάδας bluetooth
//Βιβλιοθήκες
#include <SoftwareSerial.h>             //Για την σειριακή σύνδεση με το bluetooth από τα pin D8 και D9

//Σταθερές
#define RXPin 8                         //Το pin λήψης δεδομένων του Bluetooth - D8
#define TXPin 9                         //Το pin αποστολής δεδομένων του Bluetooth - D9

//Αντικείμενα
SoftwareSerial BTserial(RXPin, TXPin);  //Αντικείμενο επικοινωνίας Software Serial για το bluetooth pins 8=RX | 9=TX

void setup() {
  Serial.begin(9600);                   //Ξεκινάμε την σειριακή επικοινωνία με τον υπολογιστή (Αυτό χρειάζεται μόνο για τον έλεγχο του προγράμματος)          
  BTserial.begin(9600);                 //Ξεκινάμε την σειριακή επικοινωνία με το bluetooth
  Serial.print("BTserial started at "); //Μηνύματα στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
  Serial.println(9600);                 //Μηνύματα στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
  Serial.println(" ");                  //Μηνύματα στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
}

void loop() {
  if (BTserial.available() > 0) {             //Αν υπάρχει διαθέσιμο μήνυμα στην θύρα του bluetooth τότε
    String command = BTserial.readString();   //Διαβάζουμε το μήνυμα
    Serial.println(command);                  //Γράφουμε το μήνυμα και στην σειριακή θύρα του υπολογιστή για έλεγχο του προγράμματος
    if (command == "getdata") {               //Αν το μήνυμα που πήραμε είναι το getdata τότε...
      Serial.print("Λάβαμε το getdata:");                                   
      //Στέλνουμε μηδενικές τιμές στο κινητό τηλέφωνο μέσω bluetooth
      BTserial.print(0);
      BTserial.print("*");
      BTserial.print(0);
      BTserial.print("*");
      BTserial.print(0);
      BTserial.print("*");
      BTserial.print(0);
      BTserial.print("*");
      BTserial.print(0);
      BTserial.print("*");
      BTserial.println(0);
      Serial.print("Στείλαμε μηδενικές τιμές στο κινητό");                                   
    }
  }
}
