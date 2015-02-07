/*
 * Arudino RGB Mood Lamp
 * =====================
 *
 * IRReceiver: 
 *
 * Decodes input from IR receiver on REC_PIN and sends
 * commands to LED controller.
 *
 * This code requires Ken Shirriff's IR Remote Library used under the LGPL License:
 * https://github.com/shirriff/Arduino-IRremote
 * http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 * Version 0.01 February, 2014
 * Copyright 2014 Alan Pullen
 * http://www.alanpullen.uk/projects/mood-lamp/
 */



#include <Wire.h>
#include <IRremote.h>

// Debug
const int debug	= 1;

// I2C Addresses
const int myAddress     = 0x50;  // Address of this device
const int destAddress   = 0x5F;  // Address of destination device

// Pin Assignments
const int REC_PIN       = 11;  // IR Data In Pin
const int OFF_PIN       = 8;  // Shutdown Pin for Adafruit Power Switch

// IR Remote Codes - NEC Protocol LED618 RGB LED Remote Control
const int IR_INC_BRITE	= 0xFFA05F;
const int IR_DEC_BRITE	= 0xFF20DF;
const int IR_OFF	= 0xFF609F;
const int IR_ON		= 0xFFE01F;
const int IR_FLASH	= 0xFFF00F;
const int IR_STROBE	= 0xFFE817;
const int IR_FADE	= 0xFFD827;
const int IR_SMOOTH	= 0xFFC837;
const int IR_RED	= 0xFF906F;
const int IR_GREEN	= 0xFF10EF;
const int IR_BLUE	= 0xFF50AF;
const int IR_WHITE	= 0xFFD02F;
const int IR_ORANGE	= 0xFFB04F;
const int IR_ORANGE_1	= 0xFFA857;
const int IR_ORANGE_2	= 0xFF9867;
const int IR_YELLOW	= 0xFF8877;
const int IR_GREEN_1	= 0xFF30CF;
const int IR_BLUE_1	= 0xFF28D7;
const int IR_BLUE_2	= 0xFF18E7;
const int IR_BLUE_3	= 0xFF08F7;
const int IR_BLUE_4	= 0xFF708F;
const int IR_PURPLE_1	= 0xFF6897;
const int IR_PURPLE_2	= 0xFF58A7;
const int IR_PURPLE_3	= 0xFF48B7;

// Commands
const int CMD_COLOUR      = 0xFA;
const int CMD_FLASH       = 0xB1;
const int CMD_STROBE      = 0xB4;
const int CMD_FADE        = 0xB6;
const int CMD_SMOOTH      = 0xBF;
const int CMD_ON          = 0xD0;
const int CMD_INC_BRITE   = 0xD6;
const int CMD_DEC_BRITE   = 0xDC;

IRrecv irrecv(REC_PIN);  // Configures IR Receiver
decode_results results;  // Stores IR Input
int keypress;            // Stores Decoded Keypress           

void sendCommand(int command, int r = 0, int g = 0, int b = 0)
{
  if(debug)
  {
    Serial.print("Command: ");
    Serial.print(command);
    Serial.print(" ");
    Serial.print(r);
    Serial.print(":");
    Serial.print(g);
    Serial.print(":");
    Serial.println(b);
  }

  Wire.beginTransmission(destAddress);
  Wire.write(0xF6);
  Wire.write(command);
  Wire.write(r);
  Wire.write(g);
  Wire.write(b);
  Wire.write(0x6F);
  Wire.endTransmission();
}

void setup()
{
  if(debug)
  {
    Serial.begin(9600);
    Serial.println("Ready to decode IR!");
  }

  digitalWrite(OFF_PIN,0);  // Keeps shutdown pin low
  irrecv.enableIRIn();      // Start the receiver
  Wire.begin(myAddress);    // Join I2C bus with myAddress
}

void loop() {
  if (irrecv.decode(&results))
  {
    if(debug)
    {
      Serial.print("IR Command: ");
      Serial.println(results.value, HEX);
    }
    keypress = results.value;

    switch(keypress)
    {
    case IR_FADE:
      sendCommand(CMD_FADE);
      break;
    case IR_INC_BRITE:
      sendCommand(CMD_INC_BRITE);   
      break;
    case IR_DEC_BRITE:
      sendCommand(CMD_DEC_BRITE);    
      break;
    case IR_OFF:
      if(debug)
      {
        Serial.println("Power Off!");
      }
      delay(100);
      digitalWrite(OFF_PIN,1);  // Turns off power switch.    
      break;
    case IR_ON:
      sendCommand(CMD_ON); 
      break;
    case IR_FLASH:
      sendCommand(CMD_FLASH);
      break;
    case IR_STROBE:
      sendCommand(CMD_STROBE);
      break;
    case IR_SMOOTH:
      sendCommand(CMD_SMOOTH);
      break;
    case IR_RED:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);
      break;
    case IR_GREEN:
      sendCommand(CMD_COLOUR,0x00,0xFF,0x00);
      break;
    case IR_BLUE:
      sendCommand(CMD_COLOUR,0x00,0x00,0xFF); 
      break;
    case IR_WHITE:
      sendCommand(CMD_COLOUR,0xFF,0xFF,0xFF);
      break;
    case IR_ORANGE:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00); 
      break;
    case IR_ORANGE_1:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);
      break;
    case IR_ORANGE_2:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00); 
      break;
    case IR_YELLOW:
      sendCommand(CMD_COLOUR,0xFF,0xFF,0x00);
      break;
    case IR_GREEN_1:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);
      break;
    case IR_BLUE_1:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);
      break;
    case IR_BLUE_2:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);  
      break;
    case IR_BLUE_3:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);
      break;
    case IR_BLUE_4:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);
      break;
    case IR_PURPLE_1:
      sendCommand(CMD_COLOUR,0xFF,0x00,0xFF);
      break;
    case IR_PURPLE_2:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);  
      break;
    case IR_PURPLE_3:
      sendCommand(CMD_COLOUR,0xFF,0x00,0x00);
      break;
    }

    irrecv.resume(); // Receive the next value
  }

  delay(100);

}

