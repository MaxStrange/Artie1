int pwm_a = 3;   //PWM control for motor outputs 1 and 2 is on digital pin 3
int pwm_b = 11;  //PWM control for motor outputs 3 and 4 is on digital pin 11
int dir_a = 12;  //dir control for motor outputs 1 and 2 is on digital pin 12
int dir_b = 13;  //dir control for motor outputs 3 and 4 is on digital pin 13
const int ACTION_LENGTH = 2000; //duration of actions
const int dutyCycle = 100; //motor power out of 255

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
  driveForwards();
  delayArtie();
  stopDriving();
  delayArtie();
}

void delayArtie()
{
  unsigned int initialTime = millis();
  while(millis() < (initialTime + ACTION_LENGTH))
  {
    //delay
  }
}

void driveForwards()
{
  analogWrite(pwm_a, dutyCycle);
  analogWrite(pwm_b, dutyCycle); 
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, LOW);
}

void stopDriving()
{
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
}

