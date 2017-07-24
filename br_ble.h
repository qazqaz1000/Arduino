#ifndef _BR_BLE_H_
#define _BR_BLE_H_

#include <SoftwareSerial.h>
#define BLE_MSG_S_REQ_OPEN        "REQ:S:OPEN"
#define BLE_MSG_S_RES_OPENOK      "RES:S:OPENOK"
#define BLE_MSG_S_ALM_OPENED      "ALM:S:OPENED"
#define BLE_MSG_S_ALM_CLOSED      "ALM:S:CLOSED"
#define BLE_MSG_S_ALM_LONGOPENED  "ALM:S:LONGOPENED"
#define BLE_MSG_S_RES_OPENFAIL    "RES:S:OPENFAIL"
#define BLE_MSG_S_RES_POWERON     "RES:S:POWERON"
#define BLE_MSG_S_RES_POWEROFF    "RES:S:POWEROFF"

#define BLE_MSG_D_RES_OPENOK      "RES:D:OPENOK"
#define BLE_MSG_D_ALM_OPENED      "ALM:D:OPENED"
#define BLE_MSG_D_ALM_CLOSED      "ALM:D:CLOSED"
#define BLE_MSG_D_ALM_LONGOPENED  "ALM:D:LONGOPENED"
#define BLE_MSG_D_RES_OPENFAIL    "RES:D:OPENFAIL"

//OPENFAIL

int brBleMsgParse();
int brBleMsgParse_MAC(String *receive_MAC);

#endif
