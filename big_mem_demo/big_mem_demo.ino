#define FILESYSTEM SD
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "AnimatedGIF.h"
//#include "../test_images/pattern.h"

#define MAX_HEIGHT 150
#define MAX_WIDTH 150
#define COLOR 3


#define DISPLAY_WIDTH 320
AnimatedGIF gif;
File f;

char *ppic;
#include "gifDraw.h"
#include "fileIO.h"




void setup() {
  Serial.begin(115200);

  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    ppic=(char *)malloc(MAX_HEIGHT*MAX_WIDTH*COLOR);
    //listDir(SD, "/", 0);
    //createDir(SD, "/mydir");
    //listDir(SD, "/", 0);
    //removeDir(SD, "/mydir");
    
  gif.begin(BIG_ENDIAN_PIXELS,GIF_PALETTE_RGB888);//gif.begin(LITTLE_ENDIAN_PIXELS);
} 

void loop() {
  long lTime;
  int iFrames;
  char *szDir = "/tet"; // play all GIFs in this directory on the SD card
  char fname[256];
  File root, temp;
  
  //listDir(SD, "/", 0);
   while (1) // run forever
   {
      root = SD.open(szDir);
      if (root)
      {
         temp = root.openNextFile();
            while (temp)
            {
              if (!temp.isDirectory()) // play it
              {
                strcpy(fname, temp.name());

                Serial.printf("Playing %s\n", temp.name());
                Serial.flush();
                ShowGIF((char *)temp.name());
             
              }
              temp.close();
              temp = root.openNextFile();
            }
         root.close();
      } // root
      delay(4000); // pause before restarting
   }
} 



/*
  if (gif.open((uint8_t *)ucPattern, sizeof(ucPattern), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    // Allocate an internal buffer to hold the full canvas size
    // In this case, 128x128 = 16k of RAM
    if (gif.allocFrameBuf(GIFAlloc) == GIF_SUCCESS)
    {
      gif.setDrawType(GIF_DRAW_COOKED); // we want the library to generate ready-made pixels
      lTime = micros();
      iFrames = 0;
      while (gif.playFrame(true, NULL))
      {
        iFrames++;
      }
      lTime = micros() - lTime;
      Serial.printf("total decode time for %d frames = %d us\n", iFrames, (int)lTime);
      gif.freeFrameBuf(GIFFree);
    }
    else
    {
      Serial.println("Insufficient memory!");
    }
    gif.close();
  }*/
