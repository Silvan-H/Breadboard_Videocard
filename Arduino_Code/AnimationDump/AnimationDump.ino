// *********************************************************************************
// ***** Write Animation to Flash (200x150 Pixels 8 frames)                    *****
// ***** Using Binary File prepared for breadboard videocard                   *****
// *********************************************************************************

#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define READ_DATA         readDataBus()

void setup()
{
  Serial.begin(115200);
  
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(WRITE_EN, OUTPUT);
  
  digitalWrite(WRITE_EN, HIGH);  // WE high (inactive)
  digitalWrite(SHIFT_LATCH, LOW);
  
  // Set data pins as inputs initially
  for (int i = EEPROM_D0; i <= EEPROM_D7; i++) {
    pinMode(i, INPUT);
  }

  delay(1000);
  AnimationDump();

}


void loop()
{
}

uint32_t getAddress(int x, int y, byte f) 
{
    uint32_t adr = (uint32_t) f << 16 | ((uint32_t)y << 8) | (uint32_t)x;
    return adr;
}
void SetAddress(long adr, bool outputEnable)
{
  uint8_t byte2 = (adr >> 16) & 0x7F;  // A16-A22 (7 bits)
  uint8_t byte1 = (adr >> 8) & 0xFF;   // A8-A15
  uint8_t byte0 = adr & 0xFF;          // A0-A7

  // MSB bit of byte2 controls OE (active low)
  if (!outputEnable) {
    byte2 |= 0x80;
  }

  // Shift out MSB first (byte2, byte1, byte0)
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, byte2);
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, byte1);
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, byte0);

  // Latch the data
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}

void ToRead()
{
  // Set data bus pins as inputs
  for (int i = EEPROM_D0; i <= EEPROM_D7; i++) {
    pinMode(i, INPUT);
  }
}

void WriteTo(byte data)
{
  // Set data bus pins as outputs
  for (int i = EEPROM_D0; i <= EEPROM_D7; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, (data >> (i - EEPROM_D0)) & 1);
  }
}

byte readDataBus()
{
  byte data = 0;
  for (int i = EEPROM_D0; i <= EEPROM_D7; i++) {
    if (digitalRead(i)) {
      data |= (1 << (i - EEPROM_D0));
    }
  }
  return data;
}

void AnimationDump()
{
  Serial.println();
  Serial.println(F("=== Animation DUMP ( 8 x 200 x 150) ==="));

  ToRead();   // Set data bus as inputs

  for (int f = 0; f < 8; f++){
      Serial.print(" Frame === ");
      Serial.print(f);
      Serial.print(" ===");

    for (int y = 0; y < 150; y++)
    {
      Serial.print("Y=");
      Serial.print(y);
      Serial.print(": ");

      for (int x = 0; x < 200; x++)
      {
        uint32_t adr = getAddress(x, y, f);

        SetAddress(adr, true);
        byte data = READ_DATA;

        // Print hex with leading zero
        if (data < 16) Serial.print("0");
        Serial.print(data, HEX);
        Serial.print(" ");
      }

      Serial.println(); // end of row
    }
  }
  Serial.println(F("=== END Animation DUMP ==="));
}
