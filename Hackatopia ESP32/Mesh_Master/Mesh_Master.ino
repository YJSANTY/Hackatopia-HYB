#include "painlessMesh.h"
#include <WiFi.h>

#define MESH_PREFIX     "whateverYouLike"
#define MESH_PASSWORD   "somethingSneaky"
#define MESH_PORT       5555

struct {
  int temp_1 = 5;
  int humi_1 = 5;
  int ldr_1  = 5;
  int soil_1 = 5;
  int temp_2 = 5;
  int humi_2 = 5;
  int ldr_2  = 5;
  int soil_2 = 5;
} alpha;

Scheduler userScheduler; // to control your personal task
painlessMesh mesh;

// Assuming you're using Serial2 for communication with NodeMCU
#define TXD2 17
#define RXD2 16

void sendMessage(); // Prototype

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

void sendMessage() {
  String msg = "Received";
  taskSendMessage.setInterval(random(TASK_SECOND * 1, TASK_SECOND * 5));
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(4800, SERIAL_8N1, RXD2, TXD2); // Using Serial2 for communication with NodeMCU

  mesh.setDebugMsgTypes(ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE);
  mesh.setDebugMsgTypes(ERROR | STARTUP);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask(taskSendMessage);
  taskSendMessage.enable();
}

void loop() {
  Serial.println(alpha.temp_1);
  Serial.println(alpha.humi_1);
  Serial.println(alpha.ldr_1);
  Serial.println(alpha.soil_1);
  Serial.println(alpha.temp_2);
  Serial.println(alpha.humi_2);
  Serial.println(alpha.ldr_2);
  Serial.println(alpha.soil_2);

  // Sending data to another device via Serial2
  Serial2.print("h");
  Serial2.println(alpha.temp_1);
  Serial2.println(alpha.humi_1);
  Serial2.println(alpha.ldr_1);
  Serial2.println(alpha.soil_1);
  Serial2.println(alpha.temp_2);
  Serial2.println(alpha.humi_2);
  Serial2.println(alpha.ldr_2);
  Serial2.println(alpha.soil_2);

  mesh.update();
}
