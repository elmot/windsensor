#ifndef __WINSTAR__H
#define __WINSTAR__H

#include "stm32f4xx.h"

class BWPict {
	public:
	  BWPict(int aG_WIDTH_BYTES, int aG_HEIGHT, char *aBYTES )
	  {
	   		G_WIDTH_BYTES = aG_WIDTH_BYTES; 
	  		G_HEIGHT = aG_HEIGHT;
	  		BYTES = (unsigned char *)aBYTES;
	  }
	  int G_WIDTH_BYTES;
	  int G_HEIGHT;
	  unsigned char * BYTES;
};

class GraphSurface {
	public:
	  static const int G_WIDTH = 160;
	  static const int G_HEIGHT = 160;
		virtual void pixel(int x,int y, int color = 1) = 0;
		virtual void box(int x1, int y1, int x2,int y2,int width = 1,  char * pattern = "1");
		virtual void line(int x1, int y1, int x2,int y2,int width = 1, char * pattern = "1") = 0;
};

class WinstarLCD : public GraphSurface 
{
    private:
	  static const int G_WIDTH_BYTES = G_WIDTH / 8;
      static const int G_BUFF_SIZE =   G_WIDTH * G_HEIGHT / 8;
	  void setCursorPos(int cursorPos);
	public:
	  static const int T_WIDTH = 32;
	  static const int T_HEIGHT = 16;
	 WinstarLCD();
	/* Init and background */
	 void init(void);
	 void bgLight(int percentage);
	/* Graph routines */
	 void updateGraphScreen(void);
	 void clearGraphBuff(void);
	 void writePict(int dispX8, int dispY, int height, BWPict pict,int pictOrY,int pictAndY);
  	 virtual void pixel(int x,int y, int color = 1);
  	 virtual void line(int x1, int y1, int x2,int y2,int width = 1,  char * pattern = "1");
	/* Text routines */
	 void setupText(void);
	 void writeText(char * text,int textX, int textY,int cursorX,int cursorY);
    private:
	 static void delay (void);
	  void wait(void);
	  void writeByte(uint8_t regNum, uint8_t value);
	  unsigned char displayBuffer[G_BUFF_SIZE];
	  bool graphMode;
};

class AffineTransform: public GraphSurface {

	public:
		AffineTransform(GraphSurface * _graphSurface);
   		virtual void pixel(int x,int y, int color = 1);
	    void reset(void);
		void rotate(float theta);
		void translate(float tx, float ty);
    	void rotate(float theta, float anchorx, float anchory);
		virtual void line(int x1, int y1, int x2,int y2,int width = 1,  char * pattern = "1");
	private:
		GraphSurface * graphSurface;
    	float m00;

		    /**
		     * The Y coordinate shearing element of the 3x3
		     * affine transformation matrix.
		     *
		     * @serial
		     */ 
		     float m10;
		
		    /**
		     * The X coordinate shearing element of the 3x3
		     * affine transformation matrix.
		     *
		     * @serial
		     */
		     float m01; 
		
		    /**
		     * The Y coordinate scaling element of the 3x3
		     * affine transformation matrix.
		     * 
		     * @serial
		     */
		     float m11; 
		
		    /**
		     * The X coordinate of the translation element of the
		     * 3x3 affine transformation matrix.
		     * 
		     * @serial
		     */
		     float m02;
		
		    /**
		     * The Y coordinate of the translation element of the
		     * 3x3 affine transformation matrix.
		     *
		     * @serial
		     */
		     float m12;

};

extern WinstarLCD winstarLCD;
#endif
