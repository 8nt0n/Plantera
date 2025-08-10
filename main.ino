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
const int CTRL_Button_UP = 9;
const int CTRL_Button_LEFT = 8;
const int CTRL_Button_DOWN = 10;
const int CTRL_Button_RIGHT = 12;
const int CTRL_Button_MIDDLE = 11;

const int Pump_resevoir = 5; 
const int Pump_pots = A0; // bruh random ahhh pin 😭

int water_time = 1000; // water time per pot in ms

void water(int time) { // yesss too lazy to do some crazy ml calculation shit...
  digitalWrite(Pump_pots, HIGH);
  delay(time);
  digitalWrite(Pump_pots, LOW);
}

void drive_right(int steps) {
  digitalWrite(motor_1, HIGH);
  digitalWrite(motor_2, LOW);
  delay(steps);
  digitalWrite(motor_1, LOW);
}

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

    // you shouldnt ever get to this line but im gonna add a brief print statement just incase
    Serial.println("Anton you fucking retard this was not supposed to happen");
  }
}

void home_motor() {

  // What the actual fuck i thought it was low and wasted 30 min of my life omg stupid retard
  bool homed = HIGH;

  while(homed == HIGH) {
    homed = digitalRead(motor_homing);
    digitalWrite(motor_1, LOW);
    digitalWrite(motor_2, HIGH);
    delay(10); 
  }

  // Drive a bit to the right
  drive_right(500);
  return;
}


// makes the motor drive to the right pot and water it
void water_plants(bool Pots[3]) {

  home_motor();

  if (Pots[0] == true) {
    // water
    water(water_time);
  }


  // drive to pot 1 if pot 1 or 2 needs to be watered
  if (Pots[1] == true || Pots[2] == true) { 
    drive_right(10000);
  }


  if (Pots[1] == true) { 
    //water only if needed if it needs to
    water(water_time);
  }

  if (Pots[2] == true) {
    // only drive to pot 2 if we need to
    drive_right(11000);
    // and water
    water(water_time);
  }

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
  pinMode(Pump_pots, OUTPUT);

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
      if (Humidity[i] != -1 && Humidity[i] < humidity_treshhold && (currentTime - LastWatered[i]) > min_duration) {
        WaterSchedule[i] = true; // water plant
        LastWatered[i] = millis(); // update when we last watered the plant
      }
      else {
        WaterSchedule[i] = false;
      }
  }
  

  // another geeenius code snippet bro (only water plants if at least one pot has to be watered)
  if (WaterSchedule[0] || WaterSchedule[1] || WaterSchedule[2]) {
    water_plants(WaterSchedule);

    // reset WaterSchedule to false false false after watering
    for (int i = 0; i < 3; i++) {
      WaterSchedule[i] = false;
    }
  }
  
  //-------------- after 9 waterings clean reseavoiua --------------//





  


  //-------------- Debug remove this later --------------//

  bool DBG_Button_pressed;

  // clean resevoir
  DBG_Button_pressed = digitalRead(CTRL_Button_MIDDLE);

  while (DBG_Button_pressed == HIGH) {
    DBG_Button_pressed = digitalRead(CTRL_Button_MIDDLE);
    digitalWrite(Pump_resevoir, HIGH);
    delay(10);
  }
  digitalWrite(Pump_resevoir, LOW);


  // drive right
  DBG_Button_pressed = digitalRead(CTRL_Button_LEFT);
  
  if (DBG_Button_pressed == HIGH) {
    drive_right(1000);
    while(DBG_Button_pressed == HIGH) {
      delay(10);
      DBG_Button_pressed = digitalRead(CTRL_Button_LEFT);
    }
  }

  // home
  DBG_Button_pressed = digitalRead(CTRL_Button_RIGHT);
  
  if (DBG_Button_pressed == HIGH) {
    home_motor();
    while(DBG_Button_pressed == HIGH) {
      delay(10);
      DBG_Button_pressed = digitalRead(CTRL_Button_RIGHT);
    }
  }

  // water
  DBG_Button_pressed = digitalRead(CTRL_Button_UP);
  
  if (DBG_Button_pressed == HIGH) {
    water(water_time);
    while(DBG_Button_pressed == HIGH) {
      delay(10);
      DBG_Button_pressed = digitalRead(CTRL_Button_UP);
    }
  }

}
