/*****************************************************
      SMART PARKING MANAGEMENT SYSTEM
              Arduino UNO V2
*****************************************************/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#include <SoftwareSerial.h>

SoftwareSerial espSerial(10, 11);

/************* LCD ****************/
LiquidCrystal_I2C lcd(0x27,16,2);

/************* Servo **************/
Servo gateServo;

/************* Pin Definition **************/

// Parking IR Sensors
const byte SLOT1 = 2;
const byte SLOT2 = 3;
const byte SLOT3 = 4;
const byte SLOT4 = 5;

// Ultrasonic
const byte TRIG = 6;
const byte ECHO = 7;

// Exit IR
const byte EXIT_IR = 8;

// Servo
const byte SERVO = 9;

/************* Servo Position **************/

const int GATE_OPEN = 90;
const int GATE_CLOSE = 0;

/************* Ultrasonic **************/

const int ENTRY_DISTANCE = 10;      // 10 cm

long duration;
float distance;

/************* Slot Status **************/

bool slot1 = true;
bool slot2 = true;
bool slot3 = true;
bool slot4 = true;

int availableSlots = 4;

String recommendSlot = "S1";

/************* Gate Status **************/

bool gateOpen = false;
bool entryDetected = false;
bool exitDetected = false;

/************* Timer **************/

unsigned long lcdTimer = 0;
unsigned long espTimer = 0;

const unsigned long LCD_DELAY = 500;
const unsigned long ESP_DELAY = 1000;

/*****************************************************
                    SETUP
*****************************************************/

void setup()
{

Serial.begin(9600);
espSerial.begin(9600);
Serial.println("Arduino Started");

/******** Pin Mode ********/

pinMode(SLOT1,INPUT);
pinMode(SLOT2,INPUT);
pinMode(SLOT3,INPUT);
pinMode(SLOT4,INPUT);

pinMode(TRIG,OUTPUT);
pinMode(ECHO,INPUT);

pinMode(EXIT_IR,INPUT);

/******** Servo ********/

gateServo.attach(SERVO);

gateServo.write(GATE_CLOSE);

/******** LCD ********/

lcd.init();

lcd.backlight();

lcd.clear();

lcd.setCursor(0,0);
lcd.print("SMART PARKING");

lcd.setCursor(1,1);
lcd.print("Initializing");

delay(2000);

lcd.clear();

}

/*****************************************************
            READ PARKING SLOT STATUS
*****************************************************/
void readSlots()
{
  // HIGH = Empty
  // LOW  = Occupied
  // Agar tumhare sensor ulte nikle to sirf ! hata dena

  slot1 = digitalRead(SLOT1);
  slot2 = digitalRead(SLOT2);
  slot3 = digitalRead(SLOT3);
  slot4 = digitalRead(SLOT4);

  availableSlots = 0;

  if(slot1) availableSlots++;
  if(slot2) availableSlots++;
  if(slot3) availableSlots++;
  if(slot4) availableSlots++;
}

/*****************************************************
        FIND RECOMMENDED SLOT
*****************************************************/
void findRecommendSlot()
{
  if(slot1)
    recommendSlot = "S1";

  else if(slot2)
    recommendSlot = "S2";

  else if(slot3)
    recommendSlot = "S3";

  else if(slot4)
    recommendSlot = "S4";

  else
    recommendSlot = "FULL";
}

/*****************************************************
          ULTRASONIC DISTANCE
*****************************************************/
float getDistance()
{
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);

  distance = duration * 0.0343 / 2.0;

  return distance;
}

/*****************************************************
          GATE OPEN FUNCTION
*****************************************************/
void openGate()
{
  if(!gateOpen)
  {
    gateServo.write(GATE_OPEN);
    gateOpen = true;
  }
}

/*****************************************************
          GATE CLOSE FUNCTION
*****************************************************/
void closeGate()
{
  if(gateOpen)
  {
    gateServo.write(GATE_CLOSE);
    gateOpen = false;
  }
}

/*****************************************************
                LCD UPDATE
*****************************************************/
void updateLCD()
{
  lcd.clear();

  // First Line
  lcd.setCursor(0,0);
  lcd.print("Free:");
  lcd.print(availableSlots);

  lcd.setCursor(10,0);
  lcd.print(recommendSlot);

  // Second Line
  lcd.setCursor(0,1);

  lcd.print(slot1 ? "1:E " : "1:F ");
  lcd.print(slot2 ? "2:E " : "2:F ");

  lcd.setCursor(8,1);

  lcd.print(slot3 ? "3:E " : "3:F ");
  lcd.print(slot4 ? "4:E" : "4:F");
}

/*****************************************************
        CREATE DATA FOR ESP32
*****************************************************/
String createPacket()
{
  String data = "";

  data += String(slot1);
  data += ",";

  data += String(slot2);
  data += ",";

  data += String(slot3);
  data += ",";

  data += String(slot4);
  data += ",";

  data += String(availableSlots);
  data += ",";

  data += recommendSlot;

  return data;
}

/*****************************************************
        SEND DATA TO ESP32
*****************************************************/
void sendData()
{
  espSerial.println(createPacket());
  Serial.println(createPacket());
}

/*****************************************************
      SHOW PARKING FULL MESSAGE
*****************************************************/
void parkingFullMessage()
{
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(" PARKING FULL ");

  lcd.setCursor(0,1);
  lcd.print(" Please Wait ");
}

/*****************************************************
            ENTRY GATE LOGIC
*****************************************************/
void entryGate()
{
  distance = getDistance();

  // Car Entry Detected
  if(distance > 0 && distance <= ENTRY_DISTANCE)
  {
    if(availableSlots > 0)
    {
      openGate();
      entryDetected = true;
    }
    else
    {
      parkingFullMessage();
    }
  }

  // Car Crossed Sensor
  if(entryDetected && distance > ENTRY_DISTANCE)
  {
    delay(1000);

    closeGate();

    entryDetected = false;
  }
}

/*****************************************************
            EXIT GATE LOGIC
*****************************************************/
void exitGate()
{
  // Car reached Exit IR
  if(digitalRead(EXIT_IR) == LOW)
  {
    if(!exitDetected)
    {
      openGate();

      exitDetected = true;
    }
  }

  // Car Passed Exit Sensor
  if(exitDetected && digitalRead(EXIT_IR) == HIGH)
  {
    delay(1000);

    closeGate();

    exitDetected = false;
  }
}

/*****************************************************
                    MAIN LOOP
*****************************************************/

void loop()
{

  // Read All Parking Slots
  readSlots();

  // Find Recommended Slot
  findRecommendSlot();

  // Entry Gate
  entryGate();

  // Exit Gate
  exitGate();

  // LCD Update Every 500 ms
  if(millis() - lcdTimer >= LCD_DELAY)
  {
    lcdTimer = millis();

    if(availableSlots > 0)
      updateLCD();
    else
      parkingFullMessage();
  }

  // Send Data to ESP32 Every 1 Second
  if(millis() - espTimer >= ESP_DELAY)
  {
    espTimer = millis();

    sendData();
  }

}
