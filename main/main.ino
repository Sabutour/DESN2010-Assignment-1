// Included libararies
#include <SPI.h>     // Required for RFID Reader
#include <MFRC522.h> // Required for RFID Reader
#include <Keypad.h>  // Required for Keypad
#include <User.h>    // My own little library!

// General initialisation

#define TOTAL_USERS 2
bool phase1Verified = false;
bool phase2Verified = false;
int current_user;
String input_password;

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

// The list of whitelisted users. Each user has a User ID, a Password, and a Card ID.
// The User ID is used only within the system to differentiate between them and is not
// displayed. Each user has their own password and keycard which are attached to their
// User object.

User userlist[TOTAL_USERS];



// RFID initialisation
#define SS_PIN 53
#define RST_PIN 5
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

void userSetup() {
  userlist[0] = User(-1, "", "");
  userlist[0].SetID(0);
  userlist[0].SetPassword("1234");
  userlist[0].SetCardID("8C 72 EA 37");

  userlist[1] = User(-1, "", "");
  userlist[1].SetID(1);
  userlist[1].SetPassword("ABCD");
  userlist[1].SetCardID("B3 9D 7D 15");

}


// Serial and SPI setup
void setup()
{
  userSetup();
  Serial.begin(9600); // Initiate a serial communication
  Serial.println("Initialising...");  
  input_password.reserve(32); // The max size of password
  SPI.begin();                // Initiate  SPI bus
  mfrc522.PCD_Init();         // Initiate MFRC522
  Serial.println("Done!");
  Serial.println("Awaiting card...");
  Serial.println();
}

// The unlock code. Only runs if both the card and password have been entered.
void unlockSignal()
{
  Serial.println("Two factor authenticaton complete, full access granted. Locking in 5 seconds...");
  delay(5000);
  Serial.println("Locking...");
  current_user = -1;
  phase1Verified = false;
  phase2Verified = false;
  Serial.println("Locked.");
}

// The main code loop, runs continuously.
void loop()
{

  if (phase1Verified && phase2Verified)
  {
    unlockSignal();
  }

  // Runs the Keypad program every loop.
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
    Serial.println("Card authorised.");
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

// Little helper function, compares the card ID with one in the database.
bool checkRFID(String to_check)
{
  for (int i = 0; i < TOTAL_USERS; i++)
  {
    Serial.println("");
    Serial.println("Checking ");
    Serial.print(to_check);
    Serial.print(" against ");
    Serial.print(userlist[i].card_id);
    Serial.println("");
    if (to_check == userlist[i].card_id)
    {
      current_user = userlist[i].id;
      Serial.print("Current User: ");
      Serial.println(current_user);
      Serial.println("");
      return true;
    }      
  }
  return false;
}

void Keypad()
{
  char key = keypad.getKey();

  if (key && phase1Verified == false)
  {
    Serial.println("");
    Serial.println("Please tap security card.");
    return;
  }

  if (key)
  {
    // Makes sure not to print a # when the user has finished inputting a password.
    if (key != '#' && key != '*')
    {
      Serial.print('*');
    }

    // Clears the password cache if the '*' is pressed.
    if (key == '*')
    {
      Serial.println("");
      Serial.println("Input cleared.");
      Serial.println("");
      input_password = "";
    }
    
    if (key == '#')
    {
      if (checkPassword(input_password) == true)
      {
        Serial.println("");
        Serial.println("Keypad authorised.");
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
    else if (key != '*')
    {
      input_password += key; // append new character to input password string
    }
  }
}

// If the password matches the current user's password,
// and if so, grants access and resets the user (-1 indicative
// of a null value.
bool checkPassword(String x)

{
  for (int i = 0; i < TOTAL_USERS; i++)
  {
    if (x == userlist[current_user].password)
    {
      Serial.println("Accepted.");
      current_user = -1;
      return true;
    }      
  }
  Serial.println("Denied.");
  current_user = -1;
  return false;
}