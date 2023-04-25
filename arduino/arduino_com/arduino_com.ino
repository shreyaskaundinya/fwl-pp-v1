// AUTHOR : Shrujana
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>


 
#define SS_PIN 53
#define RST_PIN 5
#define LASER 13 // Pin controlling the laser
#define LDR A0 // Analog input of the LDR
#define LED_RED 15 //Manages all the Red LEDs
#define LED_GREEN 16 // Manages all green LEDs
#define LED_WHITE 17 // Manages all White LEDs
#define PRESSURE A1 // Analog input of the Pressure sensor
#define PRESSURE_INIT 200 //Set the initial weight of the object
#define PRESSURE_DEFAULT 20
#define LASER_INIT 1000 // set the threshold for LDR

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Global states
static int locked = 1; //rfid
static int laserState = 1;
static int pressureState = 1;

void manageLED(){ //Controls all LEDS during all states
    if(locked){
        digitalWrite(LED_GREEN,LOW);
        digitalWrite(LED_WHITE,LOW);
        digitalWrite(LED_RED,HIGH);
    }
    else{
        digitalWrite(LED_GREEN,LOW);
        digitalWrite(LED_WHITE,HIGH);
        digitalWrite(LED_RED,LOW);
    }

}

void manageLDR(){   //analog read the LDR data and send laser breach data

    int ldrStatus = analogRead(LDR);
    Serial.println(ldrStatus);
    if (ldrStatus > LASER_INIT && locked == 1) {
        Serial.println("LASER breached");
        laserState = 0;
        return;
    }
    else if(ldrStatus > LASER_INIT && locked == 0){
        Serial.println("LASER Authorized access");
        laserState = 1;
        return;
    }
    else {
        // Serial.println("LASER DEACTIVATED");
        laserState = 1;
    }

}

void manageRFID(){  // Waits for card read constantly and send unlock data
    Serial.println("checking rfid...");

    if (!mfrc522.PICC_IsNewCardPresent()) {
        return; 
    }
    
    // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial())  
    {
        return;
    }

    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    // GET the id from Card reader
    String id= "";
    byte letter;
    
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        id.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ")); // copies the vlaue read into id as a hex formatt
        id.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    id.toUpperCase();
    Serial.println(id);
    
    // if system is already locked, unlock it
    if (locked) {
        unlockRFID(id);
    }
    // if system is unlocked, lock it
    else {
        lockSystem(id);
    }
}

void managePRESSURE(){  // serial read pressure plate and alert breach if any
   int pressureRead = analogRead(PRESSURE);
   
   Serial.println(pressureRead);
   if(pressureRead <= PRESSURE_DEFAULT && locked == 1){
        Serial.println("PRESSURE breached");
        pressureState = 0;
   }
   else if(pressureRead >= PRESSURE_INIT && locked ==0){
       Serial.println("Safe to remove the Object");
       pressureState = 1;
   }
   else {
      pressureState = 1;
   }

}

void alertBREACH(){ //sends data to node MCU.
        // This part of the code is the data sending to the node mcu
        //SendData(laserState,PressureState,locked)
        char charBuf[4];

        sprintf(charBuf, "%d%d%d", locked, laserState, pressureState); 
        
        //Serial.println("calling alert breach");
        Serial.println(charBuf);
        Wire.write(charBuf);
        // message = rfidState + laserState + pressureState
}

void unlockRFID(String id){ // just verifies if the card is valid or not
    if(id.substring(1) == "54 02 10 23")
        locked  = 0;
    else
        locked  = 1;
}

void lockSystem(String id){ // just verifies if the card is valid or not
    if(id.substring(1) == "54 02 10 23")
        locked  = 1;
    else
        locked  = 0;

    digitalWrite(LASER,HIGH);
    digitalWrite(PRESSURE,HIGH);
   
}

void manageLaser(){
    if(locked){
        digitalWrite(LASER,HIGH);
    }
    else
        digitalWrite(LASER,LOW);
}


void setup(void){
    pinMode(LASER,OUTPUT);
    pinMode(LED_GREEN,OUTPUT);
    pinMode(LED_RED,OUTPUT);
    pinMode(LED_WHITE,OUTPUT);
    pinMode(LDR,INPUT);
    pinMode(PRESSURE,INPUT);
    Serial.begin(9600);
    SPI.begin();
    mfrc522.PCD_Init();
    delay(10);
    mfrc522.PCD_DumpVersionToSerial();
    // wire for Node MCU
    Wire.begin(8);
    Wire.onRequest(alertBREACH);
    
}

void loop(void){
    manageRFID();
    manageLDR();
    manageLaser();
    managePRESSURE();
    manageLED();
    delay(4000);
}

  
