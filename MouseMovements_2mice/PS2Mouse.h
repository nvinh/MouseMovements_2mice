#ifndef __PS2_H__
#define __PS2_H__

#include <Arduino.h>

class PS2Mouse {
  public:
    PS2Mouse(int data, int clk);
    void mouse_write(char data);
    char mouse_read(void);
    void mouse_init();
    void getPosition(uint8_t &stat, int &x, int &y, int &z);

  private:
    int MDATA;
    int MCLK;
    void golo(int pin);
    void gohi(int pin);
};

#endif // __PS_2_H__
