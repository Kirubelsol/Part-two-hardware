#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

#include "M5Atom.h"
#include "math.h" //for acceleration and related calculations
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#define Data_pin 27 //specific for ATOM matrix

bool IMU6886Flag = false;
float accX = 0, accY = 0, accZ = 0, gyroX, gyroY, gyroZ;
uint8_t DisBuff[2 + 5 * 5 * 3];
// Set these to your desired credentials.
const char *ssid = "Group 8 Fidgeting";
const char *password = "Fidgeting";

long var = 0;
int trial = 0;
long previoustime = 0; // for getting values in specific time range

double pitch, roll; // to calculate arc value for tapping motion
double r_rand = 180 / PI;
float arc;
float arc1 = 0;
float arc2 = 0;
float arcdiff = 0; // difference of the arc is the one used to detect tapping motion
float gyroY1 = 0;
float gyroY2 = 0;
float gyroYdiff = 0; // difference of gyroY value to detect sideways motion
double val;
int check1 = 1; // for checking up and dowm leg fidgeting (intensity)
int check2 = 1; // for checking approximate side to side leg fidgeting (intensity)
int check3 = 1;// for checking repetition of the motion (the four ranges)
int repetition = 0; // for storing repetition
bool starting = false; // for the user to start the leg band by pressing the M5 atom
int startfromphone = 0; // to start the program from a phone or other devices

void setBuff(uint8_t Rdata, uint8_t Gdata, uint8_t Bdata) // function to change the color
{
  DisBuff[0] = 0x05;
  DisBuff[1] = 0x05;
  for (int i = 0; i < 25; i++)
  {
    DisBuff[2 + i * 3 + 0] = Rdata;
    DisBuff[2 + i * 3 + 1] = Gdata;
    DisBuff[2 + i * 3 + 2] = Bdata;
  }
}

WiFiServer server(80);

void setup() {

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");

  // for the functionalities using M5 atom
  M5.begin(true, false, true);
  delay(10);
  setBuff(0x00, 0x00, 0x00);
  M5.dis.displaybuff(DisBuff);

  if (M5.IMU.Init() != 0)
  {
    IMU6886Flag = false;
  }
  else
  {
    IMU6886Flag = true;
  }
}

uint8_t color = 0; // the state of the color

void loop() {

  if (IMU6886Flag == true)
  {
    // Serial.printf("yes");
    //start from phone mainly since it is a leg band
    if (M5.Btn.wasPressed() || startfromphone == 1) {
      starting = true;

    }

    long currentTime = millis();
    M5.IMU.getGyroData(&gyroX, &gyroY, &gyroZ); //get the orientation
    M5.IMU.getAttitude(&pitch, &roll);
    arc = atan2(pitch, roll) * r_rand + 180; // cakculate arc
    val = sqrt(pitch * pitch + roll * roll);
    if ((currentTime - previoustime > 100) && (currentTime - previoustime < 1100)) {
      arc1 = arc;
      gyroY1 = gyroY;
    }
    if (((currentTime - previoustime) > 1200) && ((currentTime - previoustime) < 2200)) {
      arc2 = arc;
      gyroY2 = gyroY;
    }

    if (((currentTime - previoustime) > 2300) && ((currentTime - previoustime) < 3200)) {
      arcdiff = abs(arc1 - arc2); // calculate the arc difference 180 highest for this
      gyroYdiff = abs(gyroY1 - gyroY2); // calculate the gyroY difference 120-140
      previoustime = currentTime;
    }

    //reset previous time
    if ((currentTime - previoustime) >= 3200) {
      previoustime = currentTime;
    }


    // for up and down (tapping)
    if ( (arcdiff >= 0) && (arcdiff < 25)) {
      check1 = 1;
      repetition--;
    }
    if ( (arcdiff >= 25) && (arcdiff < 70)) {
      check1 = 2;
      repetition++;
    }
    if ( (arcdiff >= 70) && (arcdiff < 130)) {
      check1 = 3;
      repetition++;
    }
    if ( (arcdiff >= 130) && (arcdiff < 360)) {
      check1 = 4;
      repetition++;
    }

    if (repetition < 0) {
      repetition = 0;
    }
    //  Serial.printf("%d \n", repetition);



    //     for approximate side to side leg motion
    if ( (gyroYdiff >= 0) && (gyroYdiff < 20)) {
      check2 = 1;
    }

    if ( (gyroYdiff >= 20) && (gyroYdiff < 50)) {
      check2 = 2;
    }

    if ( (gyroYdiff >= 50) && (gyroYdiff < 80)) {
      check2 = 3;
    }

    if ( (gyroYdiff >= 80) && (gyroYdiff < 180)) {
      check2 = 4;
    }

    //  Serial.printf("%d \r\n", check1);


    //Giving the user to start the program either from phone or by pressing the M5 atom screen
    if ((starting == true) || (startfromphone == 1)) {
      //for arc values(tapping) - the first two rows from top of M5 atom
      if ( (check1 == 1)) // green
      {
        M5.dis.drawpix(0, 0xf00800);
        M5.dis.drawpix(1, 0xf00800);
        M5.dis.drawpix(2, 0xf00800);
        M5.dis.drawpix(3, 0xf00800);
        M5.dis.drawpix(4, 0xf00800);
        M5.dis.drawpix(5, 0xf00800);
        M5.dis.drawpix(6, 0xf00800);
        M5.dis.drawpix(7, 0xf00800);
        M5.dis.drawpix(8, 0xf00800);
        M5.dis.drawpix(9, 0xf00800);
      }

      if (check1 == 2) // white
      {
        M5.dis.drawpix(0, 0xffffff);
        M5.dis.drawpix(1, 0xffffff);
        M5.dis.drawpix(2, 0xffffff);
        M5.dis.drawpix(3, 0xffffff);
        M5.dis.drawpix(4, 0xffffff);
        M5.dis.drawpix(5, 0xffffff);
        M5.dis.drawpix(6, 0xffffff);
        M5.dis.drawpix(7, 0xffffff);
        M5.dis.drawpix(8, 0xffffff);
        M5.dis.drawpix(9, 0xffffff);
      }

      if (check1 == 3) // yellow
      {
        M5.dis.drawpix(0, 0xffff00);
        M5.dis.drawpix(1, 0xffff00);
        M5.dis.drawpix(2, 0xffff00);
        M5.dis.drawpix(3, 0xffff00);
        M5.dis.drawpix(4, 0xffff00);
        M5.dis.drawpix(5, 0xffff00);
        M5.dis.drawpix(6, 0xffff00);
        M5.dis.drawpix(7, 0xffff00);
        M5.dis.drawpix(8, 0xffff00);
        M5.dis.drawpix(9, 0xffff00);
      }


      if (check1 == 4) // red
      {
        M5.dis.drawpix(0, 0x00f000);
        M5.dis.drawpix(1, 0x00f000);
        M5.dis.drawpix(2, 0x00f000);
        M5.dis.drawpix(3, 0x00f000);
        M5.dis.drawpix(4, 0x00f000);
        M5.dis.drawpix(5, 0x00f000);
        M5.dis.drawpix(6, 0x00f000);
        M5.dis.drawpix(7, 0x00f000);
        M5.dis.drawpix(8, 0x00f000);
        M5.dis.drawpix(9, 0x00f000);
      }

      // forgyroY(side to side) takes the second two rows of the M5 atom from top
      if ( (check2 == 1)) // green
      {
        M5.dis.drawpix(10, 0xf00800);
        M5.dis.drawpix(11, 0xf00800);
        M5.dis.drawpix(12, 0xf00800);
        M5.dis.drawpix(13, 0xf00800);
        M5.dis.drawpix(14, 0xf00800);
        M5.dis.drawpix(15, 0xf00800);
        M5.dis.drawpix(16, 0xf00800);
        M5.dis.drawpix(17, 0xf00800);
        M5.dis.drawpix(18, 0xf00800);
        M5.dis.drawpix(19, 0xf00800);
      }

      if (check2 == 2) // white
      {
        M5.dis.drawpix(10, 0xffffff);
        M5.dis.drawpix(11, 0xffffff);
        M5.dis.drawpix(12, 0xffffff);
        M5.dis.drawpix(13, 0xffffff);
        M5.dis.drawpix(14, 0xffffff);
        M5.dis.drawpix(15, 0xffffff);
        M5.dis.drawpix(16, 0xffffff);
        M5.dis.drawpix(17, 0xffffff);
        M5.dis.drawpix(18, 0xffffff);
        M5.dis.drawpix(19, 0xffffff);
      }

      if (check2 == 3) // yellow
      {
        M5.dis.drawpix(10, 0xffff00);
        M5.dis.drawpix(11, 0xffff00);
        M5.dis.drawpix(12, 0xffff00);
        M5.dis.drawpix(13, 0xffff00);
        M5.dis.drawpix(14, 0xffff00);
        M5.dis.drawpix(15, 0xffff00);
        M5.dis.drawpix(16, 0xffff00);
        M5.dis.drawpix(17, 0xffff00);
        M5.dis.drawpix(18, 0xffff00);
        M5.dis.drawpix(19, 0xffff00);
      }


      if (check2 == 4) // red
      {
        M5.dis.drawpix(10, 0x00f000);
        M5.dis.drawpix(11, 0x00f000);
        M5.dis.drawpix(12, 0x00f000);
        M5.dis.drawpix(13, 0x00f000);
        M5.dis.drawpix(14, 0x00f000);
        M5.dis.drawpix(15, 0x00f000);
        M5.dis.drawpix(16, 0x00f000);
        M5.dis.drawpix(17, 0x00f000);
        M5.dis.drawpix(18, 0x00f000);
        M5.dis.drawpix(19, 0x00f000);
      }

      // check for repitition of the motion - takes the bottom row
      if ((repetition >= 0) && (repetition < 200))// green
      {
        check3 = 1;
        M5.dis.drawpix(20, 0xf00800);
        M5.dis.drawpix(21, 0xf00800);
        M5.dis.drawpix(22, 0xf00800);
        M5.dis.drawpix(23, 0xf00800);
        M5.dis.drawpix(24, 0xf00800);
      }
      if ((repetition >= 200) && (repetition < 500)) //white
      {
        check3 = 2;
        M5.dis.drawpix(20, 0xffffff);
        M5.dis.drawpix(21, 0xffffff);
        M5.dis.drawpix(22, 0xffffff);
        M5.dis.drawpix(23, 0xffffff);
        M5.dis.drawpix(24, 0xffffff);
      }
      if ((repetition >= 500) && (repetition < 1000)) // yellow
      {
        check3 = 3;
        M5.dis.drawpix(20, 0xffff00);
        M5.dis.drawpix(21, 0xffff00);
        M5.dis.drawpix(22, 0xffff00);
        M5.dis.drawpix(23, 0xffff00);
        M5.dis.drawpix(24, 0xffff00);
      }
      if ((repetition >= 1000)) // red
      {
        check3 = 4;
        M5.dis.drawpix(20, 0x00f000);
        M5.dis.drawpix(21, 0x00f000);
        M5.dis.drawpix(22, 0x00f000);
        M5.dis.drawpix(23, 0x00f000);
        M5.dis.drawpix(24, 0x00f000);
      }


    }
  }


  else
  {
    // Fail to initialize IMU (Maybe)
    M5.dis.clear();
    M5.dis.drawpix(2, 2, CRGB::Red);
    delay(500);
    M5.update();
  }
  M5.update();


  //code for hotspot connection and controlling and watching the change in fidgeting from phone or other devices

  WiFiClient client = server.available();   // listen for incoming clients
  //Serial.printf("1 \n ");
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {
      // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        //Serial.printf("3 \n");
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        //        if (currentLine.endsWith("GET /H")) {
        //          //INTERESTING PART 2
        //          Serial.println("I AM HIGH");
        //          client.println("I AM HIGH");
        //        }
        //        if (currentLine.endsWith("GET /L")) {
        //          //INTERESTING PART 3
        //          Serial.println("I AM LOW");
        //          client.println("I AM LOW");
        //        }

        if (currentLine.endsWith("GET /home")) {
          Serial.println("Home");

          // html and javascript code sent to the browser


          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<head>");

          client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
          client.println(".b{ background-color: #4CAF50; border: none; color: white; padding: 16px 40px; height:200px;width:400px;");
          client.println("text-decoration: none; font-size: 60px; margin: 2px; cursor: pointer;} </style>");
          client.println("<style type=\"text/css\">p{marigin: 15px 0;}</style>");
          // java script start
          client.println("<script type=\"text/javascript\">");
          client.println("let status =1;");
          // function to control the buttons
          client.println("function off(){");
          // If the state was turned on on button click then turn it off
          client.println("if (document.getElementById('B').innerHTML == \"Turn Off\") { ");
          client.println("document.getElementById('B').innerHTML = \"Turn On\"; ");
          client.println("status =1;");
          // remove the text and values
          client.println("document.getElementById('Z').remove();");
          client.println("document.getElementById(\"val\").remove();");
          client.println("document.getElementById('Y').remove();");
          client.println("document.getElementById(\"val1\").remove();");
          client.println("document.getElementById('X').remove();");
          client.println("document.getElementById(\"val2\").remove();}");

          // If the state was turned off on button click turn it on
          client.println("else {");
          //create elements and add them to the html document
          client.println("document.getElementById('B').innerHTML= \"Turn Off\";");
          client.println("status =0;");
          // create header files and append to html for fidgeting up down (tapping)
          client.println("let h2 = document.createElement('p');");
          client.println("let txt = document.createTextNode(\"Fidgeting -Up/Down:\");");
          client.println("h2.setAttribute('id','Z');");
          client.println("h2.setAttribute('style', \"font-size: 66px; display: inline; color: gray\");");
          client.println("h2.appendChild(txt);");
          client.println("document.getElementById('div1').appendChild(h2);");
          client.println("let h1 = document.createElement('h2');");
          client.println("h1.setAttribute('id',\"val\");");
          client.println("h1.setAttribute('style', \"font-size: 60px; display: inline; color:maroon\");");
          client.println("document.getElementById('div1').appendChild(h1);");
          // create header files and append to html for fidgeting sideways
          client.println("let h3 = document.createElement('p');");
          client.println("let txt2 = document.createTextNode(\"Fidgeting sideways:\");");
          client.println("h3.setAttribute('id','Y')");
          client.println("h3.setAttribute('style', \"font-size: 66px; display: inline; color: gray\");");
          client.println("h3.appendChild(txt2);");
          client.println("document.getElementById('div2').appendChild(h3);");
          client.println("let h4 = document.createElement('h2');");
          client.println("h4.setAttribute('id',\"val1\");");
          client.println("h4.setAttribute('style', \"font-size: 60px; display: inline; color:maroon\");");
          client.println("document.getElementById('div2').appendChild(h4);");

          // create header files and append to html for repetition
          client.println("let h5 = document.createElement('p');");
          client.println("let txt3 = document.createTextNode(\"Fidgeting Repetition:\");");
          client.println("h5.setAttribute('id','X')");
          client.println("h5.setAttribute('style', \"font-size: 66px; display: inline; color: gray\");");
          client.println("h5.appendChild(txt3);");
          client.println("document.getElementById('div3').appendChild(h5);");
          client.println("let h6 = document.createElement('h2');");
          client.println("h6.setAttribute('id',\"val2\");");
          client.println("h6.setAttribute('style', \"font-size: 60px; display: inline; color:maroon\");");
          client.println("document.getElementById('div3').appendChild(h6);}}");



          client.println("window.onload = function() {");
          client.println("if (status==1)");
          client.println("httpGetAsync(\"/stateA\");");
          client.println("else  httpGetAsync(\"/stateB\");");
          client.println("function httpGetAsync(theUrl) { ");
          client.println("var xmlHttp = new XMLHttpRequest();");
          client.println("var frequency_val = 1000;");
          client.println("xmlHttp.onreadystatechange = function() { ");
          client.println("if (xmlHttp.readyState == 4 && xmlHttp.status == 200){");
          client.println("let x =xmlHttp.responseText;");
          //Dynamically changing the values for the three fidgeting values
          client.println("document.getElementById(\"val\").innerHTML = x[0]; ");
          client.println("document.getElementById(\"val1\").innerHTML = x[1]; ");
          client.println("document.getElementById(\"val2\").innerHTML = x[2]; }}");

          client.println("xmlHttp.open(\"GET\", theUrl, true); ");
          client.println("xmlHttp.send(null);");
          //client.println("xmlHttp.timeout = 1000;}}");
          client.println("if (status ==1)");
          client.println("setTimeout(function(){httpGetAsync(\"/stateB\")}, frequency_val);");
          client.println("else setTimeout(function(){httpGetAsync(\"/stateA\")}, frequency_val); } }");
          client.println("</script>");



          client.println("</head>");
          client.println("<body><h1 style= \"font-size:100px;\">Fidget Tracker</h1>");
          client.println("<p><a><button class=\"b\" id='B' onclick=off()>Turn On</button></a></p>");
          // Create three div tags to store the three fidgeting values separately
          client.println("<p></p>");
          client.println("<p id = 'div1'></p>");
          client.println("<p id = 'div2'></p>");
          client.println("<p id = 'div3'></p>");
          client.println("<p></p>");
          client.println("<p></p>");
          client.println("<p style=\"font-size:30px;font-family: Helvetica;  \">If your fidgeting values are 3 and above try to do breathing exercises to help you calm down. </p>");
          client.println("<p style=\"font-size:30px;font-family: Helvetica;  \">Keep calm !</p>");
          client.println("</body>");
          client.println("</html>");
        }

        if (currentLine.endsWith("GET /stateA")) {

          client.println(String(check1) + String(check2) + String(check3));
          startfromphone = 1;
          Serial.print("ONNNNNN");
        }
        else if (currentLine.endsWith("GET /stateB")) {
          client.println(String(check1) + String(check2) + String(check3));
          startfromphone = 0;
          starting = false;
          setBuff(0x00, 0x00, 0x00);
          M5.dis.displaybuff(DisBuff);
          Serial.print("OFFFFFF");
        }
      }
    }
    //   close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
