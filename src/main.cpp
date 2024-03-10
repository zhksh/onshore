#include <Arduino.h>
#include <tempHistory.h>

#include <stdbool.h>
#include <RCSwitch.h>
#include <debug.h>
#include <util.h>
#include <vals.h>


#define RECEIVER_PIN 0     //interrupt 0 == pin 2

#define BAUD  9600


RCSwitch receiver = RCSwitch();
TempHistory hist = TempHistory();

void setup() {
  Serial.begin(BAUD);
  Serial.println("Receiver setup");
  receiver.enableReceive(RECEIVER_PIN);
}


void encodeForUI(float tb, float temp0, float temp1, float aux){
  Serial.print("DATA|");
  
  //key for ui
  Serial.print("tb");
  Serial.print(":");
  Serial.print(tb);
  Serial.print(";");

  //key for ui
  Serial.print("tmp0");
  Serial.print(":");
  Serial.print(temp0);
  Serial.print(";");

  //key for ui
  Serial.print("tmp1");
  Serial.print(":");
  Serial.print(temp1);
  Serial.print(";");

   //key for ui
  Serial.print("aux");
  Serial.print(":");
  Serial.print(aux);

  Serial.println("");
}


float convertTransToTemp(long transmitted){
  //we want to make a byte a float with one decimal and therefore need transmitted < 1000
  if (transmitted >= 1000) return (float) transmitted;
  float temp = (float) transmitted/10;
  //we assume if tempt drops by more than 20C cmp to avg and last value its an overflow
  //and add the cutoff value, which should be precisely 256/10
  if ((temp + 20) < hist.getAvg() &&  hist.getLast() - temp  > 20){
    temp += 25.6;
  } 
  hist.push(temp);

  return temp;
}


void loop() {
  if (receiver.available()) {
    unsigned long data = receiver.getReceivedValue();


    if (islegit(data, SENSOR_RC_PREFIX, SENSOR_RC_PREFIX_POS, SENSOR_RC_PREFIX_LEN)) {
      long tb_val =  BIT_RANGE(data, TURB_LEN, TURB_POS);
      long temp0_val = BIT_RANGE(data, TEMP_LEN, TEMP0_POS);
      long temp1_val = BIT_RANGE(data, TEMP_LEN, TEMP1_POS);


      float temp0 = convertTransToTemp(temp0_val);
      float temp1 = convertTransToTemp(temp1_val);
      // Serial.println("##############");
      // Serial.println(temp0);
      // Serial.println(temp1);
      encodeForUI(tb_val, temp0, temp1, 0.0);
    }
    else {
      Serial.print("Noise: ");
      debug(receiver);
    }
    
    receiver.resetAvailable();
  }
}