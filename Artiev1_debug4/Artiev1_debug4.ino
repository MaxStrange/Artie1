int pwm_a = 3;
int pwm_b = 11;
int dir_a = 12;
int dir_b = 13;

void setup()
{
  pinMode(pwm_a, OUTPUT); //Set control pins to be outputs
  pinMode(pwm_b, OUTPUT);
  pinMode(dir_a, OUTPUT);
  pinMode(dir_b, OUTPUT);
  
  analogWrite(pwm_a, 100);
  //set both motors to run at (100/255 = 39% of duty cycle)
  analogWrite(pwm_b, 100);
}

void loop()
{
  analogWrite(pwm_a, 200);
  analogWrite(pwm_b, 200); 
  digitalWrite(dir_a, HIGH);
  digitalWrite(dir_b, LOW);

  unsigned int initialTime = millis();
  while(millis() < (initialTime + 1000))
  {
    //delay
  }
  
  analogWrite(pwm_a, 0);
  analogWrite(pwm_b, 0);
  
  initialTime = millis();
  while(millis() < (initialTime + 1000))
  {
    //delay
  }
  
  delay(1000);
}
