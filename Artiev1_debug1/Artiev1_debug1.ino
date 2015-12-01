int inChar;

int pwm_a = 3;   //PWM control for motor outputs 1 and 2 is on digital pin 3
int pwm_b = 11;  //PWM control for motor outputs 3 and 4 is on digital pin 11
int dir_a = 12;  //dir control for motor outputs 1 and 2 is on digital pin 12
int dir_b = 13;  //dir control for motor outputs 3 and 4 is on digital pin 13
const int ACTION_LENGTH = 2000; //duration of actions
const int dutyCycle = 100; //motor power out of 255

boolean loopFlag = true;

void setup()
{
  pinMode(pwm_a, OUTPUT); //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
    
  Serial.begin(9600);
}

void loop()
{
  if (loopFlag)
  {
    while (Serial.available())
    {
      readSerial();
//      delay(actionLength);
      loopFlag = false;
    }
  }
  else
  {
    stopDriving();
    delay(100);
    if (Serial.available())
    {
      loopFlag = true;
    }
  }
}

void readSerial()
{
  inChar = Serial.read();
  
  switch (inChar) {
    case 'A': turnTightLeft();
              break;
    case 'B': turnTightRight();
              break;
    case 'C': turnWideLeft();
              break;
    case 'D': turnWideRight();
              break;
    case 'E': driveForwards();
              break;
    case 'F': driveBackwards();
              break;
    case 't': testConnection();
              break;
    case 'G': sendID();
              break;
    case 'I': stopDriving();
              break;
    case 'J': calibrate();
              break;
    default:  break;
  }
}

void calibrate()
{
  Serial.write('J');
  driveForwards();
}

void delayAndRecordDistanceValues(char actionID)
{
  unsigned int initialTime = millis();
  while (millis() < (initialTime + ACTION_LENGTH))
  {
    //delay
  }
  Serial.write(actionID);
  Serial.write(2);
}

void driveBackwards()
{
  analogWrite(pwm_a, dutyCycle);
  analogWrite(pwm_b, dutyCycle); 
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, HIGH);
  
  delayAndRecordDistanceValues('F');
}

void driveForwards()
{
  analogWrite(pwm_a, dutyCycle);
  analogWrite(pwm_b, dutyCycle); 
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, LOW);
  
  delayAndRecordDistanceValues('E');
}

void turnTightLeft()
{
  analogWrite(pwm_a, dutyCycle);
  analogWrite(pwm_b, dutyCycle);  
  digitalWrite(dir_a, LOW);
  digitalWrite(dir_b, LOW);
  
  delayAndRecordDistanceValues('A');
}

void turnTightRight()
{
  analogWrite(pwm_a, dutyCycle);
  analogWrite(pwm_b, dutyCycle);
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, HIGH);
  
  delayAndRecordDistanceValues('B');
}

void turnWideLeft()
{
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, dutyCycle);
  digitalWrite(dir_b, LOW);
  
  delayAndRecordDistanceValues('C');
}

void turnWideRight()
{ 
  analogWrite(pwm_a, dutyCycle);
  analogWrite(pwm_b, 0);
  digitalWrite(dir_a, HIGH);
  
  delayAndRecordDistanceValues('D');
}

void sendID()
{
  Serial.write("x"); 
}

void stopDriving()
{
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
}

void testConnection()
{
  Serial.write("X");
}
