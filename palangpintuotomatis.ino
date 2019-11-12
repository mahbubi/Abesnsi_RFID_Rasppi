#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h> 
#include <LiquidCrystal_I2C.h>
#define Trigpin 8
#define Echopin 7
Servo microservo; 
int pos = 0; 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 10, 33, 107, 178 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 10, 33, 107, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };                  //subnet mask
EthernetServer server(80);                             //server port     
int counter = 0;
int curstate = 0;
int prevstate = 0;
String readString;
long duration, distance;
Servo servo_test;
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
  lcd.begin();
  pinMode(Trigpin, OUTPUT);
  pinMode(Echopin, INPUT);
  servo_test.attach(9);
  Serial.begin(9600);
  servo_test.write(0);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}
void loop() {
  // Create a client connection
  digitalWrite(Trigpin, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trigpin, LOW);
  duration = pulseIn(Echopin, HIGH);
  distance = duration / 58.2;
  EthernetClient client = server.available();
   if (distance < 10 )
   {
   //Serial.println("PINTU TERBUKA");
   lcd.print("PINTU TERBUKA");
   lcd.setCursor(0, 1);
   servo_test.write(90);
   lcd.setCursor(0,2);
   lcd.print(" ");
   curstate = 1;
   delay (2000);
   }
   if (distance > 10)  {
     servo_test.write(0);
     curstate =0;
     lcd.clear();
   }
   if(curstate != prevstate){
    if(curstate == 1){
      counter = counter + 1;
      Serial.print("Kendaraan Yang Masuk :");
      Serial.println(counter);
      }
      }
   if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        //read char by char HTTP request
        if (readString.length() < 100) {
          //store characters to string
          readString += c;
          //Serial.print(c);
         }

         //if HTTP request has ended
         if (c == '\n') {          
           Serial.println(readString); //print to serial monitor for debuging
     
           client.println("HTTP/1.1 200 OK"); //send new page
           client.println("Content-Type: text/html");
           client.println();     
           client.println("<HTML>");
           client.println("<HEAD>");
           client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
           client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
           client.println("<link rel='stylesheet' type='text/css' href='http://randomnerdtutorials.com/ethernetcss.css' />");
           client.println("<TITLE>Palang Pintu Otomatis</TITLE>");
           client.println("</HEAD>");
           client.println("<BODY>");
           client.println("<H1>Mahbubi Hamdani Responsi Project</H1>");
           client.println("<hr />");
           client.println("<br />");  
           client.println("<H2>Arduino with Ethernet Shield</H2>");
           client.println("<H2>Jumlah Kendaraan Yang Masuk : </H2>");
           client.println(counter);
           client.println("<H4>Untuk Membuka Manual silahkan tekan</H4>");
           client.println("<br />");  
           client.println("<a href=\"/?button1on\"\"><button>Open</button></a>");   
           client.println("<br />"); 
           client.println("</BODY>");
           client.println("</HTML>");
     
           delay(1);
           //stopping client
           client.stop();
           //controls the Arduino if you press the buttons
          if (readString.indexOf("?button1on")){
            lcd.print("PINTU TERBUKA");
            curstate = 1;
            lcd.setCursor(0, 1);
            servo_test.write(90);
            lcd.setCursor(0,2);
            lcd.print(" ");
            delay (2000);
            }
            else {
              curstate = 0;
              servo_test.write(0);
              lcd.clear();
             }         
            }           
           }
           //clearing string for next read
            readString="";  
           
         }
       }
    }
