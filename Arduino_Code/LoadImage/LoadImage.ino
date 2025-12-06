// *********************************************************************************
// ***** Write Image to Flash (200x150 Pixels)                                 *****
// ***** Using Binary File prepared for breadboard videocard                   *****
// *********************************************************************************

#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define SET_WE(state)     digitalWrite(WRITE_EN, state)
#define READ_DATA         readDataBus()

void setup()
{
  Serial.begin(115200);

  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(WRITE_EN, OUTPUT);

  digitalWrite(WRITE_EN, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);

  for (int i = EEPROM_D0; i <= EEPROM_D7; i++) {
    pinMode(i, INPUT);
  }

  delay(1000);

  Serial.println("\n=== Flash Image Loader (Serial Mode) ===");
  Serial.println("READY");
  
  unsigned long index = 0;
  
  for (int y = 0; y < 150; y++) {
    for (int x = 0; x < 200; x++) {
      
      // Wait for byte
      while (!Serial.available()) {
        // Just wait
      }
      
      byte pixel = Serial.read();
      uint16_t adr = getAddress(x, y);
      
      WriteFLASH((long)adr, pixel);
      
      // Send ACK every byte (or every N bytes for speed)
      if (index % 64 == 0) {  // ACK every 10 bytes
        Serial.write('K');  // Simple ACK character
      }
      
      index++;
    }
  }

  Serial.println("\n=== Image Write Complete ===");

  Serial.write("\n=== DONE ===");
}


void loop()
{
  // Nothing - test runs once in setup
}

uint16_t getAddress(int x, int y) 
{
    uint16_t adr = ((uint16_t)y << 8) | (uint16_t)x;
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

bool WriteFLASH(long adr, byte data)
{
  SET_WE(HIGH);
  SetAddress(0, false);  // OE disabled
  
  // Byte Program command sequence
  SetAddress(0x5555, false); WriteTo(0xaa); SET_WE(HIGH); SET_WE(LOW); SET_WE(LOW); SET_WE(HIGH);
  SetAddress(0x2aaa, false); WriteTo(0x55); SET_WE(HIGH); SET_WE(LOW); SET_WE(LOW); SET_WE(HIGH);
  SetAddress(0x5555, false); WriteTo(0xa0); SET_WE(HIGH); SET_WE(LOW); SET_WE(LOW); SET_WE(HIGH);
  SetAddress(adr, false); WriteTo(data); SET_WE(HIGH); SET_WE(LOW); SET_WE(LOW); SET_WE(HIGH);
  
  ToRead();
  SetAddress(adr, true);  // OE enabled for data polling
  
  // Wait for write completion (poll bit 7)
  int c = 0;
  while (((READ_DATA & 0x80) != (data & 0x80)) && (c < 100)) c++;
  
  SetAddress(0, false);  // OE disabled
  return c < 100;
}
