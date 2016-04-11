// This #include statement was automatically added by the Particle IDE.
#include "rgb-controls/rgb-controls.h"
using namespace RGBControls;

// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

// Define hardware connections
int PIN_GATE_IN = D3;
int PIN_LED_OUT = D7;
int IRQ_GATE_IN = RX;
int PIN_ANALOG_IN = A0;

Led led(D0, D1, D2);
Color red(255, 0, 0);
Color green(0, 255, 0);
Color yellow(255, 255, 0);
Color white(255, 255, 255);
Color currentColor(0, 0, 0);

void callback(char* topic, byte* payload, unsigned int length);
MQTT client("52.34.171.0", 1883, callback);

void debug(String message, int value) {
    char msg [100];
    sprintf(msg,message.c_str(), value);
    Spark.publish("DEBUG", msg);
}
 




//Show Color
void showColor(){
    led.setColor(currentColor);
    delay(1000);
}



//Receive messages from MQ
void callback(char* topic, byte* payload, unsigned int length) {
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
    debug(message, 1);    
    
    if(message.equals("red")){
        currentColor = red;
    } else if(message.equals("yellow")){
       currentColor = yellow;
    } else if(message.equals("green")){
        currentColor = green;
    } else {
        currentColor = white;
    }
    showColor();
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
  digitalWrite(PIN_LED_OUT, pin_val);  
}

void setup()
{
  // configure input to interrupt
  pinMode(PIN_GATE_IN, INPUT);
  pinMode(PIN_LED_OUT, OUTPUT);
  attachInterrupt(IRQ_GATE_IN, soundISR, CHANGE);
  currentColor = white;
  showColor();
  client.connect("sparkclient");
  if(client.isConnected()){
     client.subscribe("practice_status");
  }
}

void loop()
{

  //MQTT
  if(client.isConnected()){
        client.loop();
  }

  
  int value = 0;
  long duration = 0;
  
  // Check the envelope input
  duration = pulseIn(PIN_GATE_IN, HIGH);
  value = analogRead(PIN_ANALOG_IN);
     
  // Convert envelope value into a message
  if(duration > 0){
    char message [255];
   snprintf(message, sizeof(message),"{\"type\":\"mic_sensor\",\"message\":\"duration_log\",\"product\":\"music_teacher\",\"value\":\"%d\"}",duration);
   if(client.isConnected()){
       debug("duration=>%d", duration); 
       client.publish("events_messages",message);
       delay(1000);
   }
  }
  // pause for 1 second
  delay(1000);
}
