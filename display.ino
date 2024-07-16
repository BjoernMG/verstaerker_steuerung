#include <PNGdec.h>
#include "icons.h" // Image is stored here in an 8 bit array


//#include "NotoSansBold15.h"
//#include "NotoSansBold36.h"
//#include "Free_Fonts.h" // Include the header file attached to this sketch
//#include "Final_Frontier_28.h"
#include "Finalnew64.h"
#include "Finalnew48.h"
#include "Finalnew36.h"

#include "display.h"


PNG png; // PNG decoder inatance
#define MAX_IMAGE_WDITH 240 // Adjust for your images


TFT_eSPI tft1 = TFT_eSPI();       // Invoke custom library
TFT_eSPI tft2 = TFT_eSPI();  
//TFT_eSPI tft3 = TFT_eSPI();  

TFT_eSPI *pTft = &tft1;



//String inputs[13] = { "TV", "AUX1", "AUX2", "WIFI", "CD", "TV", "AUX1", "AUX2", "WIFI", "CD", "TV", "AUX1", "AUX2"};
String inputs[13] = { "TV", "CD", "WIFI", "AUX2", "AUX1","TV", "CD", "WIFI", "AUX2", "AUX1", "TV", "CD", "WIFI"};
int lastInp = 0;

static uint32_t count = 0;
int oldValue = 0;

int lastVolume=0;

uint16_t fg_color = TFT_WHITE;
uint16_t bg_color = TFT_BLACK;       // This is the background colour used for smoothing (anti-aliasing)

uint16_t x = 100;  // Position of centre of arc
uint16_t y = 100;

uint8_t radius       = 70; // Outer arc radius
uint8_t thickness    = 20;     // Thickness
uint8_t inner_radius = radius - thickness;        // Calculate inner radius (can be 0 for circle segment)

// 0 degrees is at 6 o'clock position
// Arcs are drawn clockwise from start_angle to end_angle
uint16_t start_angle = 30; // Start angle must be in range 0 to 360

void initDisplays(){
  pinMode(firstScreenCS, OUTPUT);
  digitalWrite(firstScreenCS, HIGH);
  
  pinMode(secondScreenCS, OUTPUT);
  digitalWrite(secondScreenCS, HIGH);

  //pinMode(thirdScreenCS, OUTPUT);
  //digitalWrite(thirdScreenCS, HIGH);
  

  // We need to 'init' both displays
  // at the same time. so set both cs pins low
  digitalWrite(firstScreenCS, LOW);
  digitalWrite(secondScreenCS, LOW);
  //digitalWrite(thirdScreenCS, LOW);
  
  
  tft1.init(firstScreenCS);
  tft1.setTextSize(2);
  tft2.init(secondScreenCS);
  tft2.setTextSize(2);
  //tft3.init(thirdScreenCS);
  //tft3.setTextSize(2);
  
  // Set both cs pins HIGH, or 'inactive'
  digitalWrite(firstScreenCS, HIGH);
  digitalWrite(secondScreenCS, HIGH);
  //digitalWrite(thirdScreenCS, HIGH);

}

void updateVolumeDisplay(int volume){
  digitalWrite(firstScreenCS, LOW);


  //uint16_t end_angle   = 32 + (volume * 3); // End angle must be in range 0 to 360
    
    ringMeter(volume, "", 120, 120, 120, 20, TFT_LIGHTGREY, TFT_LIGHTGREY, TFT_LIGHTGREY, TFT_BLACK );

    digitalWrite(firstScreenCS, HIGH);
}

void setDisplays(){
  
  /*
   Update the first screen
   all 'tft1.' call must be done
   after setting the cs pin low
  */
  digitalWrite(firstScreenCS, LOW);
  tft1.setRotation(2);  
  tft1.fillScreen(TFT_BLUE);
  // set the cs pin back to HIGH when finished
  // with this particular display
  digitalWrite(firstScreenCS, HIGH);
  delay(50);
  setIcon(ICON_VOL, firstScreenCS);
  delay(50);
  // Update the second screen
  digitalWrite(secondScreenCS, LOW);
  tft2.setRotation(4);  
  tft2.fillScreen(TFT_RED);
  //tft2.drawFloat(88, 2, 100, 100);

  //lastInp=4;
  //tft2.drawString(inputs[2], 175, 20, 12);
  digitalWrite(secondScreenCS, HIGH);
  delay(50);
  setIcon(ICON_INPUT, secondScreenCS);
}


void ringMeter(int value, String gauge, int xpos, int ypos, int radius, int thickness, int textColor, int meterColor, int ringColor, int backColor ){

  bool arc_end = false;   
  int start_angle = 40;
  int end_angle = start_angle + 2 + (value * 4.35) ;    
  Serial.print("start angle: ");
  Serial.print(start_angle);
  Serial.print("   end angle: ");
  Serial.println(end_angle);


  tft1.drawSmoothArc(xpos, ypos, radius, radius -20, 40 , 320, TFT_DARKGREY, TFT_BLACK, 0);
  tft1.drawSmoothArc(xpos, ypos, radius, radius -20, start_angle , end_angle, meterColor, TFT_BLACK, 0);

  //tft1.drawSmoothArc(xpos, ypos, radius, radius -20, start_angle , end_angle, meterColor, TFT_BLACK, 0);


  tft1.loadFont(Finalnew64);

  uint16_t x = tft1.width()/2;
  uint16_t y = tft1.height()/2;

  tft1.setTextDatum(MC_DATUM);
  tft1.setTextColor(TFT_BLACK, TFT_BLACK);
  tft1.drawString( String(lastVolume), x, y - 0, 12);
  tft1.setTextColor(TFT_WHITE, TFT_BLACK);
  tft1.drawString( String(value), x, y - 0, 12);
  lastVolume = value;
  tft1.setTextPadding(0);

  lastVolume = value;
  
}

void updateInputDisplay(int inp){
  digitalWrite(secondScreenCS, LOW);

  uint16_t x = tft1.width()/2;
  uint16_t y = tft1.height()/2;

  tft2.setTextDatum(MC_DATUM);
  tft2.loadFont(Finalnew48);
  tft2.setTextColor(TFT_BLACK); 
  tft2.drawString( inputs[lastInp +3], x, y-10, 12);
  tft2.setTextColor(TFT_WHITE); 
  tft2.drawString( inputs[inp +3], x, y-10, 12);

  tft2.loadFont(Finalnew36);
  tft2.setTextColor(TFT_BLACK); 
  tft2.drawString( inputs[lastInp +2], x, y-60, 12);
  tft2.setTextColor(TFT_DARKGREY); 
  tft2.drawString( inputs[inp +2], x, y-60, 12);

  tft2.setTextColor(TFT_BLACK); 
  tft2.drawString( inputs[lastInp +4], x, y+40, 12);
  tft2.setTextColor(TFT_DARKGREY); 
  tft2.drawString( inputs[inp +4], x, y+40, 12);
  
/*
  tft2.loadFont(Finalnew36);
  tft2.setTextColor(TFT_BLACK);   
  //tft2.setFreeFont(&FreeMonoBold12pt7b);
  tft2.drawString(inputs[lastInp +1], 175, 20, 12);
  tft2.drawString(inputs[lastInp +2], 135, 70, 12);
  tft2.loadFont(Finalnew48);
  tft2.drawString(inputs[lastInp +3], 95, 125, 12);
  tft2.loadFont(Finalnew36);
  tft2.drawString(inputs[lastInp +4], 135, 200, 12);
  tft2.drawString(inputs[lastInp +5], 175, 250, 12);

  tft2.setTextColor(TFT_DARKGREY);   
  tft2.loadFont(Finalnew36);
  tft2.drawString(inputs[inp +1], 175, 20, 12);
  tft2.drawString(inputs[inp +2], 135, 70, 12);
  tft2.loadFont(Finalnew48);
  tft2.setTextColor(TFT_LIGHTGREY); 
  tft2.drawString(inputs[inp +3], 95, 125, 12);
  tft2.loadFont(Finalnew36);
  tft2.setTextColor(TFT_DARKGREY); 
  tft2.drawString(inputs[inp +4], 135, 200, 12);
  tft2.drawString(inputs[inp +5], 175, 250, 12);
*/
  lastInp = inp;

  digitalWrite(secondScreenCS, HIGH);
}

void setIcon(int icon, uint8_t csPin){
  digitalWrite(csPin, LOW);
  int16_t rc;
  pTft = &tft1;
  switch(icon){
    case ICON_VOL:
      rc = png.openFLASH((uint8_t *)volume_64, sizeof(volume_64), pngDraw);
    break;
    case ICON_MUTE:
      rc = png.openFLASH((uint8_t *)mute_64, sizeof(mute_64), pngDraw);
    break;
    case ICON_BASS:
      rc = png.openFLASH((uint8_t *)bass_64, sizeof(bass_64), pngDraw);
    break;
    case ICON_TREBLE:
      rc = png.openFLASH((uint8_t *)treble_64, sizeof(treble_64), pngDraw);
    break;
    case ICON_INPUT:
      rc = png.openFLASH((uint8_t *)input_64, sizeof(input_64), pngDraw);
    break;
  }

  if (rc == PNG_SUCCESS) {
    Serial.println("Successfully load png file");
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    pTft->startWrite();
    uint32_t dt = millis();
    rc = png.decode(NULL, 0);
    Serial.print(millis() - dt); Serial.println("ms");
    pTft->endWrite();
    digitalWrite(csPin, HIGH);
  }
}

void pngDraw(PNGDRAW *pDraw) {
  uint16_t lineBuffer[MAX_IMAGE_WDITH];
  png.getLineAsRGB565(pDraw, lineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft1.pushImage(88, 170 + pDraw->y, pDraw->iWidth, 1, lineBuffer);
}


void disableDisplays(){
    digitalWrite(firstScreenCS, LOW);
    tft1.fillScreen(TFT_BLACK);
    digitalWrite(firstScreenCS, HIGH);
    digitalWrite(secondScreenCS, LOW);
    tft2.fillScreen(TFT_BLACK);
    digitalWrite(secondScreenCS, HIGH);
}

void showVolumeDisplay(int vol){
  digitalWrite(firstScreenCS, LOW);
  delay(50);
  setIcon(ICON_VOL, firstScreenCS);
  delay(50);
  tft1.fillScreen(TFT_BLACK);
  digitalWrite(firstScreenCS, HIGH);
  updateVolumeDisplay(vol);
}

void showBassDisplay(int bass){
  digitalWrite(firstScreenCS, LOW);
  delay(50);
  setIcon(ICON_BASS, firstScreenCS);
  delay(50);
  tft1.fillScreen(TFT_BLACK);
  digitalWrite(firstScreenCS, HIGH);
  updateBassDisplay(bass);
}

void showInputDisplay(int inp){
  digitalWrite(secondScreenCS, LOW);
  delay(50);
  setIcon(ICON_INPUT, secondScreenCS);
  delay(50);
  tft2.fillScreen(TFT_BLACK);
  digitalWrite(secondScreenCS, HIGH);
  updateInputDisplay(inp);
}

void showTrebleDisplay(int treble){
  digitalWrite(secondScreenCS, LOW);
  delay(50);
  setIcon(ICON_TREBLE, secondScreenCS);
  delay(50);
  tft2.fillScreen(TFT_BLACK);
  digitalWrite(secondScreenCS, HIGH);
  updateTrebleDisplay(treble);
}

void updateBassDisplay(int bass){
  digitalWrite(firstScreenCS, LOW);
    
    ringMeter(bass, "", 120, 120, 120, 20, TFT_LIGHTGREY, TFT_LIGHTGREY, TFT_LIGHTGREY, TFT_BLACK );

    digitalWrite(firstScreenCS, HIGH);
}

void updateTrebleDisplay(int treble){
  digitalWrite(secondScreenCS, LOW);
    
    ringMeter(treble, "", 120, 120, 120, 20, TFT_LIGHTGREY, TFT_LIGHTGREY, TFT_LIGHTGREY, TFT_BLACK );

    digitalWrite(secondScreenCS, HIGH);
}





