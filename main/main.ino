#include <User.h>

// Included libararies
#include <SPI.h>     // Required for RFID Reader
#include <MFRC522.h> // Required for RFID Reader
#include <Keypad.h>  // Required for Keypad


// General initialisation

bool phase1Verified = false;
bool phase2Verified = false;

// Keypad initialisation
const int ROW_NUM = 4;    // four rows
const int COLUMN_NUM = 4; // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

byte pin_rows[ROW_NUM] = {23, 25, 27, 29};      // connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {31, 33, 35, 37}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

String input_password;


// The lists of whitelisted passwords / card IDs. Each corresponds to a user.
// Need to improve to make object such that each user has their own of each.
// Currently, any combination of valid password and cardID will work.
User userlist[] = {
  User(1, "1234", "B3 9D 7D 15"), 
  User(2, "4321", "B1 9D 7D 15"), 
  User(3, "1111", "B4 D4 C4 34"), 
  User(4, "ABCD", "55 55 55 55")
  };

// RFID initialisation
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

void setup()
{
  Serial.begin(9600);         // Initiate a serial communication
  input_password.reserve(32); // maximum input characters is 32, change if needed
  SPI.begin();                // Initiate  SPI bus
  mfrc522.PCD_Init();         // Initiate MFRC522
  Serial.println("Awaiting card...");
  Serial.println();
}

void unlockSignal()
{ // Write code to tell the user the door was unlocked.
}

void loop()
{

  if (phase1Verified && phase2Verified)
  {
    Serial.println("Two factor authenticaton complete, full access granted.");
    delay(3000);
    unlockSignal();
    Serial.println("Locking...");
    phase1Verified = false;
    phase2Verified = false;
    Serial.println("Locked.");
  }

  Keypad();
  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  // Show UID on serial monitor
  Serial.print("Card read. UID tag :");
  String content = "";
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
  if (checkRFID(content.substring(1)) == true)
  {
    Serial.println("Card authorised. Stage 1 complete.");
    Serial.println();
    phase1Verified = true;
    delay(1000);
  }
  else
  {
    Serial.println("Access denied");
    phase1Verified = false;
    phase2Verified;
    delay(1000);
  }
}

bool checkRFID(String to_check)
{
  for (int i = 0; i < 4; i++)
  {
    if (to_check.c_str() == "test")
    {
      return true;
    }
    else
      return false;
  }
}

void Keypad()
{
  char key = keypad.getKey();

  if (key)
  {
    // Makes sure not to print a # when the user has finished inputting a password.
    if (key != '#')
    {
      Serial.print('*');
    }

    // Clears the password cache if the '*' is pressed.
    if (key == '*')
    {
      input_password = "";
    }
    else if (key == '#')
    {
      if (checkPassword(input_password) == true)
      {
        Serial.println("");
        Serial.println("Keypad authorised. Stage 2 complete.");
        phase2Verified = true;
      }
      else
      {
        Serial.println("");
        Serial.println("Incorrect PIN.");
        phase1Verified = false;
        phase2Verified = false;
      }

      input_password = ""; // clear input password
    }
    else
    {
      input_password += key; // append new character to input password string
    }
  }
}

bool checkPassword(String x)
{
  // for (int i = 0; i < 4; i++)
  // {
  //   if (x.c_str() == userlist[i].getPassword())
  //   {
  //     return true;
  //   }
  //   else
  //     return false;
  // }
}