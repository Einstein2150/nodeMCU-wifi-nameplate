#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//#####FLASH#######
#include <FS.h>
//#################

#include <WiFiClient.h>
//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>

//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80


#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);//0x27 is the adress of my I2C-Converter. Look up for your own adress!

//##############################################

const char* ssid = "YOUR-SSID";
const char* password = "YOUR-PASS";

int deboucelength = 90;
boolean freestatus = false;

String line[4] = {"123456789012345678","123456789012345678","123456789012345678","123456789012345678"};


//##############################################


void handleSwitch() {
  togglefree();
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleDisplay() {
  display(server.arg("line1"),server.arg("line2"),server.arg("line3"),server.arg("line4"));
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleWriteDisplay() {
  writeDisplayConfig();
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleRoot() {
  String out = "";
  out += "<form action=\"/switch\" method=\"POST\">"\
         "<input type=\"submit\" value=\"Switch Status\"></form>"\
         "<p>";
  out += String("Status is: ") + ((freestatus)?"free":"busy");
  out += "</p><h3>Displaytext:</h3><p>";
  out += line[0] + "</p><p>" + line[1] + "</p><p>" + line[2] + "</p><p>" + line[3] + "</p>";
  out += "<form action=\"/display\" method=\"POST\">"\
         "<input type=\"text\" name=\"line1\" placeholder=\"Line 1\"></br>"\
         "<input type=\"text\" name=\"line2\" placeholder=\"Line 2\"></br>"\
         "<input type=\"text\" name=\"line3\" placeholder=\"Line 3\"></br>"\
         "<input type=\"text\" name=\"line4\" placeholder=\"Line 4\"></br>"\
         "<input type=\"submit\" value=\"Change values\"></form>";
  out += "<form action=\"/write\" method=\"POST\">"\
         "<input type=\"submit\" value=\"Save values to EEPROM\"></form>"\
         "<p>";
  
  server.send(200, "text/html", out);
  }
//##############################################


void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

// The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(20,4);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("booting...");
  lcd.setCursor(0, 1);      
  lcd.print("                  ");
  lcd.setCursor(0, 2);      
  lcd.print("Config-IP is");
  lcd.setCursor(0, 3);      
  lcd.print(WiFi.localIP());

//#####FLASH#######
SPIFFS.begin();

readDisplayConfig();//load settings from EEPROM
display(line[0],line[1],line[2],line[3]);//Show the content

//#################


//Pins for LED and switch
pinMode(D7, OUTPUT);//free-LED-PIN
pinMode(D6, OUTPUT);//busy-LED-PIN
pinMode(D5, INPUT);//switch
togglefree();//setup switch action


//#######################################
server.on("/", handleRoot);
server.on("/switch", handleSwitch);
server.on("/display", handleDisplay);
server.on("/write", handleWriteDisplay);
//#######################################


  // Start the server 
  server.begin();
  Serial.println("HTTP server started");
}


//#######################################
void loop() {
  ArduinoOTA.handle();
  server.handleClient(); 
  switchstatus();
}
//#######################################

String extract(String inputvalue) {
  if (inputvalue.length() <= 18) {
    return inputvalue;
    }
  else
    {
      inputvalue = inputvalue.substring(0,18);
      return inputvalue;
      }
  }



void display(String newline1, String newline2, String newline3, String newline4) {

if (newline1 != NULL) {line[0] = extract(newline1);}
if (newline2 != NULL) {line[1] = extract(newline2);}
if (newline3 != NULL) {line[2] = extract(newline3);}
if (newline4 != NULL) {line[3] = extract(newline4);}

//Clear the Display
  lcd.setCursor(0, 0);
  lcd.print("                  ");
  lcd.setCursor(0, 1);      
  lcd.print("                  ");
  lcd.setCursor(0, 2);      
  lcd.print("                  ");
  lcd.setCursor(0, 3);      
  lcd.print("                  ");
  
//Write the new lines
  lcd.setCursor(0, 0);
  lcd.print(line[0]);
  lcd.setCursor(0, 1);      
  lcd.print(line[1]);
  lcd.setCursor(0, 2);      
  lcd.print(line[2]);
  lcd.setCursor(0, 3);      
  lcd.print(line[3]);
  }



void switchstatus() {
  if (digitalRead(D5) == LOW)
     {
        delay(deboucelength);//Debounce-Delay for the physical toggle-button
        while (1)
        {
        //switch the status when button is released
             if (digitalRead(D5) == HIGH)
             {
                 togglefree();
                 break;
              };
          };
       };
}



void togglefree() {
  if (freestatus)
    {
        lcd.setCursor(18, 0);
        lcd.print("|B");
        lcd.setCursor(18, 1);      
        lcd.print("|U");
        lcd.setCursor(18, 2);      
        lcd.print("|S");
        lcd.setCursor(18, 3);      
        lcd.print("|Y");
        digitalWrite(D7, HIGH);
        digitalWrite(D6, LOW);
        freestatus = false;
      }
  else
    {
        lcd.setCursor(18, 0);
        lcd.print("|F");
        lcd.setCursor(18, 1);      
        lcd.print("|R");
        lcd.setCursor(18, 2);      
        lcd.print("|E");
        lcd.setCursor(18, 3);      
        lcd.print("|E");
        digitalWrite(D7, LOW);
        digitalWrite(D6, HIGH);
        freestatus = true;
        }
  }



void readDisplayConfig() {
  for (int i=1; i < 5; i++){
    String filename = String("/line") + i + ".txt";
        Serial.println("opening " + filename);
    File tempfile = SPIFFS.open(filename, "r");
    if (!tempfile) {
        Serial.println("file open failed");
        //Creating the missing file:
        File newfile = SPIFFS.open(filename, "w");
        if (!newfile) {Serial.println("Kritischer Fehler beim erzeugen der neuen Konfigurationsdatei");}
        newfile.print(String(line[(i-1)]));
    }
    else {
    line[(i-1)] = tempfile.readString(); // Inhalt der Textdatei wird gelesen...
      }
    tempfile.close();
   }
}

void writeDisplayConfig() {
  Serial.println("Start writing...");
  for (int i=1; i < 5; i++){
    String filename = String("/line") + i + ".txt";
        Serial.println("opening " + filename);
    File tempfile = SPIFFS.open(filename, "r+");
    if (!tempfile) {
        Serial.println("file open failed");
    }
    else {
    int linelength = String(line[(i-1)]).length(); //Laenge vom aktuellen String speichern
    for (int b = linelength; b<18 ;b++) {
      line[(i-1)] += " ";//auffüllen mit Leerzeichen
      }
    
    tempfile.print(line[(i-1)]); // Inhalt der Zeile wird geschrieben...
      }
      Serial.println("line written in " + filename);
    tempfile.close();
   }
}
