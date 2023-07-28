/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <Arduino.h>
#include <stdint.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "lidar-ea564-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "eUWiG09rhhfQN1edqidjXHpsMCSKHIUkpzIABzKN"
#define WIFI_SSID     "tún"
#define WIFI_PASSWORD "14082000"
#define SAFE_RADIUS 30

/*******************************************************************************
* Funcitions prototype
*******************************************************************************/
void set_speed_percent(int percent);
void car_stop();
void car_forward();
void car_back();
void car_go_rect();
void car_right();
void car_forward_left();
void car_forward_right();

void car_left();
int calc_TurnTime(float angle);

uint8_t command;
int fwd_left = D8;           //Forward motion of Left motor 5 -> 15
int rev_left = D7;           //Reverse motion of Left motor 4 ->13
int fwd_right = D6;          //Forward motion of Right motor 3->12
int rev_right = D5;          //Reverse motion of Right motor 2-> 14
int led_Pin = D4;            // Khai báo chân điều khiển đèn led 12 -> 2
int speed_Pin = 0;
int speed_Percent;
int turning_Time = 0;
int forward_Time = 0;
float run_Speed = 0;
String input_string = "STOP";
String rx_command;
FirebaseData firebaseData;

void change_string(String& string) {
    if(string == "hello") {
        string = "alo";
    } else if (string == "alo") {
        string = "hello";
    }    
}

/*****************************************************************************
* Setup Funcition
*****************************************************************************/
void setup() {
    Serial.begin(9600);
    
    car_stop();                     /* initialize with motors stopped */
    delay(100);
    pinMode(led_Pin, OUTPUT);
    pinMode(speed_Pin, OUTPUT);
    pinMode(fwd_left, OUTPUT);      /* set Motor pins as output */
    pinMode(rev_left, OUTPUT);
    pinMode(fwd_right, OUTPUT);
    pinMode(rev_right, OUTPUT);

    analogWrite(speed_Pin, 1023);
    digitalWrite(led_Pin, HIGH);
    
    /* connect to wifi */
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("connecting");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());
    
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.setString(firebaseData,"Command", input_string);
    Firebase.setInt(firebaseData,"ForwardTime", 1000);
}

/*****************************************************************************
* Loop Funcition
*****************************************************************************/
void loop() {
    Firebase.getString(firebaseData, "Command");
    rx_command = firebaseData.stringData();
    Firebase.getInt(firebaseData, "ForwardTime");
    forward_Time = firebaseData.intData();

    if(rx_command != "0") {        
        if (rx_command == "w") {
            car_forward();
            delay(forward_Time);
            car_stop();
        }
        else if (rx_command == "s") {
            car_back();
            delay(forward_Time);
            car_stop();
        } 
        else if (rx_command == "RECTANGLE") {
            car_go_rect();
        }
        else if (rx_command == "LEFT") {
            car_left();
            delay(500);
            car_stop();
        }
        else if (rx_command == "RIGHT") {
            car_right();
            delay(500);
            car_stop();
        }
        else if (rx_command == "a") {
            car_forward_left();
            delay(500);
            car_stop();
        }
        else if (rx_command == "d") {
            car_forward_right();
            delay(500);
            car_stop();
        }       
    }
}

/*****************************************************************************
* Funcitions
*****************************************************************************/
int calc_TurnTime(float angle) {
  return angle * turning_Time / (2 * PI);
}

void set_speed_percent(int percent) {
    analogWrite(speed_Pin, percent * 1023 / 100);
}

void car_go_rect() {
    for (int i = 0; i < 4; i++) {
        car_forward();
        delay(forward_Time);
        car_stop();
        delay(1000);
        car_forward_right();
        delay(turning_Time / 4);
        car_forward_right();
        delay(1000);
    }
}

void car_forward() {
    digitalWrite(rev_right, LOW); // move forward
    digitalWrite(rev_left, LOW);
    digitalWrite(fwd_right, HIGH);
    digitalWrite(fwd_left, HIGH);
}

void car_back() {
    digitalWrite(rev_right, HIGH); //movebackward
    digitalWrite(rev_left, HIGH);
    digitalWrite(fwd_right, LOW);
    digitalWrite(fwd_left, LOW);
}

void car_left() {
    digitalWrite(rev_right, LOW);
    digitalWrite(fwd_right, HIGH);
    digitalWrite(rev_left, HIGH);
    digitalWrite(fwd_left, LOW);
}

void car_right() {
    digitalWrite(rev_right, HIGH);
    digitalWrite(fwd_right, LOW);
    digitalWrite(rev_left, LOW);
    digitalWrite(fwd_left, HIGH);
}

void car_forward_left() {
    digitalWrite(rev_right, LOW);
    digitalWrite(fwd_right, HIGH);
    digitalWrite(rev_left, LOW);
    digitalWrite(fwd_left, LOW);
}

void car_forward_right() {
    digitalWrite(rev_right, LOW);
    digitalWrite(fwd_right, LOW);
    digitalWrite(rev_left, LOW);
    digitalWrite(fwd_left, HIGH);
}

void car_back_left() {
    digitalWrite(rev_right, HIGH);
    digitalWrite(fwd_right, LOW);
    digitalWrite(rev_left, LOW);
    digitalWrite(fwd_left, LOW);
}

void car_back_right() {
    digitalWrite(rev_right, LOW);
    digitalWrite(fwd_right, LOW);
    digitalWrite(rev_left, HIGH);
    digitalWrite(fwd_left, LOW);
}

void car_stop() {
    digitalWrite(led_Pin, 0);
    digitalWrite(rev_right, LOW); //Stop
    digitalWrite(fwd_right, LOW);
    digitalWrite(rev_left, LOW);
    digitalWrite(fwd_left, LOW);
}


/******************************************************************************
* End of file
******************************************************************************/
