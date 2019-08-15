// McpDigitalPot 2-channel Digital Potentiometer
// ww1.microchip.com/downloads/en/DeviceDoc/22059b.pdf

// The default SPI Control Register - SPCR = B01010000;
// interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
// sample on leading edge of clk,system clock/4 rate (fastest).
// Enable the digital pins 11-13 for SPI (the MOSI,MISO,SPICLK)
#include <SPI.h>
#include "McpDigitalPot.h"

//---------- constructor ----------------------------------------------------

McpDigitalPot::McpDigitalPot(uint8_t slave_select_pin, float rAB_ohms)
{
  initSpi(slave_select_pin);
  initResistance(rAB_ohms, rW_ohms_typical);
}

McpDigitalPot::McpDigitalPot(uint8_t slave_select_pin, float rAB_ohms, float rW_ohms)
{
  initSpi(slave_select_pin);
  initResistance(rAB_ohms, rW_ohms);
}

//------------------ protected -----------------------------------------------

uint16_t McpDigitalPot::byte2uint16(byte high_byte, byte low_byte)
{
  return (uint16_t)high_byte<<8 | (uint16_t)low_byte;
}

byte McpDigitalPot::uint16_high_byte(uint16_t uint16)
{
  return (byte)(uint16>>8);
}

byte McpDigitalPot::uint16_low_byte(uint16_t uint16)
{
  return (byte)(uint16 & 0x00FF);
}

void McpDigitalPot::initSpi(uint8_t slave_select_pin)
{
  // Set slave select (Chip Select) pin for SPI Bus, and start high (disabled)
  ::pinMode(slave_select_pin,OUTPUT);
  ::digitalWrite(slave_select_pin,HIGH);
  this->slave_select_pin = slave_select_pin;
}

void McpDigitalPot::initResistance(float rAB_ohms, float rW_ohms)
{
  this->rAB_ohms             = rAB_ohms;
  this->rW_ohms              = rW_ohms;
  this->rAW_ohms_max         = rAB_ohms - rW_ohms;
  this->scale                = rAW_ohms_max;
}

float McpDigitalPot::wiperPositionIncrement()
{
  return (rAW_ohms_max - rW_ohms) / resolution;
}

unsigned int McpDigitalPot::resistanceToPosition(float resistance)
{
  if(resistance <= 0.0)
    return 0;
  else if(scale != rAW_ohms_max)
    resistance = resistance * rAW_ohms_max / scale;

  return (unsigned int)((resistance - rW_ohms) / wiperPositionIncrement() ) + 0.5;
}

float McpDigitalPot::positionToResistance(unsigned int position)
{
  float resistance =  rW_ohms + ( (float)position * wiperPositionIncrement() );

  if(scale != rAW_ohms_max)
    resistance = resistance * scale / rAW_ohms_max;
  
  return resistance;
}

void McpDigitalPot::spiWrite(byte cmd_byte, byte data_byte)
{
  cmd_byte |= kCMD_WRITE;
  ::digitalWrite(slave_select_pin, LOW);
  byte high_byte = SPI.transfer(cmd_byte);
  byte low_byte  = SPI.transfer(data_byte);
  ::digitalWrite(slave_select_pin, HIGH);
  bool result = ~low_byte;
}

uint16_t McpDigitalPot::spiRead(byte cmd_byte)
{
  cmd_byte |= kCMD_READ;
  ::digitalWrite(slave_select_pin, LOW);
  byte high_byte = SPI.transfer(cmd_byte);
  byte low_byte  = SPI.transfer(0xFF);
  ::digitalWrite(slave_select_pin, HIGH);
  return byte2uint16(high_byte, low_byte);
}

void McpDigitalPot::internalSetWiperPosition(byte wiperAddress, unsigned int position, bool isNonVolatile)
{
  byte cmd_byte    = 0x00;
  byte data_byte   = 0x00;
  cmd_byte        |= wiperAddress;

  // Calculate the 9-bit data value to send
  if(position > 255)
    cmd_byte      |= B00000001; // Table 5-1 (page 36)
  else
    data_byte      = (byte)(position & 0x00FF);

  spiWrite(cmd_byte|kADR_VOLATILE, data_byte);

  if(isNonVolatile)
  {
    // EEPROM write cycles take 4ms each. So we block with delay(5); after any NV Writes
    spiWrite(cmd_byte|kADR_NON_VOLATILE, data_byte);
    delay(5);
  }
}

//---------- public ----------------------------------------------------

float McpDigitalPot::getResistance(unsigned int wiperIndex)
{
  return positionToResistance( getPosition(wiperIndex) );
}

unsigned int McpDigitalPot::getPosition(unsigned int wiperIndex)
{
  if (wiperIndex == 1) {
     return 0x01FF & this->spiRead(kADR_WIPER1|kADR_VOLATILE);
  } else {
     return (unsigned int)( 0x01FF & this->spiRead(kADR_WIPER0|kADR_VOLATILE) );
  }
}

void McpDigitalPot::setResistance(unsigned int wiperIndex, float resistance)
{
  setPosition( wiperIndex, resistanceToPosition(resistance) );
}

void McpDigitalPot::writeResistance(unsigned int wiperIndex, float resistance)
{
  writePosition( wiperIndex, resistanceToPosition(resistance) );
}

void McpDigitalPot::setPosition(unsigned int wiperIndex, unsigned int position)
{
  if (wiperIndex == 1) {
    this->internalSetWiperPosition(kADR_WIPER1, position, false);
  } else {
    this->internalSetWiperPosition(kADR_WIPER0, position, false);
  }
}

void McpDigitalPot::writePosition(unsigned int wiperIndex, unsigned int position)
{
  if (wiperIndex == 1) {
    this->internalSetWiperPosition(kADR_WIPER1, position, true);
  } else {
    this->internalSetWiperPosition(kADR_WIPER0, position, true);
  }
}



// // Not implemented
// bool McpDigitalPot::pot0_connected(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// bool McpDigitalPot::pot1_connected(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// void McpDigitalPot::pot0_connect(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// void McpDigitalPot::pot1_connect(bool terminal_a, bool wiper, bool terminal_b)
// {
//   
// }
// 
// bool McpDigitalPot::pot0_shutdown()
// {
//   
// }
// 
// bool McpDigitalPot::pot1_shutdown()
// {
//   
// }
// 
// void McpDigitalPot::pot0_shutdown(bool shutdown)
// {
//   
// }
// 
// void McpDigitalPot::pot1_shutdown(bool shutdown)
// {
//   
// }
// 
// bool McpDigitalPot::hw_shutdown()
// {
//   
// }