#include <SPI.h>
#include <MFRC522.h>

#include <Servo.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include <Keypad.h>

#include <Ultrasonic.h>

#include <Bonezegei_DHT11.h>


#define DHT11 2
#define BUZZER 3
#define LED_GREEN 4
#define LED_WHITE 5
#define LED_YELLOW 7
#define BUTTON_RED 9
#define PIR 10
#define SERVO 11
#define BUTTON_BLUE 12
#define LED_RED 13
#define AUDIO A0
#define FLAME A1
#define LDR A2
#define MQ2 A3


#define RST_PIN 49                 // Configurable, see typical pin layout above
#define SS_PIN 48                  // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

const byte ROWS = 4;  //four rows
const byte COLS = 4;  //three columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 33, 35, 37, 39 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 41, 43, 45, 47 };  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

Ultrasonic ultrasonic(25, 26);

Bonezegei_DHT11 dht(DHT11);

Servo myservo;

bool ledYellow = 0;
bool tempVal = 0;
bool doorStatus = 0;
bool zoneStatus[3] = { 0 };
short selectedZone = 0;
short zoneOffset = 'A';

char setPass[4] = { '1', '2', '3', '4' };
short passLength = 4;
char keyPass[4] = { 0 };
short pressedKeys = 0;
bool alarmTriggered = 0;
bool tempDisplay = 0;
unsigned long tempTime=0;

LiquidCrystal_I2C lcd(0x27, 20, 4);
void lightDetect();
void flameDetect();
void gasDetect();
void doorLock();
void alarmControl();
void zonePrint();
void alarmTrigger();
unsigned long getID();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  dht.begin();
  mfrc522.PCD_Init();
  myservo.attach(11);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Hello, world!");
  lcd.setCursor(0, 1);
  lcd.print("DOOR: UNLOCKED");
  myservo.write(0);
  zonePrint(selectedZone);
  //analogWrite(SERVO, 128);
}



void loop() {

  float light = analogRead(LDR);
  light = light / 1024;  //get the percentage of light
  if (light < 0.28)
    light = 0;
  else if (light > 0.8)
    light = 1;
  analogWrite(LED_WHITE, (light * 255));

  if (digitalRead(FLAME) == 0)
    alarmTrigger(1);
  if (analogRead(MQ2) > 600)
    alarmTrigger(0);

  if (mfrc522.PICC_IsNewCardPresent())
    doorLock();

  char key = keypad.getKey();
  if (key) {
    digitalWrite(BUZZER, 1);
    delay(50);
    digitalWrite(BUZZER, 0);
    alarmControl(key);
  }

  if (zoneStatus[0] == 1)
    if (digitalRead(PIR) == 1) {
      digitalWrite(BUZZER, 1);
      alarmTriggered=1;
    }
  if (zoneStatus[1] == 1)
    if (digitalRead(AUDIO) == 0) {
      digitalWrite(BUZZER, 1);
      alarmTriggered=1;
    }
  if (zoneStatus[2] == 1)
    if(ultrasonic.read() <= 5){
      digitalWrite(BUZZER, 1);
      alarmTriggered=1;
    }
  if (tempDisplay == 1 && (millis() - tempTime) >= 1000){

    lcd.setCursor(0,0);
    lcd.print("TMP|HUM|GAS|LIGH");
    lcd.setCursor(0,1);
    float tempDHT = 0;
    short tempDHTint=0;
    short humDHT = 0;
    if (dht.getData()) {
      tempDHT = dht.getTemperature();
      tempDHTint= tempDHT;
      humDHT = dht.getHumidity();
    }
    if (tempDHT >= 100)
      tempDHT = 99;
    else if(tempDHT < 10)
      lcd.print("0");
    lcd.print(tempDHTint);
    lcd.print("C|");

    if (humDHT >= 100)
      humDHT = 99;
    else if(humDHT < 10)
      lcd.print("0");
    lcd.print(humDHT);
    lcd.print("%|");
  
    float readMQ2= analogRead(MQ2);
    readMQ2 = readMQ2 / 1024 *100;
    if (readMQ2 >= 100)
      readMQ2 = 99;
    else if(readMQ2 < 10)
      lcd.print("0");
    short readMQ2int= readMQ2;
    lcd.print(readMQ2int);
    lcd.print("%|");
    float readLDR= analogRead(LDR);
    readLDR = 100-(readLDR / 1024 *100);
    if (readLDR >= 100)
      readLDR = 99;
    else if(readLDR < 10)
      lcd.print("0");
    short readLDRint= readLDR;

    lcd.print(readLDRint);
    lcd.print("%|");

    tempTime = millis();
  }
}

void lightDetect() {
  float light = analogRead(LDR);
  light = light / 1024;  //get the percentage of light
  if (light < 0.28)
    light = 0;
  else if (light > 0.8)
    light = 1;
  analogWrite(LED_WHITE, (light * 255));
  delay(100);
}

void flameDetect() {
  //unsigned long time01=millis();
  while (digitalRead(BUTTON_RED) == 1) {
    digitalWrite(LED_RED, 0);
    delay(100);
    digitalWrite(BUZZER, 1);
    delay(40);
    digitalWrite(BUZZER, 0);
    delay(30);
    digitalWrite(BUZZER, 1);
    delay(80);
    digitalWrite(BUZZER, 0);
    digitalWrite(LED_RED, 1);
    delay(100);
  }
  digitalWrite(BUZZER, 0);
  digitalWrite(LED_RED, 0);
}

void gasDetect() {

  while (digitalRead(BUTTON_BLUE) == 1) {
    digitalWrite(LED_YELLOW, 0);
    digitalWrite(BUZZER, 1);
    delay(30);
    digitalWrite(BUZZER, 0);
    delay(30);
    digitalWrite(LED_YELLOW, 1);
    delay(100);
  }
  digitalWrite(BUZZER, 0);
  digitalWrite(LED_YELLOW, 0);
}

void doorLock() {
  unsigned long uid = getID();
  //Serial.println(uid);
  if (uid != 4294947084 && uid != 4294944506)
    return;

  doorStatus = !doorStatus;

  if (doorStatus == 0) {
    myservo.write(0);
    //lcd.print("UNLOCKED");
  } else {
    myservo.write(90);
    //lcd.print("LOCKED  ");
  }
  zonePrint(selectedZone);
}

void zonePrint(short zone) {
  lcd.setCursor(0, 0);
  lcd.print("ZONE ");
  char key = zone + zoneOffset;
  lcd.print(key);
  lcd.print(": ");
  if (zoneStatus[zone] == 0)
    lcd.print("DISARMED");
  else
    lcd.print("ARMED   ");
    
  lcd.setCursor(0, 1);
  lcd.print("DOOR: ");

  if (doorStatus == 0) {
    lcd.print("UNLOCKED");
  } else {
    lcd.print("LOCKED  ");
  }
}

void alarmControl(char key) {
  switch (key) {
    case 'A':
    case 'B':
    case 'C':
      selectedZone = key - zoneOffset;
      zonePrint(selectedZone);
      break;
    case 'D':
      tempDisplay = !tempDisplay;
      lcd.clear();
      if (tempDisplay == 0)
        zonePrint(selectedZone);
      break;
    case '#':
      break;
    case '*':
      break;
    default:
      keyPass[pressedKeys++] = key;
      if (pressedKeys == passLength) {
        pressedKeys = 0;
        for (int i = 0; i < passLength; i++) {
          if (setPass[i] != keyPass[i]) {
            Serial.println("Wrong password!");
            digitalWrite(BUZZER, 1);
            delay(200);
            digitalWrite(BUZZER, 0);
            delay(50);
            digitalWrite(BUZZER, 1);
            delay(200);
            digitalWrite(BUZZER, 0);
            break;
          } else if (i == passLength - 1) {
            //Serial.println("Correct password!");
            zoneStatus[selectedZone] = !zoneStatus[selectedZone];
            alarmTriggered=0;
            zonePrint(selectedZone);
            digitalWrite(BUZZER, 1);
            delay(100);
            digitalWrite(BUZZER, 0);
            delay(100);
            digitalWrite(BUZZER, 1);
            delay(100);
            digitalWrite(BUZZER, 0);
          }
        }
      }
      break;
  }
  if (zoneStatus[selectedZone] == 1 && alarmTriggered == 1)
    digitalWrite(BUZZER, 1);
  else{
    digitalWrite(BUZZER, 0);
    alarmTriggered=0;
  }
}
//0: GAS
//1: FIRE
void alarmTrigger(short triggerID) {
  switch (triggerID) {
    case 0:
      while (digitalRead(BUTTON_BLUE) == 1) {
        digitalWrite(LED_YELLOW, 0);
        digitalWrite(BUZZER, 1);
        delay(30);
        digitalWrite(BUZZER, 0);
        delay(30);
        digitalWrite(LED_YELLOW, 1);
        delay(100);
      }
      digitalWrite(BUZZER, 0);
      digitalWrite(LED_YELLOW, 0);
      break;
    case 1:
      while (digitalRead(BUTTON_RED) == 1) {
        digitalWrite(LED_RED, 0);
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(40);
        digitalWrite(BUZZER, 0);
        delay(30);
        digitalWrite(BUZZER, 1);
        delay(80);
        digitalWrite(BUZZER, 0);
        digitalWrite(LED_RED, 1);
        delay(100);
      }
      digitalWrite(BUZZER, 0);
      digitalWrite(LED_RED, 0);
      break;
    case 2:
      unsigned long timeTemp01 = millis();
      while (millis() - timeTemp01 <= 5000) {
        delay(100);
        digitalWrite(BUZZER, 1);
        delay(100);
        digitalWrite(BUZZER, 0);
      }
      break;
  }
}
//Source: https://stackoverflow.com/questions/32839396/how-to-get-the-uid-of-rfid-in-arduino
unsigned long getID() {
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return 0;
  }
  unsigned long hex_num;
  hex_num = mfrc522.uid.uidByte[0] << 24;
  hex_num += mfrc522.uid.uidByte[1] << 16;
  hex_num += mfrc522.uid.uidByte[2] << 8;
  hex_num += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA();  // Stop reading
  return hex_num;
}