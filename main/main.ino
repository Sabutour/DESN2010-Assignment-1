// Included libararies
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>

// Keypad initialisation
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {23, 25, 27, 29}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {31, 33, 35, 37}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password = "01189998819991197253"; // change your password here
String input_password;
 
// RFID initialisation
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
String required_card = "BD 31 15 2B";
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  input_password.reserve(32); // maximum input characters is 33, change if needed
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Awaiting card...");
  Serial.println();

}
void loop() 
{
  Keypad();
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("Card read. UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  if (content.substring(1) == required_card) //change here the UID of the card/cards that you want to give access
  {
    Serial.println("Card authorised. Stage 1 complete.");
    Serial.println();
    delay(3000);
  }
 
 else   {
    Serial.println("Access denied");
    delay(3000);
  }
} 

void Keypad(){
  char key = keypad.getKey();

  if (key){
    if (key != '#') {
      Serial.print('*');
    }

    if(key == '*') {
      input_password = ""; // clear input password
    } else if(key == '#') {
      if(password == input_password) {
        Serial.println("");
        Serial.println("Access Granted.");
        // DO YOUR WORK HERE
        
      } else {
        Serial.println("");
        Serial.println("Access Denied.");
      }

      input_password = ""; // clear input password
    } else {
      input_password += key; // append new character to input password string
    }
  }
}