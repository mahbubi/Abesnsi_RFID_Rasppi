#include <SPI.h>
#include <Servo.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

#define Trigpin 8
#define Echopin 7
int pos = 0;
int counter = 0;
//int counter1;
int curstate = 0;
int prevstate = 0;
String coun_str;
char count[50];
String readString;
long duration, distance;
// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };

IPAddress dnServer(10, 13, 10, 13);
IPAddress gateway(10, 33, 107, 254);
IPAddress subnet(255, 255, 255, 0);
IPAddress server(10, 33, 109, 82);
IPAddress ip(10, 33, 107, 135);
Servo microservo;
Servo servo_test;
LiquidCrystal_I2C lcd(0x27, 16, 2);


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("aaa", count);
      // ... and resubscribe
      //client.subscribe("topic/Kamu");
      //Serial.println(client.subscribe("Kamu"));
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(57600);

  lcd.begin();
  pinMode(Trigpin, OUTPUT);
  pinMode(Echopin, INPUT);
  servo_test.attach(9);

  client.setServer(server, 1883);
  client.setCallback(callback);

  servo_test.write(0);
  Ethernet.begin(mac, ip, dnServer, gateway, subnet);
  //Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop() {
  digitalWrite(Trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigpin, LOW);
  duration = pulseIn(Echopin, HIGH);
  distance = duration / 58.2;
  coun_str = String(counter);
  coun_str.toCharArray(count, coun_str.length()+1 );

  if (distance < 10 ) {
    //Serial.println("PINTU TERBUKA");
    lcd.print("PINTU TERBUKA");
    lcd.setCursor(0, 1);
    servo_test.write(90);
    lcd.setCursor(0, 2);
    lcd.print(" ");
    curstate = 1;
    delay (2000);
  }

  if (distance > 10)  {
    servo_test.write(0);
    curstate = 0;
    lcd.clear();
  }

  if (curstate != prevstate) {
    if (curstate == 1) {
      counter = counter + 1;
      client.publish("aaa",count);
    }
  }

  if (!client.connected()) {
    reconnect();
  } else {
    client.publish("aaa",count);
  }
  client.loop();
}
