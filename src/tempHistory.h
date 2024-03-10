#include <Arduino.h>

class TempHistory {

  int length = 10;
  float stack[10] = {};
  int idx = 0;

  void shift(){
    for (int i = 0; i < length-1; i++){
        stack[i] = stack[i+1];
    }
  }

  public: 
  float getLast(){
    return stack[idx-1];
  }

  void push(float x){
    if (idx == (length-1)) shift();
    stack[idx] = x;
    if (idx < (length-1)) idx++;

    // for (int i = 0; i < length; i++){
    //     Serial.println(stack[i]);
    // }
  }

  float getAvg(){
    float sum = 0;
    for (int i = 0; i < length;i++){
        sum += stack[i];
    }
    return sum/idx;
  }

};
