#include <SPI.h>
#include <WiFiNINA.h>
#include <Servo.h>
#include <Wire.h>
#include "wifi_info.h" // file containing SSID and wifi password
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // defining screen width and height for OLED
#define SCREEN_HEIGHT 64

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;

int status = WL_IDLE_STATUS;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WiFiServer server(80);
boolean hasUser = false; // true when the first user connects to the website
int eyePos = 0; // which direction the robot is pointing for the eyes
String movement = "i am stopped"; // to describe the current direction and movement on the website
String somethingToSay = "hi"; // what the robot is displaying based on user input
Servo servo; // the servo used for steering

void setup() {
  pinMode(4, OUTPUT); // the pin for the driving motor
  servo.attach(9);
  servo.write(83); // setting the wheels foward
  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  if (WiFi.status() == WL_NO_MODULE) { // checking if connection failed
    display.print("Connection failed");
    display.display();
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) { // checking firmware
    display.print("Connection failed");
    display.display();
  } 
  display.print("Connecting..."); // tell user robot is connecting
  display.display();
  servo.write(83);
  while (status != WL_CONNECTED) { // connect to wifi
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin(); // start the server
  printWifi(); // print information for the user to connect
}


void loop() {
  WiFiClient client = server.available();

  int count = millis();

  if (count % 7000 == 0 && hasUser) { // blink the robots eyes (if there is a user)
    blynk(eyePos);
  }

  if (count % 1000 == 0 && hasUser) { // update display based on eyePos for eyes and somethingToSay for what its saying 
    display.clearDisplay();
    display.fillRoundRect(20 + eyePos, 0, 15, 15, 2, WHITE);
    display.fillRoundRect(94 + eyePos, 0, 15, 15, 2, WHITE);
    display.fillTriangle(60, 21, 68, 21, 64, 26, WHITE);
    display.fillRoundRect(0, 25, 128, 27, 1, BLACK);
    if (somethingToSay.length() < 22) {
      display.setCursor(64 - (somethingToSay.length() * 3), 50);
      display.print(somethingToSay);
    } else if (somethingToSay.length() >= 22) {
      display.setCursor(0, 39);
      display.print(somethingToSay);
    }
    display.display();
  }

  if (client) {
    hasUser = true;
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) { // html and css for the website
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<HTML>"
                         "<HEAD></HEAD>"
                         "<BODY>"
                         "<div id=""control"">"
                         "<a href=""/F"" class=""control-button"" id=""f"">forward</a>"
                         "<a href=""/S"" class=""control-button"" id=""s"">stop</a>"
                         "<a href=""/L"" class=""control-button"" id=""l"">left</a>"
                         "<a href=""/R"" class=""control-button"" id=""r"">right</a>"
                         "<p id=""movement"">" + movement + "</p>"
                         "</div>" // div containing move controls and current movement
                         "<div id=""speech"">"
                         "<form action=""/get""><label id=""something-label"" for=""something"">say something</label><input type=""text"" id=""something"" name=""something"" maxlength=""63"" size=""20""><br>"
                         "<input id=""submit"" type=""submit"" value=""submit""></form>"
                         "<p id=""saying"">now im saying: " + somethingToSay + "</p>"
                         "</div>" // div containing tools to update somethingToSay, and whats cuurently displayed
                         "<style type=\"text/css\">body {background-color: #ffd096; overflow-x: hidden;}" // css for style and layout
                         "#control {position: relative; top: 30px; max-width: 960px; margin: auto; text-align: center; width: 600px; height: 600px; border-radius: 100px; border: 10px solid #374b99;}"
                         ".control-button {position: absolute; text-decoration: none; font-family: 'Comic Sans MS', 'Marker Felt', sans-serif; font-size: 40px; color: #374b99;}"
                         "#f {left: 215px; top: 100px;}"
                         "#s {left: 250px; top: 270px;}"
                         "#l {left: 50px; top: 270px;}"
                         "#r {left: 450px; top: 270px;}"
                         "#movement {position: relative; top: 450px; font-family: 'Comic Sans MS', 'Marker Felt', sans-serif; font-size: 25px; color: #374b99;}"
                         "#speech {position: relative; top: 60px; max-width: 960px; margin: auto; width: 600px; height: 600px; border-radius: 100px; border: 10px solid #374b99;text-align: center;}"
                         "#something-label {  position: relative;top: 100px;font-family: 'Comic Sans MS', 'Marker Felt', sans-serif;font-size: 40px;color: #374b99;}"
                         "#something {  position: relative;top: 150px;border-radius: 50px;border: 5px solid #374b99;background: transparent;font-family: 'Comic Sans MS', 'Marker Felt', sans-serif;font-size: 40px;text-indent: 20px;color: #374b99;}"
                         "#submit {  position: relative;top: 170px;background: transparent;border: none;font-family: 'Comic Sans MS', 'Marker Felt', sans-serif;font-size: 40px;color: #374b99;}"
                         "#saying {  position: relative;top: 200px;font-family: 'Comic Sans MS', 'Marker Felt', sans-serif;font-size: 30px;color: #374b99;}"
                         "</style>"
                         "</BODY></HTML>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        if (currentLine.endsWith("GET /F")) { // checking if user has a get request for forward
          digitalWrite(4, HIGH); // moving forawrd
          servo.write(83); // pointing foward
          movement = "i am going forward"; // updating movement
        }
        if (currentLine.endsWith("GET /S")) { // checking if user has a get request for stopping
          digitalWrite(4, LOW); // stopping
          servo.write(83); // pointing forward
          eyePos = 0; // updating eyePos
          movement = "i am stopped"; // updating movement
        }
        if (currentLine.endsWith("GET /L")) { // checking if user has a get request for left
          servo.write(53); // pointing left
          eyePos = -10; // updating eyePos
          if(movement.indexOf("i am going forward") > -1) { // updating movement
            movement = "i am going forward to the left";
          } else {
            movement = "i am stopped to the left";
          }
        }
        if (currentLine.endsWith("GET /R")) { // checking if user has a get request for right
          servo.write(113); // pointing right
          eyePos = 10; // updating eyePos
          if(movement.indexOf("i am going forward") > -1) { // updating movement
            movement = "i am going forward to the right";
          } else {
            movement = "i am stopped to the right";
          }
        }
        if (currentLine.indexOf("GET /get?something=") > -1 && currentLine.indexOf("HTTP") > -1) { // checking if user has a get request to say something
          somethingToSay = currentLine.substring(currentLine.indexOf("GET /get?something=") + 19, currentLine.indexOf("HTTP")); // updating somethingToSay based on request
          for (int i = 0; somethingToSay[i] != 0; i++) { // replacing + with spaces
            if (somethingToSay[i] == '+') somethingToSay[i] = ' ';
          }
          somethingToSay.replace("%92", "'"); // replacing %92 with apostrophe
          somethingToSay.replace("%21", "!"); // replacing %21 with exclamation
        }
      }
    }
    client.stop();
  }
}

void printWifi() { // print wifi connection so user can connect
  display.clearDisplay();
  display.setCursor(0, 16);
  display.print("Connected");
  display.setCursor(0, 26);
  IPAddress ip = WiFi.localIP();
  display.print("Go to:");
  display.setCursor(0, 36);
  display.print("http://");
  display.println(ip); // print ip address for website
  display.display();
}

void blynk(int eyePos) { // binking robots eyes

  display.fillRoundRect(0, 0, 128, 15, 1, BLACK);
  display.display();
  display.drawLine(20 + eyePos, 8, 35 + eyePos, 8, WHITE);
  display.drawLine(94 + eyePos, 8, 109 + eyePos, 8, WHITE);
  display.display();
  delay(100);
  display.fillRoundRect(0, 0, 128, 15, 1, BLACK);
  display.display();
  display.fillRoundRect(20 + eyePos, 0, 15, 15, 2, WHITE);
  display.fillRoundRect(94 + eyePos, 0, 15, 15, 2, WHITE);
  display.display();
}
