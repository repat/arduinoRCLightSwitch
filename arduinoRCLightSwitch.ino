/**
Original Code by RCSwitch Project: http://code.google.com/p/rc-switch/
Update by repat<repat@repat.de>, April2013
*/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h> // this is new
#include <EthernetServer.h> // so is this
#include <RCSwitch.h>

// Ethernet configuration
// MAC Address
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// IP Address
byte ip[] = { 
  192,168,1, 42 };
// EthernetServer Port 80
EthernetServer EthernetServer(80);

// RCSwitch configuration
RCSwitch mySwitch = RCSwitch();
// Port 7
int RCTransmissionPin = 7;


/**
 * Setup
 */
void setup() {
  // Init Ethernet connection
  Ethernet.begin(mac, ip);
  EthernetServer.begin();
  // Init RCSwitch
  mySwitch.enableTransmit( RCTransmissionPin );
}

/**
 * Loop
 */
void loop() {
  char* command = httpEthernetServer();
}

/**
 * Command dispatcher
 */
void processCommand(char* command) {
  if (strcmp(command, "1-on") == 0) {
    mySwitch.switchOn("11001", "10000");
  } 
  else if (strcmp(command, "1-off") == 0) {
    mySwitch.switchOff("11001", "10000");
  } 
  else if (strcmp(command, "2-on") == 0) {
    mySwitch.switchOn("11001", "01000");
  } 
  else if (strcmp(command, "2-off") == 0) {
    mySwitch.switchOff("11001", "010000");
  }
  else if (strcmp(command, "3-on") == 0) {
        mySwitch.switchOff("11001", "001000");
  } 
  else if (strcmp(command, "3-off") == 0) {
    mySwitch.switchOff("11001", "001000");
  }
}

/**
 * HTTP Response with homepage
 */
void httpResponseHome(EthernetClient c) {
  c.println("HTTP/1.1 200 OK");
  c.println("Content-Type: text/html");
  c.println();
  c.println("<html>");
  c.println("<body>");
  c.println(    "<h1>Repats Lampen</h1>");
  c.println(    "<ul>");
  c.println(        "<li><a href=\"./?1-on\">Stehlampe on</a></li>");
  c.println(        "<li><a href=\"./?1-off\">Stehlampe off</a></li>");
  c.println(    "</ul>");
  c.println(    "<ul>");
  c.println(        "<li><a href=\"./?2-on\">LEDs on</a></li>");
  c.println(        "<li><a href=\"./?2-off\">LEDs off</a></li>");
  c.println(    "</ul>");
  c.println(    "<ul>");
  c.println(        "<li><a href=\"./?3-on\">Schreibtischlampe on</a></li>");
  c.println(        "<li><a href=\"./?3-off\">Schreibtischlampe off</a></li>");
  c.println(    "</ul>");
  c.println("</body>");
  c.println("</html>");
}

/**
 * Process HTTP requests, parse first request header line and
 * call processCommand with GET query string (everything after
 * the ? question mark in the URL).
 */
char*  httpEthernetServer() {
  EthernetClient EthernetClient = EthernetServer.available();
  
  if (EthernetClient) {
    
    char sReturnCommand[32];
    int nCommandPos=-1;
    sReturnCommand[0] = '\0';
    
    while (EthernetClient.connected()) {
      
      if (EthernetClient.available()) {
        
        char c = EthernetClient.read();
        
        if ((c == '\n') || (c == ' ' && nCommandPos>-1)) {
          
          sReturnCommand[nCommandPos] = '\0';
          
          if (strcmp(sReturnCommand, "\0") == 0) {
            httpResponseHome(EthernetClient);
          } 
          else {
            processCommand(sReturnCommand);
            httpResponseRedirect(EthernetClient);
          }
          break;
        }
        
        if (nCommandPos>-1) {
          sReturnCommand[nCommandPos++] = c;
        }
        if (c == '?' && nCommandPos == -1) {
          nCommandPos = 0;
        }
      }
      
      if (nCommandPos > 30) {
        httpResponse414(EthernetClient);
        sReturnCommand[0] = '\0';
        break;
      }
    }
    
    
    if (nCommandPos!=-1) {
      sReturnCommand[nCommandPos] = '\0';
    }
    
    // give the web browser time to receive the data
    delay(1);
    
    EthernetClient.stop();

    return sReturnCommand;
  }
  return '\0';
}

/**
 * HTTP Redirect to homepage
 */
void httpResponseRedirect(EthernetClient c) {
  c.println("HTTP/1.1 301 Found");
  c.println("Location: /");
  c.println();
}

/**
 * HTTP Response 414 error
 * Command must not be longer than 30 characters
 **/
void httpResponse414(EthernetClient c) {
  c.println("HTTP/1.1 414 Request URI too long");
  c.println("Content-Type: text/plain");
  c.println();
  c.println("414 Request URI too long");
}


