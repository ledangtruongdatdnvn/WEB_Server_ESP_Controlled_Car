//http://arduino.esp8266.com/stable/package_esp8266com_index.json
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

//Wifi
const char* wifiName = "Leo";
const char* password = "0384167161";

/*----------------------------------------*/
//  -- Motor --
// Right motor
const int rightMotorPin1 = D1;
const int rightMotorPin2 = D2;
// Left motor
const int leftMotorPin1 = D3;
const int leftMotorPin2 = D4;
// Enable motor
const int enableRightMotor = D0;
const int enableLeftMotor = D5;
/*----------------------------------------*/

/*----------------------------------------*/
// Sử dụng relay
const int relay = D7;
/*----------------------------------------*/

/*----------------------------------------*/
#define flameSensorPinAnalog A0
#define flameSensorPinDigital D6
boolean fire = false;
int flameAnalogValue;
int flameDigitalValue;
/*----------------------------------------*/

//Speed Of Motor
String sliderValue = "0";
const char* inputParameter = "value";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create an Event Source on /events
AsyncEventSource events("/events");

// Timer variables
unsigned long lastTime = 0;  
unsigned long timerDelay = 500;

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Controlled Car</title>
  <style>
    html {
      font-family: Arial, Helvetica, sans-serif;
      display: inline-block;
      text-align: center;
    }

    body {
      background-color: rgb(255, 219, 174);
      max-width: max-content;
      margin: 0px auto;
      padding-bottom: 25px;
    }

    .title {
      font-size: 50px;
      text-align: center;
    }

    p {
      font-size: 30px;
    }

    .slider {
      -webkit-appearance: none;
      margin: 10px;
      width: 250px;
      height: 30px;
      background: #ffb5b5;
      outline: none;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }

    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 35px;
      height: 35px;
      background: #8ddbff;
      cursor: pointer;
    }

    .slider::-moz-range-thumb {
      width: 35px;
      height: 35px;
      background: #01070a;
      cursor: pointer;
    }

    .control-buttons {
      margin: auto;
      width: max-content;
      border: 3px solid rgb(24, 128, 255);
      padding: 10px;
      border-radius: 20px;
    }

    .control-buttons-top {
      text-align: center;
    }

    .button-top {
      background-color: #8ddbff;
      height: 50px;
      width: 70px;
      border-radius: 10px;

    }

    .button-top:hover {
      box-shadow: 2px 2px 2px #7dd6ff;
    }

    .button-top:active {
      opacity: 0.8;
    }

    .control-buttons-mid {
      text-align: center;
    }

    .button-mid-left {
      background-color: #8ddbff;
      height: 50px;
      width: 70px;
      border-radius: 10px;
    }

    .button-mid-left:hover {
      box-shadow: 2px 2px 2px #7dd6ff;
    }

    .button-mid-left:active {
      opacity: 0.8;
    }

    .button-mid-stop {
      background-color: #8ddbff;
      height: 50px;
      width: 70px;
      border-radius: 10px;
    }

    .button-mid-stop:hover {
      box-shadow: 2px 2px 2px #7dd6ff;
    }

    .button-mid-stop:active {
      opacity: 0.8;
    }

    .button-mid-right {
      background-color: #8ddbff;
      height: 50px;
      width: 70px;
      border-radius: 10px;
    }

    .button-mid-right:hover {
      box-shadow: 2px 2px 2px #7dd6ff;
    }

    .button-mid-right:active {
      opacity: 0.8;
    }

    .control-buttons-bottom {
      text-align: center;
    }

    .button-bottom {
      background-color: #8ddbff;
      height: 50px;
      width: 70px;
      border-radius: 10px;
    }

    .button-bottom:hover {
      box-shadow: 2px 2px 2px #7dd6ff;
    }

    .button-bottom:active {
      opacity: 0.8;
    }

    .water-button {
      background-color: #8ddbff;
      height: 50px;
      width: 70px;
      border-radius: 10px;
    }

    .water-button:hover {
      box-shadow: 2px 2px 2px #7dd6ff;
    }

    .water-button:active {
      opacity: 0.8;
    }
    .water-button-stop {
      background-color: #8ddbff;
      height: 50px;
      width: 70px;
      border-radius: 10px;
    }

    .water-button-stop:hover {
      box-shadow: 2px 2px 2px #7dd6ff;
    }

    .water-button-stop:active {
      opacity: 0.8;
    }
  </style>
</head>

<body>
  <h2 class="title">WELCOME TO MY CONTROLLED CAR</h2>
  <p id="fire">Fire: %FIRE% </p>
  <button class="water-button" onclick="extinguishFire('yes')"><svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor"
      class="bi bi-water" viewBox="0 0 16 16">
      <path
        d="M.036 3.314a.5.5 0 0 1 .65-.278l1.757.703a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.757-.703a.5.5 0 1 1 .372.928l-1.758.703a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0L.314 3.964a.5.5 0 0 1-.278-.65zm0 3a.5.5 0 0 1 .65-.278l1.757.703a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.757-.703a.5.5 0 1 1 .372.928l-1.758.703a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0L.314 6.964a.5.5 0 0 1-.278-.65zm0 3a.5.5 0 0 1 .65-.278l1.757.703a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.757-.703a.5.5 0 1 1 .372.928l-1.758.703a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0L.314 9.964a.5.5 0 0 1-.278-.65zm0 3a.5.5 0 0 1 .65-.278l1.757.703a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.014-.406a2.5 2.5 0 0 1 1.857 0l1.015.406a1.5 1.5 0 0 0 1.114 0l1.757-.703a.5.5 0 1 1 .372.928l-1.758.703a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0l-1.014-.406a1.5 1.5 0 0 0-1.114 0l-1.015.406a2.5 2.5 0 0 1-1.857 0l-1.757-.703a.5.5 0 0 1-.278-.65z" />
    </svg></button>
  <button class="water-button-stop" onclick="extinguishFire('no')">
    <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-stop-circle" viewBox="0 0 16 16">
      <path d="M8 15A7 7 0 1 1 8 1a7 7 0 0 1 0 14zm0 1A8 8 0 1 0 8 0a8 8 0 0 0 0 16z"/>
      <path d="M5 6.5A1.5 1.5 0 0 1 6.5 5h3A1.5 1.5 0 0 1 11 6.5v3A1.5 1.5 0 0 1 9.5 11h-3A1.5 1.5 0 0 1 5 9.5v-3z"/>
    </svg>
  </button>
  <p><span id="textslider_value">Speed Of Controlled Car: %SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="255" value="%SLIDERVALUE%"
      step="1" class="slider"></p>
  <div class="control-buttons">
    <div class="control-buttons-top">
      <button class="button-top" onclick="run('up')">
        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-arrow-bar-up"
          viewBox="0 0 16 16">
          <path fill-rule="evenodd"
            d="M8 10a.5.5 0 0 0 .5-.5V3.707l2.146 2.147a.5.5 0 0 0 .708-.708l-3-3a.5.5 0 0 0-.708 0l-3 3a.5.5 0 1 0 .708.708L7.5 3.707V9.5a.5.5 0 0 0 .5.5zm-7 2.5a.5.5 0 0 1 .5-.5h13a.5.5 0 0 1 0 1h-13a.5.5 0 0 1-.5-.5z" />
        </svg>
      </button>
    </div>
    <br />
    <div class="control-buttons-mid">
      <button class="button-mid-left" onclick="run('left')">
        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-arrow-bar-left"
          viewBox="0 0 16 16">
          <path fill-rule="evenodd"
            d="M12.5 15a.5.5 0 0 1-.5-.5v-13a.5.5 0 0 1 1 0v13a.5.5 0 0 1-.5.5zM10 8a.5.5 0 0 1-.5.5H3.707l2.147 2.146a.5.5 0 0 1-.708.708l-3-3a.5.5 0 0 1 0-.708l3-3a.5.5 0 1 1 .708.708L3.707 7.5H9.5a.5.5 0 0 1 .5.5z" />
        </svg>
      </button>
       <button class="button-mid-stop" onclick="run('stop')">
         <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-stop"
          viewBox="0 0 16 16">
          <path
            d="M3.5 5A1.5 1.5 0 0 1 5 3.5h6A1.5 1.5 0 0 1 12.5 5v6a1.5 1.5 0 0 1-1.5 1.5H5A1.5 1.5 0 0 1 3.5 11V5zM5 4.5a.5.5 0 0 0-.5.5v6a.5.5 0 0 0 .5.5h6a.5.5 0 0 0 .5-.5V5a.5.5 0 0 0-.5-.5H5z" />
        </svg>
      </button>
      <button class="button-mid-right" onclick="run('right')">
        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-arrow-bar-right"
          viewBox="0 0 16 16">
          <path fill-rule="evenodd"
            d="M6 8a.5.5 0 0 0 .5.5h5.793l-2.147 2.146a.5.5 0 0 0 .708.708l3-3a.5.5 0 0 0 0-.708l-3-3a.5.5 0 0 0-.708.708L12.293 7.5H6.5A.5.5 0 0 0 6 8zm-2.5 7a.5.5 0 0 1-.5-.5v-13a.5.5 0 0 1 1 0v13a.5.5 0 0 1-.5.5z" />
        </svg>
      </button>
    </div>
    <br />
    <div class="control-buttons-bottom">
      <button class="button-bottom" onclick="run('down')">
        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-arrow-bar-down"
          viewBox="0 0 16 16">
          <path fill-rule="evenodd"
            d="M1 3.5a.5.5 0 0 1 .5-.5h13a.5.5 0 0 1 0 1h-13a.5.5 0 0 1-.5-.5zM8 6a.5.5 0 0 1 .5.5v5.793l2.146-2.147a.5.5 0 0 1 .708.708l-3 3a.5.5 0 0 1-.708 0l-3-3a.5.5 0 0 1 .708-.708L7.5 12.293V6.5A.5.5 0 0 1 8 6z" />
        </svg>
      </button>
    </div>
  </div>
  <script>
    function updateSliderPWM(element) {
      var sliderValue = document.getElementById("pwmSlider").value;
      document.getElementById("textslider_value").innerHTML = "Speed Of Controlled Car: " + sliderValue;
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/slider?value=" + sliderValue, true);
      xhr.send();
    }
    function run(message) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/run?value="+message, true);
      xhr.send();
    }
    function extinguishFire(message){
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/extinguishFire?value="+message, true);
      xhr.send();
    }
    if (!!window.EventSource) {
      var source = new EventSource('/events');
      source.addEventListener('open', function(e) {
        console.log("Events Connected");
      }, false);
      source.addEventListener('error', function(e) {
        if (e.target.readyState != EventSource.OPEN) {
          console.log("Events Disconnected");
        }
      }, false);
     
     source.addEventListener('message', function(e) {
        console.log("message", e.data);
     }, false);
     
     source.addEventListener('fire', function(e) {
        console.log("fire", e.data);
        document.getElementById("fire").innerHTML = "Fire: " + e.data;
     }, false);
    }
  </script>
</body>

</html>
)rawliteral";
void getSensorData(){
  flameDigitalValue = digitalRead(flameSensorPinDigital);
  flameAnalogValue = analogRead(flameSensorPinAnalog);
  if(flameDigitalValue == 1){
    fire = true;
  } else if(flameDigitalValue == 0)  {
    fire = false;
  }
}
String processor(const String& var){
  getSensorData();
  if (var == "SLIDERVALUE"){
    return sliderValue;
  } else if (var == "FIRE"){
    return fire?"yes":"no";
  }
  return String();
}
void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);

  }
  else if (rightMotorSpeed >= 0)
  {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  }

  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
  }
  else if (leftMotorSpeed >= 0)
  {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
  }
  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));
  
}


void setup(){
  Serial.begin(115200);
  delay(1000);
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  
  analogWrite(enableRightMotor, 0);
  digitalWrite(rightMotorPin1, LOW);
  digitalWrite(rightMotorPin2, LOW);
  analogWrite(enableLeftMotor, 0);
  digitalWrite(leftMotorPin1, LOW);
  digitalWrite(leftMotorPin2, LOW);

   //khởi tạo relay
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  //khởi tạo cảm biến lửa
  pinMode(flameSensorPinAnalog, INPUT);
  pinMode(flameSensorPinDigital, INPUT);

  WiFi.begin(wifiName, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });


  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String message;
    if (request->hasParam(inputParameter)) {
      message = request->getParam(inputParameter)->value();
      sliderValue = message;
    }
    else {
      message = "No message sent";
    }
    Serial.println(message);
    request->send(200, "text/plain", "OK");
  });

  server.on("/extinguishFire", HTTP_GET, [](AsyncWebServerRequest *request){
     String message;
    if (request->hasParam(inputParameter)) {
      message = request->getParam(inputParameter)->value();
    }
    else {
      message = "No message sent";
    }
    if(message == "yes"){
      digitalWrite(relay, HIGH);
      request->send(200, "text/plain", "OK");
    }else{
      digitalWrite(relay, LOW);
      request->send(200, "text/plain", "OK");
    }

  });
    
  server.on("/run", HTTP_GET, [](AsyncWebServerRequest *request){
    String message;
    if(request->hasParam(inputParameter)){
      message = request->getParam(inputParameter)->value();
      if(message == "up"){
         rotateMotor(sliderValue.toInt(),sliderValue.toInt());
      }else if(message == "down"){
          rotateMotor(("-" + sliderValue).toInt(),("-" + sliderValue).toInt());
      }else if(message == "left"){
        rotateMotor(sliderValue.toInt(),0);   
      } else if(message == "right"){
         rotateMotor(0,sliderValue.toInt());
      }else{
         rotateMotor(0,0);
      }
    }
    request->send(200, "text/plain", "OK");
  });

  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it got is: %u\n", client->lastId());
    }
    // send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!", NULL, millis(), 10000);
  });
  server.addHandler(&events);
  server.begin();
}



void loop() {
  if ((millis() - lastTime) > timerDelay) {
    getSensorData();
    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    Serial.println(fire);
    events.send(String(fire?"yes":"no").c_str(),"fire",millis());
    lastTime = millis();
  }
}
