#include <T3Mac.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>   
#include <FastLED.h>



const int MAX_UDP_SIZE = 1024;

CRGB leds[MAX_UDP_SIZE];

int ACTUAL_LED_COUNT;
int LEAD_LED_COUNT = 0;

int mac_index[19][6] = {
  {4,233,229,0,192,62},
  {4,233,229,0,191,119},
  {4,233,229,0,192,141},
  {4,233,229,0,191,16},
  {4,233,229,0,192,65},
  {4,233,229,0,191,78},
  {4,233,229,0,192,251},
  {4,233,229,0,192,248},
  {4,233,229,0,192,66},
  {4,233,229,0,192,16},
  {4,233,229,0,192,50},
  {4,233,229,0,192,214},
  {4,233,229,0,192,240},
  {4,233,229,0,173,215},
  {4,233,229,0,192,218},
  {4,233,229,0,191,191},
  {4,233,229,0,190,244},
  {4,233,229,0,192,223},
  {4,233,229,0,192,41}
 };
 
 
String ID[19] = {"A2","B2","C2","D2","E2","E3","F3","G3","H3","I3","F4","G4","H4","I4","J4","K4","K1","L1","M1"};

char ID_LETTER[19] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S'};


//byte mac[];
IPAddress ip[19] = {
  IPAddress(192,168,1,100),
  IPAddress(192,168,1,101),
  IPAddress(192,168,1,102),
  IPAddress(192,168,1,103),
  IPAddress(192,168,1,104),
  IPAddress(192,168,1,105),
  IPAddress(192,168,1,106),
  IPAddress(192,168,1,107),
  IPAddress(192,168,1,108),
  IPAddress(192,168,1,109),
  IPAddress(192,168,1,110),
  IPAddress(192,168,1,111),
  IPAddress(192,168,1,112),
  IPAddress(192,168,1,113),
  IPAddress(192,168,1,114),
  IPAddress(192,168,1,115),
  IPAddress(192,168,1,116),
  IPAddress(192,168,1,117),
  IPAddress(192,168,1,118)
};

int FRAME_ID = -1;

const int FRAME_DIVIDER = 10;

int LED_LENGTHS[19][7] = {
  {2,2,3,2,2,2,0}, //A2
  {2,1,1,2,2,2,2}, //B2
  {3,4,3,2,3,2,2}, //C2
  {3,3,4,4,3,4,3}, //D2 {3,3,4,4,3,4,3}
  {0,0,0,0,1,2,3}, //E2
  {3,3,5,5,5,5,5}, //E3
  {2,2,3,3,2,3,3}, //F3
  {3,3,3,3,3,2,3}, //G3
  {4,4,3,2,3,3,2}, //H3
  {3,3,5,5,4,5,4}, //I3
  {5,4,3,2,0,0,0}, //F4
  {6,6,5,6,6,5,5}, //G4
  {4,5,6,5,6,6,5}, //H4
  {3,2,3,3,2,3,3}, //I4
  {5,5,4,3,3,3,3}, //J4
  {0,0,2,4,5,5,6}, //K4
  {6,7,7,6,3,1,0}, //K1
  {5,4,3,3,3,4,5}, //L1
  {0,0,2,4,5,5,5}  //M1
};
 
unsigned int localPort = 8888;   

char packetBuffer[MAX_UDP_SIZE]; //buffer to hold incoming packet,
EthernetUDP Udp;

//receive data
int counter = 0;
int reply_frequency = 10;


//sensor variables
int reading_count = 0;
unsigned long sensor_total = 0;
int sensor_max = 0;
int resetPin = 20  ;

int micro_delay = 0;
int milli_delay = 0;


void setup() {
  delay(2000);
  Serial.begin(9600);
  Serial.setTimeout(50);
  read_mac();
  FRAME_ID = compareMac();

  
  //Ethernet Module Reset
  digitalWrite(resetPin, LOW);
  pinMode(resetPin, OUTPUT);
  delayMicroseconds(10);
  pinMode(resetPin, INPUT);
  

  Ethernet.begin(mac, ip[FRAME_ID]);
  // start listening for clients
  delay(200);
  Udp.begin(localPort);
  
    ACTUAL_LED_COUNT = 0;
  
    if (LED_LENGTHS[FRAME_ID][0] > 0) {
      LEDS.addLeds<WS2801, 2, 3, GRB, DATA_RATE_MHZ(1)>(leds, ACTUAL_LED_COUNT, LED_LENGTHS[FRAME_ID][0]);
      ACTUAL_LED_COUNT +=  LED_LENGTHS[FRAME_ID][0];
    }
    if (LED_LENGTHS[FRAME_ID][1] > 0) {
      LEDS.addLeds<WS2801, 4, 5, GRB, DATA_RATE_MHZ(1)>(leds, ACTUAL_LED_COUNT, LED_LENGTHS[FRAME_ID][1]);
      ACTUAL_LED_COUNT +=  LED_LENGTHS[FRAME_ID][1];
    }
    if (LED_LENGTHS[FRAME_ID][2] > 0) {
      LEDS.addLeds<WS2801, 6, 7, GRB, DATA_RATE_MHZ(1)>(leds, ACTUAL_LED_COUNT, LED_LENGTHS[FRAME_ID][2]);
      ACTUAL_LED_COUNT +=  LED_LENGTHS[FRAME_ID][2];
    }
    if (LED_LENGTHS[FRAME_ID][3] > 0) {
      LEDS.addLeds<WS2801, 8, 9, GRB, DATA_RATE_MHZ(1)>(leds, ACTUAL_LED_COUNT, LED_LENGTHS[FRAME_ID][3]);
      ACTUAL_LED_COUNT +=  LED_LENGTHS[FRAME_ID][3];
    }
    if (LED_LENGTHS[FRAME_ID][4] > 0) {
      LEDS.addLeds<WS2801, 14, 15, GRB, DATA_RATE_MHZ(1)>(leds, ACTUAL_LED_COUNT, LED_LENGTHS[FRAME_ID][4]);
      ACTUAL_LED_COUNT +=  LED_LENGTHS[FRAME_ID][4];
    }
    if (LED_LENGTHS[FRAME_ID][5] > 0) {
      LEDS.addLeds<WS2801, 16, 17, GRB, DATA_RATE_MHZ(1)>(leds, ACTUAL_LED_COUNT, LED_LENGTHS[FRAME_ID][5]);  
      ACTUAL_LED_COUNT +=  LED_LENGTHS[FRAME_ID][5];
    }
    if (LED_LENGTHS[FRAME_ID][6] > 0) {
      LEDS.addLeds<WS2801, 18, 19, GRB, DATA_RATE_MHZ(1)>(leds, ACTUAL_LED_COUNT, LED_LENGTHS[FRAME_ID][6]);
      ACTUAL_LED_COUNT +=  LED_LENGTHS[FRAME_ID][6];
    }
  
  getLeadInLEDs(); 
  
  LEDS.showColor(CRGB(0, 0, 0));
  LEDS.show();
  delay(2000);
}

void loop() {

  // int startChar = client.read();


  int packetSize = Udp.parsePacket();
  if(packetSize)
  {
    Udp.read((char*)packetBuffer,MAX_UDP_SIZE);
    char startChar = packetBuffer[0];
    char listenChar = packetBuffer[1];
    

    if (startChar == '!' && FRAME_ID <= FRAME_DIVIDER) {
      memcpy(leds, packetBuffer+2+(LEAD_LED_COUNT*3), ACTUAL_LED_COUNT*3);
     
      if (micro_delay > 0)
         delayMicroseconds(micro_delay);
      if (milli_delay > 0)  
        delay(milli_delay);
        
      LEDS.show();
    } else if (startChar == '@' && FRAME_ID > FRAME_DIVIDER) {
      memcpy(leds, packetBuffer+2+(LEAD_LED_COUNT*3), ACTUAL_LED_COUNT*3);
      
      if (micro_delay > 0)
         delayMicroseconds(micro_delay);
      if (milli_delay > 0)  
        delay(milli_delay);
        
      LEDS.show();
    } else if (startChar == '#') {
        Serial.println("CHECK");
          setDelayValues(String(packetBuffer+2));
     
    }
    else if (startChar >= 0) {
      // discard unknown characters
    }
     setSensorValues();
    if(listenChar == ID_LETTER[FRAME_ID]){
     sensor_reply();
    }
  }
  serialCheck();
}



void sensor_reply() {
  char cstr[50];
  String str;

  // if (counter > reply_frequency) {
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  /*Udp.write("Hi from Teensy, ");
  str = String(analogRead(A9));
  str.toCharArray(cstr,16);
  */
  //str = ID[FRAME_ID];
 // str += ",";
  str += FRAME_ID;
  str += ",";
  //str += sensor_total;
  //str += ",";
  str += reading_count;
  str += ",";
  str += sensor_total/(reading_count);
  str += ",";
  str += sensor_max;
  
  str.toCharArray(cstr,50);
  Udp.write(cstr);
  Udp.endPacket();
  
  resetSensorValues();
}


void setSensorValues() {
  int temp = analogRead(A9);
  sensor_total = sensor_total + temp;
  reading_count = reading_count + 1;
  
  if (temp > sensor_max) {
    sensor_max = temp;
  }
}


void resetSensorValues() {
  sensor_total = 0;
  reading_count = 0;
  sensor_max = 0;
}


void getLeadInLEDs() {
  if (FRAME_ID <= FRAME_DIVIDER){
    for(int i = 0; i < FRAME_ID; i++) {
      for(int j = 0; j < 7; j++) {
       LEAD_LED_COUNT += LED_LENGTHS[i][j];
      }
    }
} else {
  for(int i = FRAME_DIVIDER+1; i < FRAME_ID; i++) {
      for(int j = 0; j < 7; j++) {
       LEAD_LED_COUNT += LED_LENGTHS[i][j];
      }
    }
}

}


int compareMac() {
 int index = -1;
 for (int i = 0; i < 19; i++) {
   boolean equal_mac = true;
   for (int j = 0; j < 6; j++) {
   
     if (mac[j] != mac_index[i][j]) {
       equal_mac = false;
       break;
     } 
   }
   if (equal_mac) {
      index = i;
       break; 
     }
 }
 
 return index;
}

void printVariables() {
  
  
  Serial.println(ip[FRAME_ID]);
    
  Serial.print("TEENSY INDEX --> ");
  Serial.print(FRAME_ID);
  Serial.print(" --> ");
  if (FRAME_ID < 0) 
    Serial.println("NOT FOUND");
  else
    Serial.println(ID[compareMac()]);
  
  Serial.print("LED Count --> ");
  Serial.println(ACTUAL_LED_COUNT);
  
  Serial.print("LED LEADIN --> ");
  Serial.println(LEAD_LED_COUNT);
  Serial.println();
  
  Serial.print(LED_LENGTHS[FRAME_ID][0]);
  Serial.print(';');
  Serial.print(LED_LENGTHS[FRAME_ID][1]);
  Serial.print(';');
  Serial.print(LED_LENGTHS[FRAME_ID][2]);
  Serial.print(';');
  Serial.print(LED_LENGTHS[FRAME_ID][3]);
  Serial.print(';');
  Serial.print(LED_LENGTHS[FRAME_ID][4]);
  Serial.print(';');
  Serial.print(LED_LENGTHS[FRAME_ID][5]);
  Serial.print(';');
  Serial.print(LED_LENGTHS[FRAME_ID][6]);
  Serial.println();
  
  Serial.print("micro > ");
  Serial.print(micro_delay);
  Serial.print("      milli > ");
  Serial.print(milli_delay);
  Serial.println();
  
  Serial.println();
  Serial.println();
  
}

void setDelayValues(String temp) {
  int frame_delays[28];
  for (int i = 0; i< 28; i++){
    frame_delays[i] = temp.toInt();
    temp = temp.substring(temp.indexOf(',')+1);    
  }
  
  micro_delay = frame_delays[FRAME_ID*2];
  milli_delay = frame_delays[(FRAME_ID*2)+1];
  
}

void serialCheck() {
  if (Serial.available()) {
    // get the new byte:
    while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    }

   printVariables(); 
  }
  
}

    
