#include <Servo.h>

const byte numChars = 32;
char receivedChars[numChars];

int distance_x = 0;
int distance_y = 0;

boolean newData = false;

Servo fireServo;
Servo panServo;
Servo tiltServo;

// Angle of the servo when at rest
const byte fireRest = 180;
// Angle the servo need to reach to push the dart
const byte firePushed = 125;

bool isFiring = false;
bool canFire = false;
bool recoiling = false;

unsigned long firingStartTime = 0;
unsigned long firingCurrentTime = 0;
const long firingTime = 150;

unsigned long recoilStartTime = 0;
unsigned long recoilCurrentTime = 0;
const long recoilTime = 2 * firingTime;

const byte motorPin = 12;

byte panServoPosition = 90;
byte tiltServoPosition = 90;

boolean fKey = false;
boolean mKey = false;

void setup() {
  Serial.begin(115200);
  while (Serial.available() > 0) {
    Serial.read();
  }

  // pinMode(motorPin, OUTPUT);
  // digitalWrite(motorPin, LOW);

  panServo.write(panServoPosition);
  panServo.attach(9);

  tiltServo.write(tiltServoPosition);
  tiltServo.attach(10);

  fireServo.write(fireRest);
  fireServo.attach(11);
}

void loop() {
  recvData();
  if (newData) {
    moveServo();
    setRecoil();
    // setMotor();
    newData = false;
  }
  fire();
  // showNewData();
}

// Markers for a string containing face coordinates
const char startMarkerCoords = '<';
const char endMarkerCoords = '>';

char keybind;

void recvData() {
  static boolean recvInProgress = false;
  static byte ndx = 0;

  char rc;

  while (Serial.available() > 0 && !newData) {
    rc = Serial.read();

    if (recvInProgress) {
      if (rc != endMarkerCoords && rc != 'Y' && rc != 'f' && rc != 'm') {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';
        if (rc == 'm') {
          keybind = 'm';
          recvInProgress = false;
          newData = true;
        }
        if (rc == 'f') {
          keybind = 'f';
          recvInProgress = false;
          newData = true;
        }
        if (rc == 'Y') {
          distance_x = atoi(receivedChars);
        } else {
          distance_y = atoi(receivedChars);
          recvInProgress = false;
          newData = true;
        }
        ndx = 0;
      }
    } else if (rc == startMarkerCoords) {
      recvInProgress = true;
    }
  }
}

void showNewData() {
  if (newData) {
    Serial.println(String(distance_x) + " " + String(distance_y));
    newData = false;
  }
}

void moveServo() {
  if (distance_x >= 0) {
    if (distance_x > 30 && panServoPosition >= 2) {
      panServoPosition -= 2;
      panServo.write(panServoPosition);
    }
  } else {
    if (distance_x < -30 && panServoPosition <= 178) {
      panServoPosition += 2;
      panServo.write(panServoPosition);
    }
  }
  if (distance_y >= 0) {
    if (distance_y > 40 && tiltServoPosition <= 128) {
      tiltServoPosition += 2;
      tiltServo.write(tiltServoPosition);
    }
  } else {
    if (distance_y < -40 && tiltServoPosition >= 22) {
      tiltServoPosition -= 2;
      tiltServo.write(tiltServoPosition);
    }
  }
}

void setMotor() {
  if (keybind == 'm') {
    if (!mKey) {
      mKey = true;
      digitalWrite(motorPin, HIGH);
    } else {
      mKey = false;
      digitalWrite(motorPin, LOW);
    }
    keybind = '\0';
  }
}

void setRecoil() {
  if (keybind == 'f') {
    if (!fKey) {
      fKey = true;
    } else {
      fKey = false;
    }
  }
  keybind = '\0';
  if (!isFiring && !recoiling && distance_x <= 60 && distance_x >= -60 && distance_y <= 80 && distance_y >= -80 && fKey) {
    canFire = true;
  } else {
    canFire = false;
  }
}

void fire() {

  if (canFire && !isFiring) {
    firingStartTime = millis();
    recoilStartTime = millis();
    isFiring = true;
  }

  firingCurrentTime = millis();
  recoilCurrentTime = millis();

  if (isFiring && firingCurrentTime - firingStartTime < firingTime) {
    fireServo.write(firePushed);
  } else if (isFiring && recoilCurrentTime - recoilStartTime < recoilTime) {
    fireServo.write(fireRest);
  } else if (isFiring && recoilCurrentTime - recoilStartTime > recoilTime) {
    isFiring = false;
  }
}
