typedef enum {
  TURN_TIGHT_LEFT = 'A',
  TURN_TIGHT_RIGHT = 'B',
  TURN_WIDE_LEFT = 'C',
  TURN_WIDE_RIGHT = 'D',
  DRIVE_FORWARDS = 'E',
  DRIVE_BACKWARDS = 'F',
  SEND_ID = 'G',
  STOP_DRIVING = 'I',
  GET_CALIBRATION_DATA = 'J',
  TEST_CONNECTION = 't',
} ActionChar;

const char END_OF_ACTION = 'P';

const unsigned int PWM_A = 3;
const unsigned int PWM_B = 11;
const unsigned int DIR_A = 12;
const unsigned int DIR_B = 13;
const unsigned int MAGNET_PIN = 2;

const unsigned int DUTY_CYCLE_RIGHT = 100;
const unsigned int DUTY_CYCLE_LEFT = 125;
const unsigned int ACTION_LENGTH = 2000;

boolean loopFlag = true;

volatile unsigned int distance_counted_by_interrupt = 0;
unsigned int distanceForLeftWheel = 0;


void setup()
{
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(MAGNET_PIN, INPUT);
  
  attachInterrupt(0, incrementDistance, FALLING);
  
  stopDriving();
  
  Serial.begin(9600);
}

void loop()
{
  while (Serial.available())
  {
    readSerial();
  }
  //if there is no serial data available, tell the mainframe
  //that it is ready
  if (loopFlag)
  {
    Serial.write(END_OF_ACTION);
    loopFlag = false;
  }
}

void incrementDistance()
{
  distance_counted_by_interrupt++;
}

void readSerial()
{
   unsigned int inChar = Serial.read();
  
  switch (inChar) {
    case TURN_TIGHT_LEFT:
              turnTightLeft();
              break;
    case TURN_TIGHT_RIGHT: 
              turnTightRight();
              break;
    case TURN_WIDE_LEFT: 
              turnWideLeft();
              break;
    case TURN_WIDE_RIGHT: 
              turnWideRight();
              break;
    case DRIVE_FORWARDS: 
              driveForwards();
              break;
    case DRIVE_BACKWARDS: 
              driveBackwards();
              break;
    case GET_CALIBRATION_DATA:
              calibrate();
              break;
    case TEST_CONNECTION: 
              testConnection();
              break;
    case SEND_ID: 
              sendID();
              break;
    case STOP_DRIVING: 
              stopDriving();
              break;
    default:  break;
  }
}

void calibrate()
{
  Serial.write(GET_CALIBRATION_DATA);
  driveForwards();
}

void sendID()
{
  Serial.write("x");
}

void testConnection()
{
  Serial.write("X");
}

void driveBackwards()
{
  analogWrite(PWM_A, DUTY_CYCLE_LEFT);
  analogWrite(PWM_B, DUTY_CYCLE_RIGHT);
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  
  delayAndRecordDistanceValues(DRIVE_BACKWARDS);
  stopDriving();
  
  loopFlag = true;
}

void driveForwards()
{
  analogWrite(PWM_A, DUTY_CYCLE_LEFT);
  analogWrite(PWM_B, DUTY_CYCLE_RIGHT);
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  
  delayAndRecordDistanceValues(DRIVE_FORWARDS);
  stopDriving();
  
  loopFlag = true;
}

void turnTightLeft()
{
  analogWrite(PWM_A, DUTY_CYCLE_LEFT);
  analogWrite(PWM_B, DUTY_CYCLE_RIGHT);
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  
  delayAndRecordDistanceValues(TURN_TIGHT_LEFT);
  stopDriving();
  
  loopFlag = true;
}

void turnTightRight()
{
  analogWrite(PWM_A, DUTY_CYCLE_LEFT);
  analogWrite(PWM_B, DUTY_CYCLE_RIGHT);
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  
  delayAndRecordDistanceValues(TURN_TIGHT_RIGHT);
  stopDriving();
  
  loopFlag = true;
}

void turnWideLeft()
{
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, DUTY_CYCLE_RIGHT);
  digitalWrite(DIR_B, LOW);

  delayAndRecordDistanceValues(TURN_WIDE_LEFT);
  stopDriving();
  
  loopFlag = true;
}

void turnWideRight()
{
  analogWrite(PWM_A, DUTY_CYCLE_LEFT);
  analogWrite(PWM_B, 0);
  digitalWrite(DIR_A, HIGH);
  
  delayAndRecordDistanceValues(TURN_WIDE_RIGHT);
  stopDriving();
  
  loopFlag = true;
}

void stopDriving()
{
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

void delayAndRecordDistanceValues(char actionIdentifierToSendToMainframe)
{
  delay(ACTION_LENGTH);//Sensors are counting the distance via interrupts
  sendAccordingToProtocol(actionIdentifierToSendToMainframe);
}

void sendAccordingToProtocol(char actionIdentifier)
{
  Serial.write(actionIdentifier);
  sendAndResetDistance(actionIdentifier);
}

void sendAndResetDistance(char actionIdentifierToSendToMainframe)
{
  distanceForLeftWheel = distance_counted_by_interrupt;
  if (actionIdentifierToSendToMainframe == TURN_WIDE_RIGHT)
  {
    Serial.write(distanceForLeftWheel);
  }
  else
  {
    Serial.write(distance_counted_by_interrupt);
  }
  distance_counted_by_interrupt = 0;
}
