#include <HXServo.h>

HXServo mySrv;

#define SRVID 103

#define BTN1  2
#define BTN2  3
#define LONGPRESSTIME 800

bool btnLongPressed[2] = {false, false};
bool isRun = false;
bool isCW = false;

byte speed = 20;

unsigned long btnStartTime[2];
unsigned long btnPressedTime[2];

void btn1Event(){
  if(!digitalRead(BTN1))
    btnStartTime[0] = millis();
  else
    btnPressedTime[0] = millis() - btnStartTime[0];
}

void btn2Event(){
  if(!digitalRead(BTN2))
    btnStartTime[1] = millis();
  else
    btnPressedTime[1] = millis() - btnStartTime[1];
}



void setup(){
  Serial.begin(115200);
  Serial.setTimeout(10);

  delay(500);

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN1), btn1Event, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN2), btn2Event, CHANGE);

  mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 1);   //stop
}

void loop(){

  //indicate whether buttons is shortly or longly pressed and do some jobs according to status.
  for(byte i=0; i<2; i++){
    if(btnPressedTime[i]){
      switch (i) {
        case 0:
          if(btnPressedTime[i] > LONGPRESSTIME){
            isRun = !isRun;
            if (!isRun)
              mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 1);   //stop
            else{
              if (isCW)
                mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 4);   //cw
              else
                mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 5);   //ccw
            }
          }
          else{
            speed += 5;
            mySrv.writeServo(SRVID, mySrv.SPEED_REGADDR, speed);
          }
          break;

        case 1:
          if(btnPressedTime[i] > LONGPRESSTIME){
            if (isRun) {
              isCW =!isCW;
              if (isCW)
                mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 4);   //cw
              else
                mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 5);   //ccw
            }
          }
          else{
            speed -= 5;
            mySrv.writeServo(SRVID, mySrv.SPEED_REGADDR, speed);
          }
          break;
      }

      btnPressedTime[i] = 0;
    }
  }

  while (Serial.available()) {
    char inChar = Serial.read();
    switch (inChar) {
      case 'r':
        isRun = true;
        if (isCW)
          mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 4);   //cw
        else
          mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 5);   //ccw
        break;

      case 's':
        isRun = false;
        mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 1);   //stop
        break;

      case 'c':
        if (isRun) {
          isCW = false;
          mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 5);   //ccw
        }
        break;

      case 'w':
        if (isRun) {
          isCW = true;
          mySrv.writeServo(SRVID, mySrv.WORKING_MODE_REGADDR, 4);   //ccw
        }
        break;

      case 'v':
        speed = Serial.parseInt();
        mySrv.writeServo(SRVID, mySrv.SPEED_REGADDR, speed);
    }
  }
}
