#include "PS2Mouse.h"

PS2Mouse::PS2Mouse(int clk, int data){
  MCLK=clk;
  MDATA=data;
}

void PS2Mouse::getPosition(uint8_t &stat, int &x, int &y, int &z){
  char mstat;
  char mx;
  char my;
  char mz;
  /* get a reading from the mouse */
  mouse_write(0xeb);  /* give me data! */
  mouse_read();      /* ignore ack */
  mstat = mouse_read();
  mx = mouse_read();
  my = mouse_read();
  mz = mouse_read();
  stat= (uint8_t)mstat;
  x= (int)mx;
  y= (int)my;
  z= (int)mz;
}

//--------------------------------------------------
/*
 * according to some code I saw, these functions will
 * correctly set the mouse clock and data pins for
 * various conditions.
 */
void PS2Mouse::gohi(int pin)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

void PS2Mouse::golo(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void PS2Mouse::mouse_write(char data)
{
  char i;
  char parity = 1;

  //  Serial.print("Sending ");
  //  Serial.print(data, HEX);
  //  Serial.print(" to mouse\n");
  //  Serial.print("RTS");
  /* put pins in output mode */
  gohi(MDATA);
  gohi(MCLK);
  delayMicroseconds(300);
  golo(MCLK);
  delayMicroseconds(300);
  golo(MDATA);
  delayMicroseconds(10);
  /* start bit */
  gohi(MCLK);
  /* wait for mouse to take control of clock); */
  while (digitalRead(MCLK) == HIGH)
    ;
  /* clock is low, and we are clear to send data */
  for (i=0; i < 8; i++) {
    if (data & 0x01) {
      gohi(MDATA);
    } 
    else {
      golo(MDATA);
    }
    /* wait for clock cycle */
    while (digitalRead(MCLK) == LOW)
      ;
    while (digitalRead(MCLK) == HIGH)
      ;
    parity = parity ^ (data & 0x01);
    data = data >> 1;
  }  
  /* parity */
  if (parity) {
    gohi(MDATA);
  } 
  else {
    golo(MDATA);
  }
  while (digitalRead(MCLK) == LOW)
    ;
  while (digitalRead(MCLK) == HIGH)
    ;
  /* stop bit */
  gohi(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH)
    ;
  /* wait for mouse to switch modes */
  while ((digitalRead(MCLK) == LOW) || (digitalRead(MDATA) == LOW))
    ;
  /* put a hold on the incoming data. */
  golo(MCLK);
  //  Serial.print("done.\n");
}

/*
 * Get a byte of data from the mouse
 */
char PS2Mouse::mouse_read(void)
{
  char data = 0x00;
  int i;
  char bit = 0x01;

  //  Serial.print("reading byte from mouse\n");
  /* start the clock */
  gohi(MCLK);
  gohi(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH)
    ;
  delayMicroseconds(5);  /* not sure why */
  while (digitalRead(MCLK) == LOW) /* eat start bit */
    ;
  for (i=0; i < 8; i++) {
    while (digitalRead(MCLK) == HIGH)
      ;
    if (digitalRead(MDATA) == HIGH) {
      data = data | bit;
    }
    while (digitalRead(MCLK) == LOW)
      ;
    bit = bit << 1;
  }
  /* eat parity bit, which we ignore */
  while (digitalRead(MCLK) == HIGH)
    ;
  while (digitalRead(MCLK) == LOW)
    ;
  /* eat stop bit */
  while (digitalRead(MCLK) == HIGH)
    ;
  while (digitalRead(MCLK) == LOW)
    ;
  /* put a hold on the incoming data. */
  golo(MCLK);
  //  Serial.print("Recvd data ");
  //  Serial.print(data, HEX);
  //  Serial.print(" from mouse\n");
  return data;
}

void PS2Mouse::mouse_init()
{ char mouseId;
   
  gohi(MCLK);
  gohi(MDATA);
  //  Serial.print("Sending reset to mouse\n");
  mouse_write(0xff);
  mouse_read();  /* ack byte */
  //  Serial.print("Read ack byte1\n");
  mouse_read();  /* blank */
  mouse_read();  /* blank */
  //  Serial.print("Setting sample rate 200\n");
  mouse_write(0xf3);  /* Set rate command */
  mouse_read();  /* ack */
  mouse_write(0xC8);  /* Set rate command */
  mouse_read();  /* ack */
  //  Serial.print("Setting sample rate 100\n");
  mouse_write(0xf3);  /* Set rate command */
  mouse_read();  /* ack */
  mouse_write(0x64);  /* Set rate command */
  mouse_read();  /* ack */
  //  Serial.print("Setting sample rate 80\n");
  mouse_write(0xf3);  /* Set rate command */
  mouse_read();  /* ack */
  mouse_write(0x50);  /* Set rate command */
  mouse_read();  /* ack */
  //  Serial.print("Read device type\n");
  mouse_write(0xf2);  /* Set rate command */
  mouse_read();  /* ack */
  mouse_read();  /* mouse id, if this value is 0x00 mouse is standard, if it is 0x03 mouse is Intellimouse */
  //  Serial.print("Setting wheel\n");
  mouse_write(0xe8);  /* Set wheel resolution */
  mouse_read();  /* ack */
  mouse_write(0x03);  /* 8 counts per mm */
  //mouse_write(0x01);
  mouse_read();  /* ack */
  mouse_write(0xe6);  /* scaling 1:1 */
  mouse_read();  /* ack */
  mouse_write(0xf3);  /* Set sample rate */
  mouse_read();  /* ack */
  mouse_write(0x28);  /* Set sample rate */
  mouse_read();  /* ack */
  mouse_write(0xf4);  /* Enable device */
  mouse_read();  /* ack */

  //  Serial.print("Sending remote mode code\n");
  mouse_write(0xf0);  /* remote mode */
  mouse_read();  /* ack */
  //  Serial.print("Read ack byte2\n");
  delayMicroseconds(20);
}

