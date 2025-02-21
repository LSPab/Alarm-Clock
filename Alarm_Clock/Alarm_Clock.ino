/*  
   Project: Arduino Alarm Clock  
   Author: Leo Santaella Prenne  
   Date: 25/02/07
   Description: This program sets an alarm using an RTC module,  
   displays time on an OLED, and activates a buzzer and motors.  
*/

#include <Wire.h>         // Library for I2C communication
#include <RTClib.h>       // Library for RTC module
#include "U8glib.h"       // Library for OLED display

#define SCREEN_WIDTH 128  // OLED screen width
#define SCREEN_HEIGHT 64  // OLED screen height

RTC_DS3231 rtc;  // Create RTC object
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK); // Initialize OLED display

// Define pin connections
const int potPin = A3;        // Potentiometer for setting time
const int buttonPin = 2;      // Button to confirm alarm time
const int buttonPin2 = 3;     // Button to stop the alarm
const int alarm = 5;          // Buzzer for alarm sound
const int motor1 = 9;         // Motor control pin 1
const int motor2 = 10;        // Motor control pin 2

// Variables for alarm time and status
int selectedTime = 0;    
bool timeLocked = false; // Time is not locked

void setup() {
  Serial.begin(9600);         // Start serial communication for debugging
  pinMode(alarm, OUTPUT);     // Set buzzer pin as output
  pinMode(buttonPin, INPUT);  // Set button 1 as input
  pinMode(buttonPin2, INPUT); // Set button 2 as input 
  pinMode(motor1, OUTPUT);    // Set motor pin 1 as output
  pinMode(motor2, OUTPUT);    // Set motor pin 2 as output

  // Check if RTC module is working
  if (!rtc.begin()) {
    Serial.println("RTC could not start!"); // Print error message if RTC fails
    while (1);
  }
}

void loop() {
  // Get current time from RTC module
  DateTime now = rtc.now(); 

  // Allow the user to set the alarm time using the potentiometer
  if (!timeLocked) {
    selectedTime = (analogRead(potPin) / 1023.0) * 1440; // Convert potentiometer value to minutes (24h format)
       selectedTime = (selectedTime / 10) * 10; // Round to nearest 10 min
  }

  // If button 1 is pressed, lock the selected time
  if (digitalRead(buttonPin) == LOW) {
    timeLocked = true;
  }

  // Convert current time to minutes for comparison
  int currentMinutes = now.hour() * 60 + now.minute();

  // If alarm time is reached, activate the buzzer and motors
  if (timeLocked && currentMinutes == selectedTime) {
    tone(alarm, 1000);         // Activate buzzer at 1000 Hz
    digitalWrite(motor1, HIGH); // Turn on motor 1
    digitalWrite(motor2, HIGH); // Turn on motor 2
  }

  // If button 2 is pressed, turn off alarm and motors
  if (digitalRead(buttonPin2) == LOW) {
    noTone(alarm);            // Stop buzzer
    digitalWrite(motor1, LOW); // Turn off motor 1
    digitalWrite(motor2, LOW); // Turn off motor 2
  }

  // Update the OLED display
  oledWrite(now.hour(), now.minute(), now.second(), selectedTime);

  delay(1000); // Wait 1 second before updating again
}

// Function to display current time and alarm time on OLED screen
void oledWrite(int hour, int minute, int second, int selectedTime) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont); // Set font for OLED text
    u8g.setPrintPos(0, 20);  // Set cursor position
    u8g.print("Time: " + String(hour) + ":" + String(minute) + ":" + String(second)); // Print current time

    u8g.setPrintPos(0, 40);  // Set cursor position
    u8g.print("Set Time: " + String(selectedTime / 60) + ":" + String(selectedTime % 60)); // Print selected alarm time
  } while (u8g.nextPage());  // Refresh OLED display
}
