// This #include statement was automatically added by the Particle IDE.
#include "rgb-controls/rgb-controls.h"
using namespace RGBControls;

// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

// Define hardware connections
int PIN_GATE_IN = D3;
int IRQ_GATE_IN = D0;
int PIN_ANALOG_IN = A0;

Led led(D0, D1, D2);
Color red(255, 0, 0);
Color green(0, 255, 0);
Color yellow(255, 255, 0);
Color white(255, 255, 255);

void callback(char* topic, byte* payload, unsigned int length);
MQTT client("52.34.171.0", 1883, callback);

//Green
void showGreen(){
    led.setColor(green);
    delay(1000);
}

//yellow
void showYellow(){
    led.setColor(yellow);
    delay(1000);
}

//red
void showRed(){
    led.setColor(red);
    delay(1000);
}

//white
void showWhite(){
    led.setColor(white);
    delay(1000);
}

void debug(String message, int value) {
    char msg [50];
    sprintf(msg, message.c_str(), value);
    Spark.publish("DEBUG", msg);
}
 

//Receive messages from MQ
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
    debug(message, 1);    
    
    if(message.equals("red")){
        debug("calling red", 1);  
        showRed();
    } else if(message.equals("yellow")){
        debug("calling yellow", 1);
        showYellow();
    } else if(message.equals("green")){
        debug("calling green", 1);
        showGreen();
    } else {
        debug("calling white", 1);
        showWhite();
    }
    delay(1000);
}


// soundISR()
// This function is installed as an interrupt service routine for the pin
// change interrupt.  When digital input 2 changes state, this routine
// is called.
// It queries the state of that pin, and sets the onboard LED to reflect that 
// pin's state.
void soundISR()
{
  int pin_val;
  pin_val = digitalRead(PIN_GATE_IN);
}

void setup()
{
  // configure input to interrupt
  pinMode(PIN_GATE_IN, INPUT);
  attachInterrupt(IRQ_GATE_IN, soundISR, CHANGE);
  showWhite();
  client.connect("sparkclient");
  if(client.isConnected()){
     client.subscribe("practice_status");
  }
}

void loop()
{
  int value;
  long duration;
  // Check the envelope input
  duration = 0;
  value = analogRead(PIN_ANALOG_IN);
  duration = pulseIn(PIN_GATE_IN, HIGH);
  if(client.isConnected()){
        client.loop();
  }

  // Convert envelope value into a message
  if(duration > 0){
    char message [255];
   snprintf(message, sizeof(message),"{\"type\":\"mic_sensor\",\"message\":\"duration_log\",\"product\":\"music_teacher\",\"value\":\"%d\"}",duration);
   if(client.isConnected()){
       debug("duration=>%d", duration); 
       client.publish("events_messages",message);
   }
  }
  // pause for 1 second
  delay(1000);
}
