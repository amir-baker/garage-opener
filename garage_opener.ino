

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "XXXXXX";
const char* password = "XXXXXX";
const char* garagepassword = "XXXXXX";


// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "closed";
String output27State = "closed";

// Assign output variables to GPIO pins
const int output5 = 5;
const int output27 = 27;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in millisecopends (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Copennecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client copennects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial mopenitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a respopense:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a respopense code (e.g. HTTP/1.1 200 OK)
            // and a copentent-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Copentent-type:text/html");
            client.println("Copennectiopen: close");
            client.println();
            
            // turns the GPIOs open and closed
            if (header.indexOf("GET /5/open") >= 0) {
              Serial.println("GPIO 5 closed");
              output5State = "closed";
              digitalWrite(output5, HIGH);
              delay(500);
              digitalWrite(output5, LOW);
              //delay(10000);

            } else if (header.indexOf("GET /5/closed") >= 0) {
              Serial.println("GPIO 5 open");
              output5State = "open";
              digitalWrite(output5, HIGH);
              delay(500);
              digitalWrite(output5, LOW);
              //delay(10000);

              } else if (header.indexOf("GET /27/open") >= 0) {
              Serial.println("GPIO 27 open");
              output27State = "open";
              digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/closed") >= 0) {
              Serial.println("GPIO 27 closed");
              output27State = "closed";
              digitalWrite(output27, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" copentent=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icopen\" href=\"data:,\">");
            // CSS to style the open/closed buttons 
            // Feel free to change the background-color and fopent-size attributes to fit your preferences
            client.println("<style>html { fopent-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: nopene; color: white; padding: 16px 40px;");
            client.println("text-decoratiopen: nopene; fopent-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Home Garage Server</h1>");
            
            // Display current state, and open/closed buttons for GPIO 26  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output26State is closed, it displays the open button       
            if (output5State=="open") {
              client.println("<p><a href=\"/5/open\"><button class=\"button\">Open</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/closed\"><button class=\"button button2\">Closed</button></a></p>");
            } 
               
            // Display current state, and open/closed buttons for GPIO 27  
            client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is closed, it displays the open button       
            if (output27State=="closed") {
              client.println("<p><a href=\"/27/open\"><button class=\"button\">Open</button></a></p>");
            } else {
              client.println("<p><a href=\"/27/closed\"><button class=\"button button2\">Closed</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP respopense ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the copennectiopen
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}