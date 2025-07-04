#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ------------------- Display ------------------- //

// OLED display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Reset pin is not used for I2C
#define OLED_RESET     -1

// Create display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// stores the humidity for each pot; -1 -> Pot not connected
int Humidity[3] = {-1, -1, -1};

// temp for plants that will be watered
bool WaterSchedule[3] = {false, false, false};

// stores the (absolute) time each plant was last watered 
unsigned long LastWatered[3] = {0, 0, 0};

// The humidity treshhold for watering in % (assuming 5v input on the sensor)
unsigned int humidity_treshhold = 41;

// The min delay between watering (6h)
unsigned long min_duration = 6UL * 60UL * 60UL * 1000UL; // UL = unsigned long | Formula: hours per delay * minutes in a hour * seconds in a minute * miliseconds in a second


// Motordriver pins
const int motor_1 = 3; // set later
const int motor_2 = 4; // set later
const int motor_homing = 2; // the homing button for the motor 


// the analog pin the Humidity sensor is connected to
const int Pot0_Humidity = A1;
const int Pot1_Humidity = A2;
const int Pot2_Humidity = A3;

// the digital pin the "return" wire of the pot is connected to
const int Pot0_Return = 13;
const int Pot1_Return = 7;
const int Pot2_Return = 6;


// The 5 Buttons under the display
const int CTRL_Button_UP = 8;
const int CTRL_Button_LEFT = 9;
const int CTRL_Button_DOWN = 10;
const int CTRL_Button_RIGHT = 11;
const int CTRL_Button_MIDDLE = 12;

const int Pump_resevoir = 5; 
const int Pump_pots = -1;


// gets the humidity value of a pot
int get_humidity(int index) {
  int humidity = -1;

  // Bro i didnt even know c had switch statements thats crazy
  switch (index) {
    case 0:
      // if the pot is connected, read the humidity, else return -1
      if (digitalRead(Pot0_Return) == 1) {
        humidity = 100 - ((analogRead(Pot0_Humidity) * 100.0) / 1023.0);
        Serial.println("0 connected");
        Serial.println(humidity);
        return humidity;
      }
      else {
        Serial.println("0 not connected");
        return -1;
      }
      break;

    case 1:
      // if the pot is connected, read the humidity, else return -1
      if (digitalRead(Pot1_Return) == 1) {
        humidity = 100 - ((analogRead(Pot1_Humidity) * 100.0) / 1023.0);
        Serial.println("1 connected");
        Serial.println(humidity);
        return humidity;
      }
      else {
        Serial.println("1 not connected");
        return -1;
      }
      break;

    case 2:
      // if the pot is connected, read the humidity, else return -1
      if (digitalRead(Pot2_Return) == 1) {
        humidity = 100 - ((analogRead(Pot2_Humidity) * 100.0) / 1023.0);
        Serial.println("2 connected");
        Serial.println(humidity);
        return humidity;
      }
      else {
        Serial.println("2 not connected");
        return -1;
      }
      break;

    // if not already, return humidity
    return humidity;
  }
}

void home_motor() {

  bool homed = LOW;

  while(homed == LOW) {
    homed = digitalRead(motor_homing);
    digitalWrite(motor_1, HIGH);
  }

  // Drive a bit backwards
  digitalWrite(motor_1, LOW);
  digitalWrite(motor_2, HIGH);
  delay(200);
  digitalWrite(motor_2, LOW);
}


// makes the motor drive to the right pot and water it
void water_plants(int Pots) {
  home_motor();
}


void setup() {

  pinMode(Pot0_Return, INPUT);
  pinMode(Pot1_Return, INPUT);
  pinMode(Pot2_Return, INPUT);

  pinMode(Pot0_Humidity, INPUT);
  pinMode(Pot1_Humidity, INPUT);
  pinMode(Pot2_Humidity, INPUT);

  pinMode(CTRL_Button_UP, INPUT);
  pinMode(CTRL_Button_LEFT, INPUT);
  pinMode(CTRL_Button_DOWN, INPUT);
  pinMode(CTRL_Button_RIGHT, INPUT);
  pinMode(CTRL_Button_MIDDLE, INPUT);

  pinMode(motor_homing, INPUT);

  pinMode(Pump_resevoir, OUTPUT);

  pinMode(motor_1, OUTPUT);
  pinMode(motor_2, OUTPUT);


  Serial.begin(9600);


  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Default I2C address is 0x3C
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // Infinite loop
  }

    // Clear display buffer
  display.clearDisplay();

  // Set text properties
  display.setTextSize(1);      // Size: 1=small, 2=medium, etc.
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);     // Top-left corner

  // Display text
  display.println("Booting");

  // Show content on screen
  display.display();

}


void loop() {
  //-------------- check wich pots are connected && log humidity data -> Humidity[3] --------------//
  
  // gather humidity for all 3 pots
  for (int i = 0; i < 3; i++) {
    Humidity[i] = get_humidity(i);
  }


  //-------------- Display the humidity data on our display --------------//
  display.clearDisplay();

  // Set text properties
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0); // 0,0 == Top-left corner

  // Display text
  display.print((digitalRead(Pot0_Return) != 0) ? "Humidity:" : "Not connected");
  display.print(" ");
  if (digitalRead(Pot0_Return) != 0) {
      display.print(Humidity[0]);
      display.println("%");
  }
  else {
    display.println("");
  }

  display.print((digitalRead(Pot1_Return) != 0) ? "Humidity:" : "Not connected");
  display.print(" ");
  if (digitalRead(Pot1_Return) != 0) {
      display.print(Humidity[1]);
      display.println("%");
  }
  else {
    display.println("");
  }
  display.print((digitalRead(Pot2_Return) != 0) ? "Humidity:" : "Not connected");
  display.print(" ");
  if (digitalRead(Pot2_Return) != 0) {
      display.print(Humidity[2]);
      display.println("%");
  }
  else {
    display.println("");
  }

  // Show content on screen
  display.display();



  //-------------- If a pot is connected && humidity under treshhold then water it --------------//

  unsigned long currentTime = millis(); // set current time (relative to boot)

  // comment for future me (yes youre literally that dumb): yes millis will overflow after 50 days, no it wont destroy this section right here because the superior older you is a literal 10x developer, look at this clean ass code, no need to worry / panick
  for (int i = 0; i < 3; i++) {
      if (Humidity[i] < humidity_treshhold && (currentTime - LastWatered[i]) > min_duration) {
        WaterSchedule[i] = true;
        LastWatered[i] = millis(); // update when we last watered the plant
      }
  }
  

  // water_plants(WaterSchedule);
  
  // after 3 waterings clean reseavoiua


  //-------------- Debug remove this later --------------//

  bool DBG_Button_pressed = digitalRead(CTRL_Button_MIDDLE);

  while (DBG_Button_pressed == HIGH) {
    DBG_Button_pressed = digitalRead(CTRL_Button_MIDDLE);
    digitalWrite(Pump_resevoir, HIGH);
    delay(10);
  }
  digitalWrite(Pump_resevoir, LOW);

}
