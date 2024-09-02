#include <SoftwareSerial.h>

SoftwareSerial SIM900A(10, 11);  // RX | TX

int buzzer = 13;
int gasA0 = A0;

void setup() {
  Serial.begin(9600);
  Serial.println("Arduino with SIM900A is ready");

  SIM900A.begin(9600);
  Serial.println("SIM900A started at 9600");

  pinMode(buzzer, OUTPUT);
}

void loop() {
  // Read gas sensor and calculate gas level
  int analogSensor = analogRead(gasA0);
  int gasLevel = (analogSensor - 50) / 10;

  Serial.print("Gas Level: ");
  Serial.println(gasLevel);  // Print gas level for monitoring

  // Check for gas leak and send SMS/sound alert
  if (gasLevel >= 5) {
    if (SendTextMessage("8619256384", "DANGER!!! GAS LEAK DETECTED!")) {  // Replace with your phone number
      tone(buzzer, 1000, 100);  // Shorter buzzer duration
    } else {
      Serial.println("Error sending SMS!");
    }
  } else {
    noTone(buzzer);
  }

  // Reduce loop delay for faster readings
  delay(250);
}

bool SendTextMessage(const char* phoneNum, const char* message) {
  SIM900A.println("AT+CMGF=1");  // Set SMS mode
  if (!checkSIM900AResponse("OK")) {
    Serial.println("Error: Setting SMS mode failed!");
    return false;
  }

  SIM900A.print("AT+CMGS=\"");
  SIM900A.print(phoneNum);
  SIM900A.println("\"\r");  // Set recipient
  if (!checkSIM900AResponse(">")) {
    Serial.println("Error: Setting recipient failed!");
    return false;
  }

  SIM900A.println(message);
  delay(50);                  // Short delay before sending Control Z
  SIM900A.println((char)26);  // Send Control Z

  if (!checkSIM900AResponse("OK")) {
    Serial.println("Error: Sending SMS failed!");
    return false;
  }

  Serial.println("SMS Sent Successfully!");
  return true;
}

bool checkSIM900AResponse(const char* expectedResponse) {
  unsigned long startTime = millis();
  while (millis() - startTime < 5000) {  // Wait for response with a timeout of 5 seconds
    if (SIM900A.available()) {
      String response = SIM900A.readStringUntil('\n');
      Serial.print("Response: ");
      Serial.println(response);
      if (response.indexOf(expectedResponse) != -1) {
        return true;
      }
    }
  }
  return false;
}
