#include "switches.h"

// -------------------------------
// initSwitches() Function
// -------------------------------
void initSwitches() {

  for (uint8_t i=0; i<NUM_SERVO; i++)
  {
    pinMode(Pin_Servos[i],INPUT);
  }
  pinMode(Pin_Solenoid,INPUT);
  for (uint8_t i=0; i<NUM_EMATCH; i++)
  {
    pinMode(Pin_Ematches[i],INPUT);
  }
  pinMode(Pin_Arm,INPUT);
  pinMode(Pin_AbortSwitch,INPUT);
  pinMode(Pin_LoadCellTare,INPUT);
  pinMode(Pin_SpareTare,INPUT);
}


// -------------------------------
// readSwitches() Function
// -------------------------------
void readSwitches(CommandPacket &cmd) {

  // Servo Commands
  for (uint8_t i=0; i<NUM_SERVO; i++)
  {
    cmd.servoState[i] = digitalRead(Pin_Servos[i]);
  }

  // Solenoid Command
  cmd.solenoidState = digitalRead(Pin_Solenoid);

  // Ematch Commands
  cmd.armedState = digitalRead(Pin_Arm);
  for (uint8_t i=0; i<NUM_EMATCH; i++)
  {
    cmd.ematchState[i] = digitalRead(Pin_Ematches[i]);
  }

  // Abort State
  if (digitalRead(Pin_AbortSwitch)) {

    // close fill, close solenoid, turn off ematches, vent line and tank.
    cmd.servoState[0] = false;
    cmd.servoState[1] = true;
    cmd.servoState[2] = true; 
    cmd.servoState[3] = true;
    cmd.solenoidState = false;
    cmd.armedState = false;
    cmd.ematchState[0] = false;
    cmd.ematchState[1] = false;
  }

  // Tare Command
  if (digitalRead(Pin_LoadCellTare)) {
    cmd.tareLoadCellState = true;
  }
  else
  {
    cmd.tareLoadCellState = false;
  }

}
