#include <TFT_eSPI.h>

#include <PNGdec.h>

/***************************************************************************/
// ESP 32 Röhrenverstärker Steuerung
// IR Receiver
// Displaysteuerung
// 
/***************************************************************************/
#include "display.h"
// FERNBEDIENUNG:
//#define DECODE_DENON        // Includes Sharp
//#define DECODE_JVC
//#define DECODE_KASEIKYO
//#define DECODE_PANASONIC    // alias for DECODE_KASEIKYO
//#define DECODE_LG
///////#define DECODE_NEC          // Includes Apple and Onkyo
//#define DECODE_SAMSUNG
//#define DECODE_SONY
#define DECODE_RC5
//#define DECODE_RC6

//#define DECODE_BOSEWAVE
//#define DECODE_LEGO_PF
//#define DECODE_MAGIQUEST
//#define DECODE_WHYNTER
//#define DECODE_FAST

//#define DECODE_DISTANCE_WIDTH // Universal decoder for pulse distance width protocols
//#define DECODE_HASH         // special decoder for all protocols

//#define DECODE_BEO          // This protocol must always be enabled manually, i.e. it is NOT enabled if no protocol is defined. It prevents decoding of SONY!

//#define DEBUG               // Activate this for lots of lovely debug output from the decoders.

//#define RAW_BUFFER_LENGTH  180  // Default is 112 if DECODE_MAGIQUEST is enabled, otherwise 100.

#include <Arduino.h>

//#define IR_RECEIVE_PIN 12

#define POWER_RELAY_PIN 12

//#define HIGH 0
//#define LOW 1

#define IR_PIN 13

#define IR_DELAY  300
#define POWER_DELAY 3000

#include "PinDefinitionsAndMore.h" // Define macros for input and output pin etc.
#include <IRremote.hpp>
#include <ArduinoJson.h>
//#include <SoftwareSerial.h> // include the SoftwareSerial library

// DISPLAYS:
#include <SPI.h>

#define firstScreenCS 15 //Volumen
#define secondScreenCS 21 //Input
#define thirdScreenCS 22

#define ICON_VOL  1
#define ICON_MUTE 2
#define ICON_BASS 3
#define ICON_TREBLE 4
#define ICON_INPUT 5


/////////////

boolean power = false;
boolean mute = false;

String volume;
int iVol = 0;
String input;

String bass;
int iBass =0;
String treble;
int iTreble = 0;
int iInp = 0;
int iLastVol = 0;
int iLastInp = 0;

const byte rxPin = 5;
const byte txPin = 6;

void sendRemoteCmd(String cmd);


// Set up a new SoftwareSerial object

//SoftwareSerial Serial1 (rxPin, txPin);

void setup() {
    Serial.begin(115200);
    while (!Serial) continue;
    Serial2.begin(9600);
    while (!Serial2) continue;

    // Initialize the "link" serial port
    // Use a low data rate to reduce the error ratio
    //Serial1.begin(9600);
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_PIN)));

    pinMode(4, OUTPUT);

    pinMode(POWER_RELAY_PIN, OUTPUT);
    // schalte ein
    power = true;
    digitalWrite(POWER_RELAY_PIN, power);

    initDisplays();
    setDisplays();
    // TSTupdateVolumeDisplay(0);
    //delay(300);
    sendRemoteCmd("POWER_ON");
    // TSTshowVolumeDisplay(0);
    // TST showInputDisplay(0);
    Serial.print("TFT_CS:");
    Serial.println(TFT_CS);

  
}

void loop() {
    
    if (IrReceiver.decode()) {
        IrReceiver.printIRResultShort(&Serial);
        /*
         * Finally, check the received data and perform actions according to the received command
         */
        switch(IrReceiver.decodedIRData.command) {
            case 0xC:
              Serial.println("received OK");
              sendRemoteCmd("OK");
              delay(IR_DELAY);
              break;
            case 0x1:
              Serial.println("received: POWER");
              if(power == false){
                Serial.println("...ON");
                sendRemoteCmd("POWER_ON");
                power = true;
                setPower(power);
              }else{
                Serial.println("...OFF");
                sendRemoteCmd("POWER_OFF");
                power = false;
                setPower(power);
              }
              delay(POWER_DELAY);
              break;
            case 0x18:
              Serial.println("received: MUTE");
              if(mute == true){
                Serial.println("...OFF");
                sendRemoteCmd("MUTE_OFF");
                mute = false;
              }else{
                Serial.println("...ON");
                sendRemoteCmd("MUTE_ON");
                mute = true;
              }
              delay(IR_DELAY);
              break;
            case 0xE:
              Serial.println("received: VOL-");
              if(mute){
                sendRemoteCmd("MUTE_OFF");
                mute = false;
              }
              sendRemoteCmd("VOLUME_DOWN");
              break;
            case 0x10:
              Serial.println("received: VOL+");
              if(mute){
                sendRemoteCmd("MUTE_OFF");
                mute = false;
              }
              sendRemoteCmd("VOLUME_UP");
              break;
            case 0xD:
              Serial.println("received: UP");
              sendRemoteCmd("UP");
              break;
            case 0x11:
              Serial.println("received: DOWN");
              sendRemoteCmd("DOWN");
              break;
            case 0x51:
              Serial.println("received: LEFT(Vol-)");
              if(mute){
                sendRemoteCmd("MUTE_OFF");
                mute = false;
              }
              sendRemoteCmd("VOLUME_DOWN");
              break;
            case 0x50:
              Serial.println("received: RIGHT(VOL+)");
              if(mute){
                sendRemoteCmd("MUTE_OFF");
                mute = false;
              }
              sendRemoteCmd("VOLUME_UP");
              break;
            case 0x4:
              Serial.println("received: MENU");
              sendRemoteCmd("MENU");
              delay(IR_DELAY);
              break;
            case 0x3:
              Serial.println("received: HOME");
              sendRemoteCmd("HOME");
              delay(IR_DELAY);
              break;
            case 0x5:
              Serial.println("received: BACK");
              sendRemoteCmd("INPUT_1");
              delay(IR_DELAY);
              break;
            case 0x6:
              Serial.println("received: F1");
              sendRemoteCmd("INPUT_2");
              break;
            case 0x7:
              Serial.println("received: F2");
              sendRemoteCmd("INPUT_3");
              break;
            case 0x8:
              Serial.println("received: F3");
              sendRemoteCmd("INPUT_4");
              break;
            case 0xFFFFFFFFFFFFFFFF:
              Serial.println("received: repeat");
              sendRemoteCmd("REPEAT");
              break;
            yield();
        }
        IrReceiver.resume(); // Enable receiving of the next value
        /*
power: 0xA
up  0x0
down  0x1
left  0x3
right 0x2
ok  0x1F
f1: 0x11

f2  0x12

f3  0x0x13


*/

    }
    iLastVol = iVol;
    iLastInp = iInp;

    checkSerialReception();
    if(iLastVol != iVol){
      // update vol display
      updateVolumeDisplay(iVol);
      iLastVol = iVol;
      if(mute){
        mute= false;
        setIcon(ICON_VOL, firstScreenCS);
      }
    }
    if(iLastInp != iInp){
      // update vol display
      updateInputDisplay(iInp);
      iLastInp = iInp;
    }

}

void sendRemoteCmd(String cmd){
  // Create the JSON document
  StaticJsonDocument<200> doc;
  doc["REMOTE_CMD"] = cmd;

  // Send the JSON document over the "link" serial port
  serializeJson(doc, Serial2);

}


void checkSerialReception(){
  // Check if the other Arduino is transmitting
  if (Serial2.available()) 
  {
    //Serial.println("Serial available");
    // Allocate the JSON document
    // This one must be bigger than the sender's because it must store the strings
    StaticJsonDocument<300> doc;

    // Read the JSON document from the "link" serial port
    DeserializationError err = deserializeJson(doc, Serial2);

    if (err == DeserializationError::Ok) 
    {
      // Print the values
      // (we must use as<T>() to resolve the ambiguity)
      //String tmp = JSON.stringify(doc);

      volume = doc["VOLUME"].as<String>();
      if(volume.indexOf("null") < 0){
        Serial.print("VOLUME::: ");
        Serial.println(volume.length());
        iVol = volume.toInt();
      }

      input = doc["INPUT"].as<String>();
      if(input.indexOf("null") < 0){
        Serial.print("INPUT::: ");
        Serial.println(input);
        iInp = input.toInt();
      }

      bass = doc["BASS"].as<String>();
      if(bass.indexOf("null") < 0){
        Serial.print("BASS::: ");
        Serial.println(bass.length());
        iBass = bass.toInt();
      }

      treble = doc["TREBLE"].as<String>();
      if(treble.indexOf("null") < 0){
        Serial.print("TREBLE::: ");
        Serial.println(treble.length());
        iTreble = treble.toInt();
      }
      

      String sMute = doc["MUTE"].as<String>();
      if(sMute.indexOf("null") < 0){
        Serial.print("MUTE::: ");
        Serial.println(sMute);
        if(sMute == "1"){
          setIcon(ICON_MUTE, firstScreenCS);
          mute= true;
        } else if(sMute == "0"){
          setIcon(ICON_VOL, firstScreenCS);
          mute= false;
        }
      }
    
    } 
    else 
    {
      // Print error to the "debug" serial port
      Serial.print("deserializeJson() returned ");
      Serial.println(err.c_str());
  
      // Flush all bytes in the "link" serial port buffer
      while (Serial2.available() > 0)
        Serial2.read();
    }
  }
}

  void setPower(boolean power){
    digitalWrite(POWER_RELAY_PIN, power);
    if(power == true){
      Serial.println("setPower: true");
      digitalWrite(secondScreenCS, LOW);
      setIcon(ICON_INPUT, secondScreenCS);
      updateInputDisplay(iInp);
      digitalWrite(secondScreenCS, HIGH);
      digitalWrite(firstScreenCS, LOW);
      updateVolumeDisplay(iVol);
      iLastVol = iVol;
      if(mute == true){
        setIcon(ICON_MUTE, firstScreenCS);
      } else {
        setIcon(ICON_VOL, firstScreenCS);
      }
      digitalWrite(firstScreenCS, HIGH);
    } else {
      Serial.println("setPower: false");
      disableDisplays();
    }
  }

  

