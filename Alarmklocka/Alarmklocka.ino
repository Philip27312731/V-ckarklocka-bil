// Project: Moving Alarm Clock
// Author: Philip Bernin
// Created on: 2025-02-07
//
// Description:
// This project is a moving alarm clock designed to wake up the user by driving away when the alarm is triggered.
// It uses an RTC module to keep track of time, relays to control motors, and a piezo buzzer to sound the alarm.
// The alarm can be stopped by pressing a button, which will turn off the motors and the buzzer.
// The system includes debounce logic to ensure reliable button press detection.

#include <Wire.h>
#include <RTClib.h>

// Initialize the RTC module
RTC_DS3231 rtc;

// Define pin connections
const int relay1Pin = 2;    
const int relay2Pin = 3;    
const int buttonPin = 4;    
const int piezoPin = 5;     

// Variables
bool alarmActive = false;   // Tracks if the alarm is active
bool alarmTriggered = false; // Tracks if the alarm was triggered

// Debounce variables
bool buttonState = false;
bool lastButtonState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; 

// Alarm time
const int time = 10;
const int alarmHour = (time + 1);    // Set alarm hour 
const int alarmMinute = 17;  // Set alarm minute

void setup() {
  
  Serial.begin(9600);

  // Setup pins
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(piezoPin, OUTPUT);

  // Turn off motors initially
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);

 
  Wire.begin(); 

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, setting the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  // Get current time from RTC
  DateTime now = rtc.now();

  // Display the current time in the Serial Monitor
  Serial.print("Current Time: ");
  Serial.print(now.hour() - 1);
  Serial.print(":");
  if (now.minute() < 10) Serial.print("0");
  Serial.print(now.minute());
  Serial.print(":");
  if (now.second() < 10) Serial.print("0"); 
  Serial.println(now.second());

  // Check if the alarm time is reached
  if (now.hour() == alarmHour && now.minute() == alarmMinute && !alarmTriggered) {
    alarmActive = true;   // Activate alarm
    alarmTriggered = true; 
    Serial.println("Alarm Activated!");
  }

  
  if (alarmActive) {
    runMotors();
    soundAlarm();
  } else {
    stopMotors(); 
  }

  // Check if the button is pressed to stop the alarm
  handleButtonPress();

  
  if (now.minute() != alarmMinute) {
    alarmTriggered = false;
  }

  delay(1000); 
}

void runMotors() {
  // Turn on relays to activate motors
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  Serial.println("Motors ON - Alarm driving away!");
}

void stopMotors() {
  // Turn off relays to deactivate motors
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  Serial.println("Motors OFF");
}

void soundAlarm() {
  // Continuously sound the buzzer
  tone(piezoPin, 1000); // 1kHz tone
  Serial.println("Buzzer ON - Wake up!");
}

void stopAlarm() {
  // Deactivate alarm
  alarmActive = false;

  // Turn off motors and buzzer
  stopMotors();
  noTone(piezoPin);
  Serial.println("Alarm Stopped - Button Pressed");
}

void handleButtonPress() {
  
  bool reading = digitalRead(buttonPin) == LOW; // Button pressed is LOW

  
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); 
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has stabilized
    if (reading && !buttonState) {
      // Button has just been pressed
      Serial.println("Button Pressed!");
      stopAlarm();
    }
    buttonState = reading; 
  }

  
  lastButtonState = reading;
}
