/* Modified mouse initialization to accomondate for
 * Intellimouse vertical wheel (scroll) 
 * following the work by DGar 02/05/2014
 * Author: Vinh Nguyen
 * Nano board
 */

/*
 * an arduino sketch to interface with a ps/2 mouse.
 * Also uses serial protocol to talk back to the host
 * and report what it finds.
 * This support 2 mice with 2 channels
 */

/*
 * Pin 5 is the mouse data pin, pin 6 is the clock pin
 * Feel free to use whatever pins are convenient.
 */
#define MDATA_L 5
#define MCLK_L 6
#define MDATA_R 9
#define MCLK_R 10
#include "PS2Mouse.h"

// left & right: when a person sitting on the wheelchair, his left hand is corresponding to the left mouse. Similarly to the right.

PS2Mouse mouse_left(MCLK_L,MDATA_L);
PS2Mouse mouse_right(MCLK_R,MDATA_R);

int minx=1000, maxx= -1000, miny= 1000, maxy= -1000;
unsigned long cycle= 0;
unsigned long startcycle = 0;
unsigned long endcycle = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup...");
  mouse_left.mouse_init();
  Serial.println("1 done!");
  mouse_right.mouse_init();
  Serial.println("2 done!");
  Serial.println("complete!");
}

/*
 * get a reading from the mouse and report it back to the
 * host via the serial line.
 */
void loop()
{
  int o= 100;
  cycle++;

  uint8_t stat_left;
  int x_left,y_left, z_left;
  mouse_left.getPosition(stat_left,x_left,y_left, z_left); 

  /* send the data back up */
  if (cycle % o == 0)
  {
    Serial.print("\tstat=");
    Serial.print(stat_left, BIN);
    Serial.print("\tX_L=");
    Serial.print(x_left, DEC);
    Serial.print("\tY_L=");
    Serial.print(y_left, DEC);
//  Serial.print("\tZ=");
//  Serial.print(z_left, DEC);
  }
  //Serial.println();

  uint8_t stat_right;
  int x_right,y_right, z_right;
  mouse_right.getPosition(stat_right,x_right,y_right, z_right); 

  /* send the data back up */
  if (cycle % o == 0)
  {
    Serial.print("\tstat=");
    Serial.print(stat_right, BIN);
    Serial.print("\tX_R=");
    Serial.print(x_right, DEC);
    Serial.print("\tY_R=");
    Serial.print(y_right, DEC);
  }  

//  int x= (int)mx;
//  int y= (int)my;
  if (minx > x_left) {minx=x_left;};
  if (miny > y_left) {miny=y_left;};
  if (maxx < x_left) {maxx=x_left;};
  if (maxy < y_left) {maxy=y_left;};

  if (cycle % o == 0)
  {
  Serial.print("\tminX=");
  Serial.print(minx, DEC);
  Serial.print("\tminY=");
  Serial.print(miny, DEC);
  Serial.print("\tmaxX=");
  Serial.print(maxx, DEC);
  Serial.print("\tmaxY=");
  Serial.print(maxy, DEC);

  endcycle = millis();
  Serial.print("\tCycles: ");
  Serial.print(cycle, DEC);
  //Serial.print("\tTime taken in millisecs: ");
  Serial.print("\tTime taken in secs: ");
  Serial.println((endcycle- startcycle)/1000, DEC);
  
  Serial.println();
  }
  //delay(20);  /* twiddle */
  delayMicroseconds(300);
}
