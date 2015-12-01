#include <ActionSequence.h>
#include <ActionSequenceStack.h>
#include <Frame.h>
#include <FrameQueue.h>
#include <FrameStack.h>
#include <Queue.h>
#include <QueueStack.h>
#include <Stack.h>

const unsigned int PWM_A = 3;
const unsigned int PWM_B = 11;
const unsigned int DIR_A = 12;
const unsigned int DIR_B = 13;

const unsigned int MAGNET_PIN = 2;

boolean loopFlag = false;

const float WHEEL_CIRC = 0.25;
const float AXEL_LENGTH = 0.17;
const unsigned int DUTY_CYCLE = 100;
const unsigned int ACTION_LENGTH = 2000;
const unsigned int DELAY_LENGTH = 10;

struct Frame currentFrame;
FrameQueue queue;//Queue of frames that are supposed to match currentFrame after each action
//QueueStack stack;//Stack of frameQueues - each frameQueue is a goal - the top one is the one that Artie is currently working on

volatile unsigned int distance_counted_by_interrupt = 0;

void setup()
{
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  
  pinMode(MAGNET_PIN, INPUT);
  
  currentFrame.x_pos = 0;
  currentFrame.y_pos = 0;
  currentFrame.theta_n = 0;
  
  attachInterrupt(0, incrementDistance, FALLING);
  
  stopDriving();
  
  Serial.begin(9600);
}

void loop()
{
  if (loopFlag)
  {
    while (Serial.available())
    {
      readSerial();
      delay(ACTION_LENGTH);
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

void incrementDistance()
{
  distance_counted_by_interrupt++;
}

void readSerial()
{
 unsigned int inChar = Serial.read();
  
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
    case 'U': doubleCheckArtiePosition();
      //make sure that the currentFrame matches where Artie is supposed to be.
      //If it doesn't, request a new frameQueue be put on the stack,
      //whose final frame is the goal - that is, it is a frame that is where Artie is supposed to be.
              break;
    case 'Z': receiveNewFrameSequence(inChar);
              break;
    default:  break;
  }
}

void doubleCheckArtiePosition()
{
//  if (stack.isEmpty())
//  {
//    return;
//  }
//  
//  while (stack.peek().isEmpty())
//  {
//    stack.pop();
//    if (stack.isEmpty())
//    {
//      return;
//    }
//  }
//
//  if (
//  (currentFrame.x_pos is sig dif from stack.peek().peek().x_pos)
//  ||
//  (currentFrame.y_pos is sig dif from stack.peek().peek().y_pos)
//  ||
//  (currentFrame.z_pos is sig dif from stack.peek().peek().z_pos)
//  ||
//  (currentFrame.theta_n is sig dif from stack.peek().peek().theta_n))
//  {
//    requestNewFrameSequence();
//  }
//  else
//  {
//    stack.peek().dequeue();
//  }
}

void requestNewFrameSequence()
{
//  Serial.println("give me a new sequence or whatever");
//  unsigned int inChar = Serial.read();
//  receiveNewFrameSequence(inChar);
}

void receiveNewFrameSequence(unsigned int inChar)
{
//  FrameQueue newFrameSequence;
//  struct Frame newFrame;
//  
//  while ((inChar == 'Z') || (inChar == 'Q'))
//  {
//    receiveAndCreateFrame(&newFrame);
//    newFrameSequence.enqueue(newFrame);
//    inChar = Serial.read();
//  }
//  
//  stack.push(newFrameSequence);
}
  
void receiveAndCreateFrame(struct Frame *newFrame)
{ 
//  unsigned int inChar = Serial.read();
//  
//  while (inChar != 'P')
//  {
//    switch (inChar) {
//      case 'X': //next data in buffer is a float which is newFrame.x_pos
//                  float x = Serial.parseFloat();
//                  newFrame->x_pos = x;
//                //if not, ask mainframe to resend frame's x_pos
//                break;
//      case 'Y': //next data in buffer is a float which is newFrame.y_pos
//                  float y = Serial.parseFloat();
//                  newFrame->y_pos = y;
//                //if not, ask the mainframe to resend frame's y_pos
//                break;
//      case 'T': //next data in buffer is a float which is the newFrame.theta_n
//                  float t = Serial.parseFloat();
//                  newFrame->theta_n = t;
//                //if not, ask the mainframe to resend frame's theta_n
//                break;
//      default: break;
//
//      inChar = Serial.read();
//    }
//  }
}

extern int __bss_end;
extern void *__brkval;

int memoryFree()
{
  int freeValue;
  
  if ((int)__brkval == 0)
    freeValue = ((int)&freeValue) - ((int)&__bss_end);
  else
    freeValue = ((int)&freeValue) - ((int)&__brkval);
  
  return freeValue;
}

void sendID()
{
  Serial.write("x");
}

void testConnection()
{
  Serial.write("Connected!");
}

void driveBackwards()
{
  analogWrite(PWM_A, DUTY_CYCLE);
  analogWrite(PWM_B, DUTY_CYCLE);
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  
  delayAndRecordDistanceValues();
  stopDriving();
  
  float distance;
  measureDistance(&distance);
  
  float dx = (-1) * distance * cos(currentFrame.theta_n);
  float dy = (-1) * distance * sin(currentFrame.theta_n);  
  currentFrame.x_pos += dx;
  currentFrame.y_pos += dy;
}

void driveForwards()
{
  analogWrite(PWM_A, DUTY_CYCLE);
  analogWrite(PWM_B, DUTY_CYCLE);
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  
  delayAndRecordDistanceValues();
  stopDriving();
  
  float distance;
  measureDistance(&distance);
  
  float dx = distance * cos(currentFrame.theta_n);
  float dy = distance * sin(currentFrame.theta_n);
  currentFrame.x_pos += dx;
  currentFrame.y_pos += dy;
}

void turnTightLeft()
{
  analogWrite(PWM_A, DUTY_CYCLE);
  analogWrite(PWM_B, DUTY_CYCLE);
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  
  delayAndRecordDistanceValues();
  stopDriving();
  
  float distance;
  measureDistance(&distance);
  
  float deltaTheta = distance / AXEL_LENGTH;
  float t_n = (currentFrame.theta_n + deltaTheta) / (float)(2 * PI);
  t_n = (int)(t_n + 0.5);
  currentFrame.theta_n = t_n;
}

void turnTightRight()
{
  analogWrite(PWM_A, DUTY_CYCLE);
  analogWrite(PWM_B, DUTY_CYCLE);
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  
  delayAndRecordDistanceValues();
  stopDriving();
  
  float distance;
  measureDistance(&distance);
  
  float deltaTheta = (-1) * (distance / AXEL_LENGTH);
  float t_n = (currentFrame.theta_n + deltaTheta) / (float)(2 * PI);
  t_n = (int)(t_n + 0.5);
  currentFrame.theta_n = t_n;
}

void turnWideLeft()
{
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, DUTY_CYCLE);
  digitalWrite(DIR_B, LOW);

  delayAndRecordDistanceValues();
  stopDriving();
  
  float distance;
  measureDistance(&distance);
  
  float deltaTheta = distance / AXEL_LENGTH;
  float deltaThetaRight;
  float deltaThetaLeft;
  float deltaThetaLow;
  float deltaThetaHigh;
  calculateThetaLowHigh(deltaTheta, &deltaThetaLow, &deltaThetaHigh);
  calculateThetaRightLeft(deltaTheta, &deltaThetaRight, &deltaThetaLeft);
  float dx = 0.5 * AXEL_LENGTH * ( sin(deltaThetaRight) - sin(deltaThetaLeft) );
  float dy = 0.5 * AXEL_LENGTH * ( cos(deltaThetaLow) - cos(deltaThetaHigh) );  
  currentFrame.theta_n += deltaTheta;
  currentFrame.x_pos += dx;
  currentFrame.y_pos += dy;
}

void turnWideRight()
{
  analogWrite(PWM_A, DUTY_CYCLE);
  analogWrite(PWM_B, 0);
  digitalWrite(DIR_A, HIGH);
  
  delayAndRecordDistanceValues();
  stopDriving();

  float distance;
  measureDistance(&distance);
  
  float deltaTheta = (-1) * (distance / AXEL_LENGTH);  
  float deltaThetaRight;
  float deltaThetaLeft;
  float deltaThetaLow;
  float deltaThetaHigh;
  rightCalculateThetaLowHigh(deltaTheta, &deltaThetaLow, &deltaThetaHigh);
  rightCalculateThetaRightLeft(deltaTheta, &deltaThetaRight, &deltaThetaLeft);
  float dx = (-1) * 0.5 * AXEL_LENGTH * ( sin(deltaThetaRight) - sin(deltaThetaLeft) );
  float dy = (-1) * 0.5 * AXEL_LENGTH * ( cos(deltaThetaLow) - cos(deltaThetaHigh) );  
  currentFrame.theta_n += deltaTheta;
  currentFrame.x_pos += dx;
  currentFrame.y_pos += dy;
}

void stopDriving()
{
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

void delayAndRecordDistanceValues()
{
  unsigned int initialTime = millis();
  while (millis() < (initialTime + ACTION_LENGTH))
  {
    //Artie is driving and the sensors are determining the distance travelled
  }
}

void measureDistance(float *distance)
{
  unsigned int countBeforeModFour = distance_counted_by_interrupt;
  unsigned int completeCircles = countBeforeModFour / 4;
  unsigned int fourthsOfCircle = countBeforeModFour % 4;
  float unRefinedCount = (float)completeCircles + ((float)fourthsOfCircle / 4.0);
  float count = unRefinedCount;
  (*distance) = count * WHEEL_CIRC;
  
  distance_counted_by_interrupt = 0;
}

void calculateThetaLowHigh(float deltaTheta, float *thetaLow, float *thetaHigh)
{
  float thetaInitial = currentFrame.theta_n;
  float thetaFinal = thetaInitial + deltaTheta;

  boolean thetaInitialInUpper;
  boolean thetaFinalInUpper;

  if ((thetaInitial >= 0) && (thetaInitial <= PI))
      thetaInitialInUpper = true;
  else
      thetaInitialInUpper = false;

  if ((thetaFinal >= 0) && (thetaFinal <= PI))
      thetaFinalInUpper = true;
  else
      thetaFinalInUpper = false;

  if (thetaInitialInUpper && thetaFinalInUpper)
  {
      *thetaHigh = deltaTheta;
      *thetaLow = 0;
      return;
  }
  else if ((!thetaInitialInUpper) && (!thetaFinalInUpper))
  {
      *thetaHigh = 0;
      *thetaLow = deltaTheta;
      return;
  }
  else if (!thetaInitialInUpper)
  {
      *thetaHigh = thetaFinal;
      *thetaLow = deltaTheta - *thetaHigh;
      return;
  }
  else
  {
      *thetaLow = thetaFinal - PI;
      *thetaHigh = deltaTheta - *thetaLow;
      return;
  }
}

void calculateThetaRightLeft(float deltaTheta, float *thetaRight, float *thetaLeft)
{
  float thetaInitial = currentFrame.theta_n;
  float thetaFinal = thetaInitial + deltaTheta;

  boolean thetaInitialInLeft;
  boolean thetaFinalInLeft;

  if ((thetaInitial >= (PI * 0.5)) && (thetaInitial <= (PI * 1.5)))
      thetaInitialInLeft = true;
  else
      thetaInitialInLeft = false;

  if ((thetaFinal >= (PI * 0.5)) && (thetaFinal <= (PI * 1.5)))
      thetaFinalInLeft = true;
  else
      thetaFinalInLeft = false;

  if (thetaInitialInLeft && thetaFinalInLeft)
  {
      *thetaLeft = deltaTheta;
      *thetaRight = 0;
      return;
  }
  else if ((!thetaInitialInLeft) && (!thetaFinalInLeft))
  {
      *thetaLeft = 0;
      *thetaRight = deltaTheta;
      return;
  }
  else if (!thetaInitialInLeft)
  {
      *thetaRight = (PI * 0.5) - thetaInitial;
      *thetaLeft = deltaTheta - *thetaRight;
      return;
  }
  else
  {
      *thetaRight = thetaFinal - (PI * 1.5);
      *thetaLeft = deltaTheta - *thetaRight;
      return;
  }
}

void rightCalculateThetaLowHigh(float deltaTheta, float *thetaLow, float *thetaHigh)
{
  double thetaInitial = currentFrame.theta_n;
  double thetaFinal = thetaInitial + deltaTheta;

  boolean thetaInitialInUpper;
  boolean thetaFinalInUpper;

  if ((thetaInitial >= 0) && (thetaInitial <= PI))
      thetaInitialInUpper = true;
  else
      thetaInitialInUpper = false;

  if ((thetaFinal >= 0) && (thetaFinal <= PI))
      thetaFinalInUpper = true;
  else
      thetaFinalInUpper = false;

  if (thetaInitialInUpper && thetaFinalInUpper)
  {
      *thetaHigh = deltaTheta;
      *thetaLow = 0;
      return;
  }
  else if ((!thetaInitialInUpper) && (!thetaFinalInUpper))
  {
      *thetaHigh = 0;
      *thetaLow = deltaTheta;
      return;
  }
  else if (!thetaInitialInUpper)
  {
      *thetaHigh = thetaFinal - PI;
      *thetaLow = deltaTheta - *thetaHigh;
      return;
  }
  else
  {
      *thetaHigh = thetaInitial;
      *thetaLow = deltaTheta - *thetaHigh;
      return;
  }
}

void rightCalculateThetaRightLeft(float deltaTheta, float *thetaRight, float *thetaLeft)
{
  double thetaInitial = currentFrame.theta_n;
  double thetaFinal = thetaInitial + deltaTheta;

  boolean thetaInitialInLeft;
  boolean thetaFinalInLeft;

  if ((thetaInitial >= (PI * 0.5)) && (thetaInitial <= (PI * 1.5)))
      thetaInitialInLeft = true;
  else
      thetaInitialInLeft = false;

  if ((thetaFinal >= (PI * 0.5)) && (thetaFinal <= (PI * 1.5)))
      thetaFinalInLeft = true;
  else
      thetaFinalInLeft = false;

  if (thetaInitialInLeft && thetaFinalInLeft)
  {
      *thetaLeft = deltaTheta;
      *thetaRight = 0;
      return;
  }
  else if ((!thetaInitialInLeft) && (!thetaFinalInLeft))
  {
      *thetaLeft = 0;
      *thetaRight = deltaTheta;
      return;
  }
  else if (!thetaInitialInLeft)
  {
      *thetaRight = thetaInitial - (PI * 1.5);
      *thetaLeft = deltaTheta - *thetaRight;
      return;
  }
  else
  {
      *thetaLeft = thetaInitial - PI;
      *thetaRight = deltaTheta - *thetaLeft;
      return;
  }
}
