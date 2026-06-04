#include <Arduino.h>
//#include <avr/eeprom.h>

// where does our characterMap start in the ASCII code
#define MAP_START      32
#define SPEED          35
#define DISPLAY_WIDTH  4
#define DISPLAY_HEIGHT 5

// maps characters to their 4x5 grid
unsigned long characterMap[59];

unsigned int timeout;
int offset = 0;
unsigned long lastMillis = 0;
unsigned long currentMillis = 0;

unsigned long graphic_t = 0b0;
unsigned long mil = 0;
byte counter = 0;
char myString[] = "#$^";
//char myString[] = "0 1 2 3 4 5 6 7 8 9";
int strLength = sizeof(myString);

byte x = 0;
byte y = 0;
int x_dir = 1;
int y_dir = 1;

int mode = 1;

// set up a character in the characterMap
void Chr(char theChar, unsigned long value) {
  characterMap[theChar - MAP_START] = value;
}

void scrolling_text();
void screen_saver();
int getNewDir();
void setLed(byte vin, byte gnd, boolean ledStatus);
void renderString(char *theString, int offset);
void matrix_fill_test() ;
void setPixel(byte x, byte y, boolean ledStatus);
void renderCharacter(char theChar, int charOffset);
void write_text();
void rainEffect();
void rows_effect();

void setup() {
#if defined(ARDUINO_AVR_UNO)
  Serial.begin(115200);
  Serial.println("System started");
#endif

int lastState = 1;//EEPROM.read(0);

if (lastState == 0)
{
  mode = 1;
  //EEPROM.update(0, mode);
}else if (lastState == 1)
{
  mode = 0;
//  EEPROM.update(0, mode);
}else
{
  //EEPROM.update(0, 1);
}

  // Rows:   1---2---3---4---5---
  Chr('A', 0b01101001111110011001);
  Chr('B', 0b11101001111010011110);
  Chr('C', 0b01111000100010000111);
  Chr('D', 0b11101001100110011110);
  Chr('E', 0b11111000111010001111);
  Chr('F', 0b11111000111010001000);
  Chr('G', 0b01111000101110010110);
  Chr('H', 0b10011001111110011001);
  Chr('I', 0b01110010001000100111);
  Chr('J', 0b01110010001010100100);
  Chr('K', 0b10011010110010101001);
  Chr('L', 0b10001000100010001111);
  Chr('M', 0b10011111111110011001);
  Chr('N', 0b10011101101110011001);
  Chr('O', 0b01101001100110010110);
  Chr('P', 0b11101001111010001000);
  Chr('Q', 0b01101001101101100001);
  Chr('R', 0b11101001111010101001);
  Chr('S', 0b11111000111100011111);
  Chr('T', 0b01110010001000100010);
  Chr('U', 0b10011001100110010110);
  Chr('V', 0b10011001100110100100);
  Chr('W', 0b10011001111111110110);
  Chr('X', 0b10011001011010011001);
  Chr('Y', 0b10011001011000101100);
  Chr('Z', 0b11110001001001001111);
  Chr(' ', 0b00000000000000000000);
  Chr('!', 0b01000100010000000100);
  Chr('1', 0b01001100010001001110);
  Chr('2', 0b11110001011010000111);
  Chr('3', 0b11100001011000011110);
  Chr('4', 0b00100110101011110010);
  Chr('5', 0b11111000111000011110);
  Chr('6', 0b01101000111010010110);
  Chr('7', 0b11110001001001001000);
  Chr('8', 0b01101001011010010110);
  Chr('9', 0b01101001011100010110);
  Chr('0', 0b01101001110110110110);
  Chr('#', 0b11111111111111111111);
  Chr('$', 0b00000000011000000000);
  Chr('^', 0b00000110011001100000);
  randomSeed(analogRead(A0));

  // how long to wait between shifting the display
  timeout = 1000 / SPEED;
  mil = millis();

}



void loop() {
  // scrolling text from myString
  // scrolling_text();
switch (mode)
{
case 0:
  matrix_fill_test();
  break;
case 1:
  write_text();
  break;
default:
  break;
}


  // rainEffect();
  // ping pong pretty effect
    // screen_saver();
  //singale letter at a time myString
  // write_text();

  // matrix fill test
  //  matrix_fill_test();
  //  rows_effect();

}

void rainEffect()
{
   byte x =random(0,DISPLAY_WIDTH);
  // for (byte x = 0 ; x < DISPLAY_WIDTH; x++ )
  // {
    for(int y = DISPLAY_HEIGHT-1; y >= 0 ; y--)
    {
      setPixel(x, y, HIGH);
      delay(3);
    }
    delay(10);
  // }

  // for (byte x = 0; x < DISPLAY_WIDTH ;x++ )
  // {
  //   for(byte y = 0; y < DISPLAY_HEIGHT; y++)
  //   {
  //     setPixel(x, y, LOW);
  //     // delay(50);
  //   }
  // }

}

void rows_effect(){
  for (byte y = 0; y < DISPLAY_HEIGHT ;y++)
  {
    for (byte x = 0 ; x < DISPLAY_WIDTH; x++)
    {
      setPixel(x, y, HIGH);
    }
    delay(100);
    for (byte x = 0 ; x < DISPLAY_WIDTH; x++)
    {
      setPixel(x, y, LOW);
    }
  }

  // for (byte x = 0 ; x < DISPLAY_WIDTH; x++ )
  // {
  //   for(byte y = DISPLAY_HEIGHT-1; y >= 0 ; y--)
  //   {
  //     setPixel(x, y, HIGH);
  //     delay(5);
  //   }
  //   delay(10);
  //   for(int y = DISPLAY_HEIGHT-1; y >= 0 ; y--)
  //   {
  //     setPixel(x, y, LOW);
  //   }
  // }
}




void scrolling_text() {
  currentMillis = millis();

  renderString(myString, offset);

  if (currentMillis - lastMillis > timeout) {
    lastMillis = currentMillis;
    // shift string over one "pixel"
    offset++;
    // if it's past the length of the string, start over from the beginning
    if (offset > strLength * (DISPLAY_WIDTH + 1)) {
      offset = -DISPLAY_WIDTH;
    }
  }
}


void screen_saver() {
  setPixel(x, y, 1);

  if (mil + 150 < millis()) {
    x += x_dir;
    y += y_dir;

    if (x <= 0 && x_dir == -1 ) {
      x_dir = 1;
      x = 0;
      //y_dir = getNewDir();
    }
    else if (x >= DISPLAY_WIDTH && x_dir == 1 )
    {
      x_dir = -1;
      x = DISPLAY_WIDTH - 2;
      //y_dir = getNewDir();
    }


    if (y <= 0 && y_dir == -1 ) {
      y_dir = 1;
      y = 0;
      //x_dir = getNewDir();
    }
    else if (y >= DISPLAY_HEIGHT && y_dir == 1 )
    {
      y_dir = -1;
      y = DISPLAY_HEIGHT - 2;
      //x_dir = getNewDir();
    }

    //DISPLAY_HEIGHT


    mil = millis();


  }

}

int getNewDir() {
  byte d = random(2);
  if (d)
    return 1;
  else
    return -1;
}

// light a pixel at the given coordinates
void setPixel(byte x, byte y, boolean ledStatus) {
  if (x >= 0 && x < DISPLAY_WIDTH) {
    if (y <= x) {
      x++;
    }
    setLed(y, x, ledStatus);
  }
}

// turn on the pins to light a LED
void setLed(byte vin, byte gnd, boolean ledStatus) {
  //delay(1);
  DDRB = 0;
  PORTB = 0;
  if (!ledStatus) return;

  pinMode(vin, OUTPUT);
  pinMode(gnd, OUTPUT);
  digitalWrite(vin, HIGH);
  digitalWrite(gnd, LOW);
}


// render the string on the given offset
void renderString(char *theString, int offset) {
  int index = 0;
  while (theString[index]) {
    renderCharacter(theString[index], offset - index * (DISPLAY_WIDTH + 1));
    index++;
  }
}

// render a character on the given offset
void renderCharacter(char theChar, int charOffset) {
  if (charOffset <= -DISPLAY_WIDTH || charOffset > DISPLAY_WIDTH) {
    // off the 'screen' nothing to do
    return;
  }

  unsigned long graphic = characterMap[theChar - MAP_START];

  for (byte y = 0; y < DISPLAY_HEIGHT; y++) {
    for (byte x = 0; x < DISPLAY_WIDTH; x++) {
      // 3 - x to reverse order
      setPixel(3 - x - charOffset, y, graphic & 0x1);
      graphic = graphic >> 1;
    }
  }
}

void test_screen(unsigned long graphic) {

  byte charOffset = 0;
  for (byte y = 0; y < DISPLAY_HEIGHT; y++) {
    for (byte x = 0; x < DISPLAY_WIDTH; x++) {
      // 3 - x to reverse order
      setPixel(3 - x - charOffset, y, graphic & 0x1);
      graphic = graphic >> 1;
    }
  }
}


void matrix_fill_test() {
  test_screen(graphic_t);
  if (mil + 50 < millis()) {
    graphic_t = graphic_t | (1UL << counter);
#if defined(ARDUINO_AVR_UNO)
    Serial.println(graphic_t, BIN);
#endif

    mil = millis();
    if (counter < 20) {
      counter++;
    }
    else
    {
      counter = 0;
      graphic_t = 0;
    }
  }
}

void write_text() {
  renderCharacter(myString[counter], 0);
  if (mil + 50 < millis()) {
    mil = millis();
    if (counter < strLength - 2) {
      counter++;
    }
    else
    {
      counter = 0;
    }
  }

}
