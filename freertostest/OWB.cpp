#include "OWB.h"

namespace owb{

OneWire oneWire(DS2408_ONEWIRE_PIN);
uint8_t address[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  
void printBytes(uint8_t* addr, uint8_t count, bool newline=0) 
{
  for (uint8_t i = 0; i < count; i++) 
  {
    Serial.print(addr[i]>>8, HEX);
    Serial.print(addr[i]&0x0f, HEX);
    Serial.print(" ");
  }
  if (newline)
  {
    Serial.println();
  }
}

byte read(void)
{    
  bool ok = false;
  uint8_t results;

  oneWire.reset();
  oneWire.select(address);
  oneWire.write(DS2408_ACCESS_READ);

  results = oneWire.read();                 /* Get the register results   */
  ok = (!results & 0x0F) == (results >> 4); /* Compare nibbles            */
  //results &= 0x0F;                          
                                            /* Clear inverted values      */

  oneWire.reset();
  
  // return ok ? results : -1;
  return results;
}


bool write(uint8_t state)
{
  uint8_t ack = 0;
  
  /* Top six bits must '1' */
  state |= 0xFC;
  
  oneWire.reset();
  oneWire.select(address);
  oneWire.write(DS2408_ACCESS_WRITE);
  oneWire.write(state);
  oneWire.write(~state);                    /* Invert data and resend     */    
  ack = oneWire.read();                     /* 0xAA=success, 0xFF=failure */  
  if (ack == DS2408_ACK_SUCCESS)
  {
    oneWire.read();                          /* Read the status byte      */
  }
  oneWire.reset();
    
  return (ack == DS2408_ACK_SUCCESS ? true : false);
}


void OWB_setup()
{ 
  
  Serial.println(F("Looking for a DS2408 on the bus"));
  
  /* Try to find a device on the bus */
  oneWire.reset_search();
  delay(250);
  if (!oneWire.search(address)) 
  {
    printBytes(address, 8);
    Serial.println(F("No device found on the bus!"));
    oneWire.reset_search();
    while(1);
  }
  
  /* Check the CRC in the device address */
  if (OneWire::crc8(address, 7) != address[7]) 
  {
    Serial.println(F("Invalid CRC!"));
    while(1);
  }
  
  /* Make sure we have a DS2408 */
  if (address[0] != DS2408_FAMILY_ID) 
  {
    printBytes(address, 8);
    Serial.println(F(" is not a DS2413!"));
    while(1);
  }
  
  Serial.print(F("Found a DS2408: "));
  printBytes(address, 8);
  Serial.println(F(""));
 }

  byte buttons(byte& BUTTONS){
    BUTTONS = read();
    //Serial.println(BUTTONS,BIN);
    return BUTTONS;
  }

}
