//Πρόγραμμα δοκιμής του αισθητήρα μικροσωματιδίων
//Ο παρακάτω κώδικας είναι από τον δικτυακό τόπο της Waveshare https://www.waveshare.com/wiki/File:Dust-Sensor-code.7z

//Ορίζουμε μερικές σταθερές που θα χρειαστούν στην μετατροπή
#define        COV_RATIO                       0.2            //ug/mmm / mv
#define        NO_DUST_VOLTAGE                 400            //mv
#define        SYS_VOLTAGE                     5000           

//Το led συνδέεται στην θύρα D10
const int iled = 10;            
//Τα δεδομένα στέλνονται στην θύρα Α2
const int vout = 2;             

//Μεταβλητές που θα χρειαστούν στην μετατροπή
float density, voltage;
int   adcvalue;

//Η συνάρτηση που κάνει την μετατροπή
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


void setup(void) {
  //Ορίζουμε το pin του Led σαν έξοδο και το σβήνουμε
  pinMode(iled, OUTPUT);
  digitalWrite(iled, LOW);                                    
  //Ξεκινάμε την σειριακή επικοινωνία
  Serial.begin(9600);                                         
}

void loop(void) {
  //Ανάβουμε το LED και περιμένουμε λίγα μικροδευτερόλεπτα
  digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  //Διαβάζουμε την τιμή του αισθητήρα (0-1024) και σβήνουμε το LED
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);

  //Εφαρμόζουμε το φίλτρο της συνάρτησης 
  adcvalue = Filter(adcvalue);

  //Μετατρέπουμε την τιμή σε τάση από 0-5V
  voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;
  
  //Μετατρέπουμε την τάση σε μικρογραμμάρια ανά κυβικό μέτρο
  if(voltage >= NO_DUST_VOLTAGE)
  {
    voltage -= NO_DUST_VOLTAGE;
    
    density = voltage * COV_RATIO;
  }
  else
    density = 0;
    
  //Εμφανίζουμε το αποτέλεσμα
  Serial.print("The current dust concentration is: ");
  Serial.print(density);
  Serial.print(" ug/m3\n");  
  //Περιμένουμε 1 δευτερόλεπτο για την επόμενη μέτρηση
  delay(1000);
}
