/*void GIFDraw(GIFDRAW *pDraw)
{
  if (pDraw->y == 0) { // set the memory window when the first line is rendered
     //spilcdSetPosition(&lcd, pDraw->iX, pDraw->iY, pDraw->iWidth, pDraw->iHeight, DRAW_TO_LCD);
  }
  // For all other lines, just push the pixels to the display
  //spilcdWriteDataBlock(&lcd, (uint8_t *)pDraw->pPixels, pDraw->iWidth*2, DRAW_TO_LCD);
} /* GIFDraw() */

//
// The memory management functions are needed to keep operating system
// dependencies out of the core library code
//
// memory allocation callback function
void * GIFAlloc(uint32_t u32Size)
{
  return malloc(u32Size);
} /* GIFAlloc() */
// memory free callback function
void GIFFree(void *p)
{
  free(p);
} /* GIFFree() */






// Draw a line of image directly on the LED Matrix
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s,*usPalette24;
    uint16_t *d, *usPalette;
    int x, y, iWidth;

  iWidth = pDraw->iWidth;
//  if (iWidth > MATRIX_WIDTH)
//      iWidth = MATRIX_WIDTH;

    usPalette24 = pDraw->pPalette24;
    y = pDraw->iY + pDraw->y; // current line
    
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + pDraw->iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while(x < pDraw->iWidth)
      {
        c = ucTransparent-1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent) // done, stop
          {
            s--; // back up to treat it like transparent
          }
          else // opaque
          {
             *d++ = usPalette[c];
             iCount++;
          }
        } // while looking for opaque pixels
        if (iCount) // any opaque pixels?
        {
          for(int xOffset = 0; xOffset < iCount; xOffset++ ){
            //dma_display.drawPixelRGB565(x + xOffset, y, usTemp[xOffset]);
          }
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent)
             iCount++;
          else
             s--; 
        }
        if (iCount)
        {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    }
    else // does not have transparency
    {
      s = pDraw->pPixels;
      //Serial.printf("(pDraw->y):%d, usPalette:%d, usPalette[0]:%d ,usPalette[1]:%d ,usPalette[2]:%d ,150*3  \n",(pDraw->y),usPalette24,usPalette24[0],usPalette24[1],usPalette24[2] );
      if((pDraw->y)>MAX_HEIGHT){
        memcpy(ppic+(pDraw->y*MAX_WIDTH*COLOR),usPalette24,MAX_WIDTH*COLOR);
      }
    }
} /* GIFDraw() */


void * GIFOpenFile(const char *fname, int32_t *pSize)
{
  f = FILESYSTEM.open(fname);
  if (f)
  {
    *pSize = f.size();
    return (void *)&f;
  }
  return NULL;
} /* GIFOpenFile() */

void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
       return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
} /* GIFReadFile() */

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{ 
  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  i = micros() - i;
//  Serial.printf("Seek time = %d us\n", i);
  return pFile->iPos;
} /* GIFSeekFile() */

unsigned long start_tick = 0;

void ShowGIF(char *name)
{
  start_tick = millis();
   
  if (gif.open(name, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw))
  {
    //x_offset = (MATRIX_WIDTH - gif.getCanvasWidth())/2;
    //if (x_offset < 0) x_offset = 0;
    //y_offset = (MATRIX_HEIGHT - gif.getCanvasHeight())/2;
    //if (y_offset < 0) y_offset = 0;
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    Serial.flush();
    while (gif.playFrame(true, NULL))
    {      
      if ( (millis() - start_tick) > 8000) { // we'll get bored after about 8 seconds of the same looping gif
        break;
      }
    }
    gif.close();
  }

} /* ShowGIF() */
