#ifndef MOSFET_H
#define MOSFET_H

#define Negative_PIN 15 // no esquemático da v2 está 15, 23 na placa  V1
#define Positive_PIN 12 // no esquemático da v2 está 12, 22 na placa  V1




void MOSFET_Init(void);
void Negative_Set_State(bool state);
void Positive_Set_State(bool state);
void Save_MOSFET_STATE_FLASH(bool negative_state, bool positive_state);
bool VerifyMOSFET_state(void);

#endif