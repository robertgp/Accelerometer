// TEAM 1 EK210 A5 FALL DETECTOR MANDOWN


// **************************************BLUETOOTH************************
// <SPI.h>
#include "Adafruit_BLE_UART.h"
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 4
Adafruit_BLE_UART uart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

// **********************************ACCELEROMETER************************
#include <Wire.h>
#include "Arduino.h"
#include "MMA7660.h"
MMA7660 accelemeter;
  int8_t x;
  int8_t y;
  int8_t z;
  float ax,ay,az;
  const int table_length = 5;
  int8_t pos[table_length][3];
  float acce[table_length][3];
  int row = 0;
  int fall_count = 0;
  int ignore_initial = 0;

// *******************************SPEAKER***********************************
#define  c     3830    // 261 Hz 
#define  d     3400    // 294 Hz 
#define  e     3038    // 329 Hz 
#define  f     2864    // 349 Hz 
#define  g     2550    // 392 Hz 
#define  a     2272    // 440 Hz 
#define  b     2028    // 493 Hz 
#define  C     1912    // 523 Hz 
// Define a special note, 'R', to represent a rest
#define  R     0
int speakerOut = 9;

// ******************************BLUETOOTH***********************************
void aciCallback(aci_evt_opcode_t event)
{
  switch (event)
  {
  case ACI_EVT_DEVICE_STARTED:
    Serial.println(F("Advertising started"));
    break;
  case ACI_EVT_CONNECTED:
    Serial.println(F("Connected!"));
    break;
  case ACI_EVT_DISCONNECTED:
    Serial.println(F("Disconnected or advertising timed out"));
    break;
  default:
    break;
  }
}

void rxCallback(uint8_t *buffer, uint8_t len)
{
  String msg = "bluetooth says: ";

  Serial.print(F("Received "));
  Serial.print(len);
  Serial.print(F(" bytes: "));
  for (int i = 0; i<len; i++)
    Serial.print((char)buffer[i]);

  Serial.print(F(" ["));

  for (int i = 0; i<len; i++)
  {
    Serial.print(" 0x");
    Serial.print((char)buffer[i], HEX);
    msg += (char)buffer[i];

  }
  Serial.println(F(" ]"));

  /* Echo the same data back! */
  len = msg.length();
  char msgArr[len];

  for (int i = 0; i<len; i++) {
    msgArr[i] = msg[i];
  }
  uart.write((uint8_t*)msgArr, len);
}

// ************************END*OF*BLUETOOTH********************************
// ***********************END*OF*STUFF*BEFORE*SETUP*************************************************************************************************
void setup()
{
  
//******************ACCELEROMETER*AND*SPEAKER******************************
  pinMode(speakerOut, OUTPUT);
  accelemeter.init();
  Serial.begin(9600);

//***********************BLUETOOTH*****************************************
while (!Serial); // Leonardo/Micro should wait for serial init
  Serial.println(F("Adafruit Bluefruit Low Energy nRF8001 Callback Echo demo"));
  uart.setRXcallback(rxCallback);
  uart.setACIcallback(aciCallback);
  uart.setDeviceName("ManDown"); /* 7 characters max! */
  uart.begin();
}
//*********************SPEAKER*********************************************

// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[], 
//  which sets each note's relative length (higher #, longer note) 
int melody[] = {  C,  b,  g,  C,  b,   e,  R,  C,  c,  g, a, C };
int beats[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 }; 
int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(speakerOut,HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }                                 
}

//***************************ACTUAL*CODE*************************************************
void loop()
{
  uart.pollACI(); //SHAMSEEN (only 1 line of code in void main)
  
      /*Serial.println("Reading xyz");
      accelemeter.getXYZ(&x,&y,&z);
      pos[row][0] = x;
      pos[row][1] = y;
      pos[row][2] = z;

      Serial.println("Reading ax_ay_az");
      accelemeter.getAcceleration(&ax,&ay,&az);
      acce[row][0] = ax;
      acce[row][1] = ay;
      acce[row][2] = az; 
      
      row++;
      
      if (row == table_length) { 
        Serial.println(x); 
        Serial.println(y);   
        Serial.println(z);
        Serial.println(ax);
        Serial.println(ay);
        Serial.println(az);
        Serial.println(millis()/1000);
        row = 0;
      }
      delay(500); */

  accelemeter.getXYZ(&x,&y,&z);
  
    //Serial.print("x = ");
    //Serial.println(x); 
    //Serial.print("y = ");
    //Serial.println(y);   
    //Serial.print("z = ");
    //Serial.println(z);
    
    pos[row][0] = x;
    pos[row][1] = y;
    pos[row][2] = z;
  
  accelemeter.getAcceleration(&ax,&ay,&az);
    //Serial.println("accleration of X/Y/Z: ");
    //Serial.print(ax);
    //Serial.println(" g");
    //Serial.print(ay);
    //Serial.println(" g");
    //Serial.print(az);
    //Serial.println(" g");
  
  acce[row][0] = ax;
  acce[row][1] = ay;
  acce[row][2] = az;
      
  delay(500);

  /*Serial.println(x);
  Serial.println(y);
  Serial.println(z);
  Serial.println(ax);
  Serial.println(ay);
  Serial.println(az);*/
  
  Serial.println(pos[row][0]);
  Serial.println(pos[row][1]);
  Serial.println(pos[row][2]);

  Serial.println(acce[row][0]);
  Serial.println(acce[row][1]);
  Serial.println(acce[row][2]);

  int xdiff, ydiff, zdiff;
  if (row == 0) {
    xdiff = pos[row][0] - pos[4][0];
    ydiff = pos[row][1] - pos[4][1];
    zdiff = pos[row][2] - pos[4][2];
  }
  else {
    xdiff = pos[row][0] - pos[row-1][0];
    ydiff = pos[row][1] - pos[row-1][1];
    zdiff = pos[row][2] - pos[row-1][2];
  }
  

// Determining a fall in this iteration
if (ignore_initial >= 5){
 if ( (xdiff >= 30 || xdiff <=-30) || (ydiff >= 30 || ydiff <= -30) || (zdiff >= 30 || zdiff <= -30) ) {
  if ((acce[row][0] <= 0.15 && acce[row][0] >= -0.15) || (acce[row][1] <= 0.15 && acce[row][1] >= -0.15) || (acce[row][2] <= 0.15 && acce[row][2] >= -0.15)) {
  //if (ax <= 0.2 && ax >= -0.2 || ay <= 0.2 && ay >= -0.2 || az <= 0.2 && az >= -0.2) {
    Serial.println("Possible Fall, Wait for Confirmation");
    int8_t check_table[5][3]; 
    //int d1x,d1y,d1z,d2x,d2y,d2z,d3x,d3y,d3z,d4x,d4y,d4z;
    int dx,dy,dz;
    for (int i = 0; i < 5; i++) {
      accelemeter.getXYZ(&x,&y,&z);
      check_table[i][0] = x;
      check_table[i][1] = y;
      check_table[i][2] = z;
    }
    /*d1x = (check_table[1][0]) - (check_table[0][0]);
    d1y = (check_table[1][1]) - (check_table[0][1]);
    d1z = (check_table[1][2]) - (check_table[0][2]);
    
    d2x = (check_table[2][0]) - (check_table[1][0]);
    d2y = (check_table[2][1]) - (check_table[1][1]);
    d2z = (check_table[2][2]) - (check_table[1][2]);

    d3x = (check_table[3][0]) - (check_table[2][0]);
    d3y = (check_table[3][1]) - (check_table[2][1]);
    d3z = (check_table[3][2]) - (check_table[2][2]);

    d4x = (check_table[4][0]) - (check_table[3][0]);
    d4y = (check_table[4][1]) - (check_table[3][1]);
    d4z = (check_table[4][2]) - (check_table[3][2]);*/

    dx = check_table[4][0] - check_table[0][0];
    dy = check_table[4][1] - check_table[0][1];
    dz = check_table[4][2] - check_table[0][2];

    if ((dx <= 15 && dx >= -15) && (dy <= 15 && dy >= -15) && (dz <= 15 && dz >= -15)) {
      fall_count++; 
    }

    if (fall_count >= 2) {
      Serial.println("MAN DOWN");
      fall_count = 0;
      // Make Speaker Sound
        for (int i=0; i<MAX_COUNT; i++) {
          tone_ = melody[i];
          beat = beats[i];
          duration = beat * tempo; // Set up timing
          playTone(); 
          delayMicroseconds(pause);// A pause between notes...
        }
    }

  }
  else {
    fall_count = 0;
  }
 }
}
// Incrememnting and "cleaning" the array 
  row++;

  Serial.println("*************");

  if (row == table_length) {
    Serial.println("Done with 1 table"); //Helps us know when one table has been filled
    row = 0;
  }
  delay(500);

  ignore_initial++;
}
