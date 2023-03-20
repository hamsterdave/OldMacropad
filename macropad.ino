#include <Adafruit_SH110X.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_MCP23X17.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <RotaryEncoder.h>
#include <Wire.h>

Adafruit_MCP23X17 mcp; //Create GPIO Expander Object

// Create the OLED display
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &SPI1, OLED_DC, OLED_RST, OLED_CS);


/////Setup Encoder/////
RotaryEncoder encoder(PIN_ROTA, PIN_ROTB, RotaryEncoder::LatchMode::FOUR3);
void checkPosition() {  encoder.tick(); } // just call tick() to check the state.
// our encoder position state
int encoder_pos = 0;

volatile int encoder0Pos = 0;
volatile int encoder0Prev = 0;
volatile int encoder0Change = 0;
//----------//

// Create the neopixel strip with the built in definitions NUM_NEOPIXEL and PIN_NEOPIXEL
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_NEOPIXEL, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
int pixelNum = 0;
int radNum = 0;
bool keyChange = 0; //Tracks whether a key has changed state

const char *msg[]{"CQ", "EXCH", "TU", "MY CALL", "HIS CALL", "B4", "REPEAT", "AGN?", "SPARE", "QRZ", "SPARE", "WIPE"};
const char *slice[]{"SLICE A", "SLICE B"};

uint8_t j = 0;
bool i2c_found[128] = {false};

void setup() {

  Serial.begin(115200);
  delay(100);
  Mouse.begin();
  Keyboard.begin();
  Wire.begin();

    // Start OLED
  display.begin(0, true); // we dont use the i2c address but we will reset!

    // text display tests
  display.setTextSize(2);
  display.setTextWrap(false);
  display.setTextColor(SH110X_WHITE, SH110X_BLACK); // white text, black background
  display.display();
  display.clearDisplay();



  //----SETUP NEOPIXEL----//
  pixels.begin();
  pixels.setBrightness(150); //Set key brightness
  for(int i=0; i< pixels.numPixels(); i++) {  //All keys green
    pixels.setPixelColor(i, 0, 255, 0); 
  }
  pixels.show();

    // set all mechanical keys to inputs
  for (uint8_t i=0; i<=12; i++) {
    pinMode(i, INPUT_PULLUP);
  }

  //----SETUP ENCODER----//
  
  pinMode(PIN_ROTA, INPUT_PULLUP);  //Encoder pins
  pinMode(PIN_ROTB, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), checkPosition, CHANGE); //Encoder interrupts
  attachInterrupt(digitalPinToInterrupt(3), checkPosition, CHANGE);

  //--------//
}

void loop() {

    display.clearDisplay();
    display.setCursor(0,0);
    display.display();



//---- ENCODER SCROLL/VFO WHEEL BEHAVIOR -----//
encoder.tick();          // check the encoder
  int newPos = encoder.getPosition(); //If it changed position
  if (encoder_pos != newPos) {
    switch (encoder.getDirection()){  //Switch on getDirection and increment scroll wheel 1 in correct direction
      case RotaryEncoder::Direction::NOROTATION:
        break;
      case RotaryEncoder::Direction::CLOCKWISE:
        Mouse.move(0,0,-1);
        break;
      case RotaryEncoder::Direction::COUNTERCLOCKWISE:
        Mouse.move(0,0,1);
        break;
    }
    encoder_pos = newPos;
  }

//----KEYBOARD ASSIGNMENTS----//
  int keyNum = 0;
  for (int i=1; i<=12; i++){
    if (!digitalRead(i)) {
    keyNum = i;   
    }
  }
  if (keyNum>0) {
    pixelNum = keyNum - 1;
    switch (keyNum) {
      case 1:
        pixels.setPixelColor(pixelNum, 255, 0, 0);
        pixels.show();
        display.setTextSize(2);
        display.setCursor(0,0);
        display.println(msg[pixelNum]);
        display.println(slice[radNum]);
        display.display();
        Keyboard.write(193+keyNum);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(pixelNum, 0,255,0);
        pixels.show();
        keyNum = 0;
        radNum = 0;
        break;
      case 2:
        pixels.setPixelColor(1, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(25,20);
        display.println("EXCH");
        display.display();
        Keyboard.write(KEY_F2);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(1, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 3:
        pixels.setPixelColor(2, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(40,20);
        display.println("TU");
        display.display();
        Keyboard.write(KEY_F3);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(2, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 4:
        pixels.setPixelColor(3, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(25,25);
        display.println("MY CALL");
        display.display();
        Keyboard.write(KEY_F4);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(3, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 5:
        pixels.setPixelColor(4, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20,25);
        display.println("HIS CALL");
        display.display();
        Keyboard.write(KEY_F5);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(4, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 6:
        pixels.setPixelColor(5, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(40,25);
        display.println("B4");
        display.display();
        Keyboard.write(KEY_F6);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(5, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 7:
        pixels.setPixelColor(6, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(35,25);
        display.println("RPT");
        display.display();
        Keyboard.write(KEY_F7);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(6, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 8:
        pixels.setPixelColor(7, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(35,20);
        display.println("AGN?");
        display.display();
        Keyboard.write(KEY_F8);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(7, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 9:
        pixels.setPixelColor(8, 0, 0, 255);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(35,20);
        display.println("SPARE");
        display.display();
        Keyboard.write(KEY_F8);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(8, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 10:
        pixels.setPixelColor(9, 0, 0, 255);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(35,20);
        display.println("SPARE");
        display.display();
        Keyboard.write(KEY_F8);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(9, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 11:
        pixels.setPixelColor(10, 0, 0, 255);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(35,20);
        display.println("SPARE");
        display.display();
        Keyboard.write(KEY_F8);
        delay(500);
        display.clearDisplay();
        pixels.setPixelColor(10, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
      case 12:
        pixels.setPixelColor(11, 255, 0, 0);
        pixels.show();
        display.clearDisplay();
        display.setTextSize(3);
        display.setCursor(30,20);
        display.println("WIPE");
        display.display();
        Keyboard.write(KEY_F12);
        display.clearDisplay();
        for(int i=0; i<10; i++){
          pixels.setPixelColor(11, 255,0,0);
          pixels.show();
          delay(100);
          pixels.setPixelColor(11, 255,255,255);
          pixels.show();
          delay(50);
        }
        pixels.setPixelColor(11, 0,255,0);
        pixels.show();
        keyNum = 0;
        break;
    }
    keyNum = 0;
  } 






}
