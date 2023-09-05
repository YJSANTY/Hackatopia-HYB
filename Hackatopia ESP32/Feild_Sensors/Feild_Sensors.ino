//************************************************************
// this is a simple example that uses the painlessMesh library
//
// 1. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 2. prints anything it receives to Serial.print
//
//
//************************************************************
#include "painlessMesh.h"

#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555 



#include<Wire.h>
#include "DHT.h"
#define DHTTYPE DHT11
#define DHTPIN D4
DHT dht(DHTPIN, DHTTYPE);

struct 
{
  float temp = 0.00;
  float humi = 0;
  int ldr  = 0;
  int soil = 0;
} data;


Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;



void read_dht() {
  data.temp = dht.readTemperature();
  data.humi = dht.readHumidity();
}

void read_ldr() {
  data.ldr = digitalRead(D5);
}

void read_soil() {
  data.soil = map(analogRead(A0), 0, 1023, 100, 0);
}



// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {

    read_dht();
    read_soil();
    read_ldr();
    String msg_1 =   String("t") + String(data.temp)  ;
    String msg_2 =   String("l") +String(data.ldr);
    String msg_3 =   String("s") +String(data.soil);
    String msg_4 =   String("h") +String(data.humi);

  mesh.sendBroadcast( msg_1 );
  mesh.sendBroadcast( msg_2 );
  mesh.sendBroadcast( msg_3 );
  mesh.sendBroadcast( msg_4 );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  delay(10);
  dht.begin();
  pinMode(D5, INPUT);
  pinMode(A0, INPUT);
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
  taskSendMessage.enable();
  taskSendMessage.enable();
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
}
