/*  NETPIE ESP8266 basic sample                            */
/*  More information visit : https://netpie.io             */

#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(13, 15); //RX TX

const char* ssid     = "Zenfone 2";
const char* password = "0870548627";

#define APPID   "takpa"
#define KEY     "CTxSrwnrQP0LYVw"
#define SECRET  "vWSm4rTCnnAXJ0L2eHXjmudpD"
#define ALIAS   "takpa0"

WiFiClient client;

int timer = 0;
MicroGear microgear(client);

/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
    /*if(strcmp(msg,"on") == 0){
      
    }else if(strcmp(msg,"off") == 0){
    
    }*/
    mySerial.print((char *)msg);
    mySerial.write('\r');
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
    Serial.println("Connected to NETPIE...");
    /* Set the alias of this microgear ALIAS */
    microgear.setAlias(ALIAS);
}


void setup() {
    /* Add Event listeners */

    /* Call onMsghandler() when new message arraives */
    microgear.on(MESSAGE,onMsghandler);

    /* Call onConnected() when NETPIE connection is established */
    microgear.on(CONNECTED,onConnected);

    Serial.begin(115200);
    Serial.println("Starting...");

    /* Initial WIFI, this is just a basic method to configure WIFI on ESP8266.                       */
    /* You may want to use other method that is more complicated, but provide better user experience */
    if (WiFi.begin(ssid, password)) {
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }

    Serial.println("WiFi connected");  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    /* Initial with KEY, SECRET and also set the ALIAS here */
    microgear.init(KEY,SECRET,ALIAS);

    /* connect to NETPIE to a specific APPID */
    microgear.connect(APPID);

    
    mySerial.begin(115200);
}

void loop() {
    /* To check if the microgear is still connected */
    if (microgear.connected()) {
        //Serial.println("connected");

        /* Call this method regularly otherwise the connection may be lost */
        microgear.loop();

        String tmp = mySerial.readStringUntil('\r');
        if(tmp != ""){
            //Serial.println("writing feed...");
            Serial.println(tmp);
            char topic[128];
            sprintf(topic,"/data/%s",ALIAS);
            microgear.publish(topic,tmp,true);
        }    
    }
    else {
        Serial.println("connection lost, reconnect...");
        if (timer >= 5000) {
            microgear.connect(APPID);
            timer = 0;
        }
        else timer += 100;

        delay(100);
    }
}
