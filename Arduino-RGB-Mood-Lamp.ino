/*********************************************************************************/
/* Moodlamp - Arduino Powered Moodlamp
/* by Alan Pullen
/* Copyright ©2015. Alan Pullen.
/* See LICENSE.MD for details
/*
/* Version 0.1
/*
/* Based on examples by Nico Hood / FastLED
/*
/*********************************************************************************/

#include <IRLremote.h>   // Lightweight IR Remote Library
#include <FastLED.h>     // FastLED Library: http://fastled.io
#include <MSGEQ7.h>      // NicoHood MSGEQ7 Library: http://nicohood.wordpress.com/

//================================================================================
// IR Remote Control Config
//================================================================================
const int interruptIR = digitalPinToInterrupt(2);    // Interrupt Pin
uint8_t IRProtocol = 0;     // Stores IR Protocol ID Number
uint16_t IRAddress = 0;     // Stores IR Command Address
uint32_t IRCommand = 0;     // Stores IR Command Code

//================================================================================
// MSGEQ7 Config
//================================================================================
const int pinAnalogIn = A0;  // DC Multiplexed Input for MSGEQ7
const int pinReset = 5;      // Reset Pin for MSGEQ7
const int pinStrobe = 4;     // Strobe Pin for MSGEQ7

//================================================================================
// IR Remote Control Codes (LED618 Remote Control, NEC Protocol)
//================================================================================
const uint16_t irAddr           = 0xFF00;    // Prevents over NEC remotes interfering
const uint32_t irCmdInc	        = 0xFA05;
const uint32_t irCmdDec	        = 0xFB04;
const uint32_t irCmdOff	        = 0xF906;
const uint32_t irCmdOn	        = 0xF807;
const uint32_t irCmdFlash	= 0xF00F;
const uint32_t irCmdStrobe	= 0xE817;
const uint32_t irCmdFade	= 0xE41B;
const uint32_t irCmdSmooth	= 0xEC13;
const uint32_t irCmdRed	        = 0xF609;
const uint32_t irCmdGreen	= 0xF708;
const uint32_t irCmdBlue	= 0xF50A;
const uint32_t irCmdWhite	= 0xF40B;
const uint32_t irCmdOrangeA	= 0xF20D;
const uint32_t irCmdOrangeB	= 0xEA15;
const uint32_t irCmdOrangeC	= 0xE619;
const uint32_t irCmdYellow	= 0xEE11;
const uint32_t irCmdGreenA	= 0xF30C;
const uint32_t irCmdBlueA	= 0xEB14;
const uint32_t irCmdBlueB	= 0xE718;
const uint32_t irCmdBlueC	= 0xEF10;
const uint32_t irCmdBlueD	= 0xF10E;
const uint32_t irCmdPurpleA	= 0xE916;
const uint32_t irCmdPurpleB	= 0xE51A;
const uint32_t irCmdPurpleC	= 0xED12;

//================================================================================
// Modes - Not really needed - makes code easier to read
//================================================================================
const uint8_t modeStatic   = 1;
const uint8_t modeFade     = 2;
const uint8_t modeMusic    = 3;
const uint8_t modeFire     = 4;
const uint8_t modeSmooth   = 5;
const uint8_t modeSunSet   = 6;
const uint8_t modeSunRise  = 7;
const uint8_t modeOn       = 8;
const uint8_t modeOff      = 9;
uint8_t currMode = 2;            // Currently selected mode

//================================================================================
// FastLED Config
//================================================================================
const int numLeds = 24;
const int ledPin = 6;
const int brightness = 255;
CRGB leds[numLeds];
uint32_t myColour = CRGB::Black;
TBlendType    currentBlending;
CRGBPalette16 currentPalette;

//================================================================================
// Other Vars
//================================================================================
const int pwrPin = 7;
const int pwrLed = 8;
const int sunriseTime = 3500;     // Delay in ms between steps, 256 steps total.
const int sunsetTime = 2000;      // Delay in ms between steps, 256 steps total.
const int fireCooling = 100;
const int fireSparking = 200;

int resetMode = 0;                // Used to reset a mode or increment colour cycler

//================================================================================
// First Run / System Setup and Initialisation
//================================================================================
void setup()
{
  //Serial.begin(9600);
  //Serial.println("Starting up ...");
 
  
  pinMode(pwrLed, OUTPUT);
  digitalWrite(pwrLed, 1);
  
  pinMode(pwrPin, OUTPUT);
  digitalWrite(pwrPin, 0); // Keeps PWR pin low
  
  // Start lightweight IR receiver code.
  IRLbegin<IR_ALL>(interruptIR);
  
  // Start MSGEQ7
  MSGEQ7.begin(pinReset, pinStrobe, pinAnalogIn);
  
  // Start FastLED Library and configure to use Adafruit Neopixels.
  FastLED.addLeds<NEOPIXEL, ledPin>(leds, numLeds).setCorrection(TypicalSMD5050);
  FastLED.setDither( 0 );
  currentPalette = ForestColors_p;
  currentBlending = BLEND;
  
}

// Cycles through hues on button press.
void ColourCycler(int dir = 1)
{
      static uint8_t hue = 0;
      if(dir > 0) {hue = hue + resetMode;}
      else {hue = hue - resetMode;}
      FastLED.showColor(CHSV(hue, 255, 255));
      delay(25);
}

//================================================================================
// Main Program Loop
//================================================================================
void loop()
{
  // Process IR Commands
  resetMode = 0;
  
  uint8_t oldSREG = SREG;
  cli();
  if (IRProtocol && (IRAddress == irAddr))
  {
    
    //Serial.println(IRProtocol, HEX);
    //Serial.println(IRAddress, HEX);
    //Serial.println(IRCommand, HEX);
    
    switch(IRCommand)
    {
      case irCmdInc:
        currMode = modeSunRise;
        resetMode = 1;
        break;
      case irCmdDec:
        currMode = modeSunSet;
        resetMode = 1;
        break;
      case irCmdOff:
        currMode = modeOff;
        resetMode = 1;
        break;
      case irCmdOn:
        currMode = modeOn;
        resetMode = 1;
        break;
      case irCmdFlash:
        currMode = modeFire;
        break;
      case irCmdStrobe:
        currMode = modeMusic;
        break;
      case irCmdFade:
        currMode = modeFade;
        resetMode = 1;
        break;
      case irCmdSmooth:
        currMode = modeSmooth;
        resetMode = 1;
        break;
      case irCmdRed:
        currMode = modeStatic;
        myColour = CRGB::Red;
        break;
      case irCmdGreen:
        currMode = modeStatic;
        myColour = CRGB::Green;
        break;
      case irCmdBlue:
        currMode = modeStatic;
        myColour = CRGB::Blue;
        break;
      case irCmdWhite:
        currMode = modeStatic;
        myColour = CRGB::White;
        break;
      case irCmdOrangeA:
        currMode = modeStatic;
        myColour = CRGB::DarkOrange;
        break;
      case irCmdOrangeB:
        currMode = modeStatic;
        myColour = CRGB::OrangeRed;
        break;
      case irCmdOrangeC:
        currMode = modeStatic;
        myColour = CRGB::Orange;
        break;
      case irCmdYellow:
        currMode = modeStatic;
        myColour = CRGB::Yellow;
        break;
      case irCmdGreenA:
        currMode = modeStatic;
        myColour = CRGB::LightGreen;
        break;
      case irCmdBlueA:
        currMode = modeStatic;
        myColour = CRGB::LightBlue;
        break;
      case irCmdBlueB:
        currMode = modeStatic;
        myColour = CRGB::Cyan;
        break;
      case irCmdBlueC:
        currMode = modeStatic;
        myColour = CRGB::SkyBlue;
        break;
      case irCmdBlueD:
        currMode = modeStatic;
        myColour = CRGB::MidnightBlue;
        break;
      case irCmdPurpleA:
        currMode = modeStatic;
        myColour = CRGB::DarkViolet;
        break;
      case irCmdPurpleB:
        currMode = modeStatic;
        myColour = CRGB::MediumPurple;
        break;
      case irCmdPurpleC:
        currMode = modeStatic;
        myColour = CRGB::Purple;
        break;
      default:
        ///Serial.println("Command Not Recognised!!!");
      break;
    }
    IRProtocol = 0;
  }
  
  SREG = oldSREG;
  

  //Serial.println(currMode);
  
  // Process Modes
  switch(currMode)
  {
    // Static Colour Display
    case modeStatic:
      FastLED.showColor(myColour);
      delay(100);
      break;
    // Rainbow Fader
    case modeFade:
      RainbowFader();
      break;
    case modeMusic:
      Music();
      break;
    case modeFire:
      Fire();
      break;
    case modeSmooth:
      Smooth();
      break;
    case modeSunRise:
      SunRise();
      break;
    case modeSunSet:
      SunSet();
      break;
    case modeOn:
      ColourCycler(1);
      break;
    case modeOff:
      ColourCycler(0);
      break;

  }
  
  //delay(1000);
}


void Fire()
{
  currentPalette = CRGBPalette16( CRGB::Maroon,  CRGB::Red, CRGB::OrangeRed, CRGB::DarkOrange);
  
  for(uint8_t x = 0; x <numLeds; x++)
  {
    leds[x] = ColorFromPalette( currentPalette, random8(), brightness, currentBlending);
  }

  // Add some flickering
  if( random8() < 64) {
    leds[ random16(numLeds)] += CRGB::Yellow;
  }

FastLED.show(); // display this frame
delay(75);

}




 

void Music()
{
  // read without delay
  static long prevMillis = 0;
  unsigned long currentMillis = millis();
  uint8_t bandData[6];
  
  if (currentMillis - prevMillis > 20)
  {
    prevMillis = currentMillis;
    
    MSGEQ7.read();
    
    for(int i = 0; i < 6; i++)
    {
      bandData[i] = MSGEQ7.get(i,MSGEQ7_LEFT);
    }

    
    for (int i = 0; i < 8; i++)
    {
      leds[i].setHSV(HUE_RED, 255, max(bandData[0],bandData[1]));
      leds[i + 8].setHSV(HUE_GREEN, 255, max(bandData[2],bandData[3]));
      leds[i + 16].setHSV(HUE_BLUE, 255, max(bandData[4],bandData[5]));

    }
    
    FastLED.show();

  }
}

void SunRise()
{

  static uint8_t colourIndex = 0;
  
   // If button pressed reset to start
  if(resetMode){colourIndex = 0;}
 
  
//currentPalette = mySunrise_p;
  currentPalette = CRGBPalette16( CRGB::Black, CRGB::OrangeRed, CRGB::DarkOrange, CRGB::LemonChiffon);
  
  for(uint8_t i = 0; i < numLeds; i++)
  {
    leds[i] = ColorFromPalette( currentPalette, colourIndex, brightness, currentBlending);
  }
  
  // Holds on last colour at end of sunrise.
  if (colourIndex != 240) {colourIndex++;}
  
 
  
  FastLED.show();
  delay(sunriseTime);
  
}

void SunSet()
{
  static uint8_t colourIndex = 0;
  
  // If button pressed reset to start
  if(resetMode){colourIndex = 0;}
//currentPalette = mySunset_p;
  currentPalette = CRGBPalette16( CRGB::LemonChiffon, CRGB::DarkOrange, CRGB::OrangeRed, CRGB::Black);
  for(uint8_t i = 0; i < numLeds; i++)
  {
    leds[i] = ColorFromPalette( currentPalette, colourIndex, brightness, currentBlending);
  }
  
  // Holds on last colour at end of sunset.
  if (colourIndex != 240) {colourIndex++;}
  
  FastLED.show();
  delay(sunsetTime);
}

void RainbowFader()
{
      static uint8_t hue = 0;
      if(resetMode){hue = 0;}
      FastLED.showColor(CHSV(hue++, 255, 255)); 
      delay(100);
}






void Smooth()
{
  static uint8_t colourIndex = 0;
  static uint8_t paletteIndex = 0;
  
  if(resetMode){colourIndex = 0; paletteIndex = 0;}
  
  if      (paletteIndex == 0) {currentPalette = CloudColors_p;}
  else if (paletteIndex == 1) {currentPalette = PartyColors_p;}
  else if (paletteIndex == 2) {currentPalette = OceanColors_p;}
  else if (paletteIndex == 3) {currentPalette = ForestColors_p;}
  else if (paletteIndex == 4) {currentPalette = RainbowColors_p;}
  else {paletteIndex = 0; currentPalette = CloudColors_p;}
  
  for(uint8_t i = 0; i < numLeds; i++)
  {
    leds[i] = ColorFromPalette( currentPalette, colourIndex, brightness, currentBlending);
  }
  
  if(colourIndex == 255) {paletteIndex++;}
  colourIndex++;
  
  FastLED.show();
  delay(100);
}

//================================================================================
// IR Interrup Event - Called on change.  Do not use Serial inside this routine!!!
//================================================================================
void IREvent(uint8_t protocol, uint16_t address, uint32_t command)
{
  // Updates global values to the newest valid input
  IRProtocol = protocol;
  IRAddress = address;
  IRCommand = command;
}
