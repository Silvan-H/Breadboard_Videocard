// *********************************************************************************
// ***** Erase Flash Script                                                    *****
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
  
  digitalWrite(WRITE_EN, HIGH);  // WE high (inactive)
  digitalWrite(SHIFT_LATCH, LOW);
  
  // Set data pins as inputs initially
  for (int i = EEPROM_D0; i <= EEPROM_D7; i++) {
    pinMode(i, INPUT);
  }

  delay(1000);

  EraseFLASH();

  Serial.println("\n=== DONE ===");
}


void loop()
{
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

bool EraseFLASH()
{
  SetAddress(0, false);  // OE disabled
  
  // Chip Erase command sequence
  SetAddress(0x5555, false); WriteTo(0xaa); SET_WE(HIGH); SET_WE(LOW); SET_WE(HIGH);   // invoke 'Chip Erase' command 
  SetAddress(0x2aaa, false); WriteTo(0x55); SET_WE(HIGH); SET_WE(LOW); SET_WE(HIGH);
  SetAddress(0x5555, false); WriteTo(0x80); SET_WE(HIGH); SET_WE(LOW); SET_WE(HIGH);
  SetAddress(0x5555, false); WriteTo(0xaa); SET_WE(HIGH); SET_WE(LOW); SET_WE(HIGH);
  SetAddress(0x2aaa, false); WriteTo(0x55); SET_WE(HIGH); SET_WE(LOW); SET_WE(HIGH);
  SetAddress(0x5555, false); WriteTo(0x10); SET_WE(HIGH); SET_WE(LOW); SET_WE(HIGH);
  ToRead();
  SetAddress(0, true);  // OE enabled for polling
  
  // Wait for erase completion (poll bit 7)
  int c = 0;
  while ((READ_DATA & 0x80) != 0x80 && c < 2000) {
    c++;
    delayMicroseconds(100);
  }
  
  SetAddress(0, false);  // OE disabled
  return c < 2000;
}