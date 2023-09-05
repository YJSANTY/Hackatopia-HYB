#include "painlessMesh.h"
#include<Wire.h>


#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

struct {
  int soil = 0;
  int mq2 = 0;  // MQ2 sensor reading
  int mq3 = 0;  // MQ3 sensor reading
  int ldr = 0;
} data;

Scheduler userScheduler; 
painlessMesh  mesh;

void sendMessage();  // Forward declaration

Task taskSendMessage(TASK_SECOND * 1, TASK_FOREVER, &sendMessage);

void read_MQ2() {
  data.mq2 = digitalRead(D0);  // MQ2 sensor connected to D0
}

void read_MQ3() {
  data.mq3 = analogRead(A0);  // MQ3 sensor connected to A0
}


void sendMessage() {
    read_MQ2();
    read_MQ3();
    
    String msg_1 = String("t") + String(data.mq2);
    String msg_2 = String("l") + String(data.soil);
    String msg_3 = String("s") + String(data.ldr);
    String msg_4 = String("h") + String(data.mq3);
    Serial.printf("Messaged");
    mesh.sendBroadcast(msg_1);
    mesh.sendBroadcast(msg_2);
    mesh.sendBroadcast(msg_3);
    mesh.sendBroadcast(msg_4);

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
  delay(10);
  
  pinMode(D0, INPUT);  // Set D0 as INPUT for MQ2

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
  mesh.update();
}
