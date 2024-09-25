#include<Wire.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int val;        // variable to control servo degree
const int MPU=0x68; 
const int MAX = 31;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

int16_t sum = 0;
float avg = 0;
int16_t queue[MAX];
int16_t front = 0;
int16_t rear = 0;
int16_t count = 0;
bool flag = false;

bool isFull()
{
  if(((rear == MAX - 1) && (front == 0) ||
      (rear == (front-1))))
    {
      return true;
    }
    
    return false; 
}

bool isEmpty()
{
  return (rear == front);
}

void enqueue(int data)
{
  if (!isFull())
    {
    //Serial.print("rear = "); Serial.println(rear);
      queue[rear++] = data;
        if(rear == MAX)
        {
          rear = 0;
        }
    }
    else
    {
      Serial.println("The Queue is Full!");
    }
}

static int dequeue() 
{
  if (!isEmpty())
    {
      int frontVal = queue[front];
     //Serial.print("front = "); Serial.println(front);
     front++;
      if(front == MAX)
      {
        front = 0;
      }
      return frontVal;
    }
    else
    {
      Serial.println("The Queue is Empty!");
        return -1;
    }
}

void  setup(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  val = 0;//analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  Serial.begin(9600);
}
void  loop(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);  
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();  
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read();  
  GyZ=Wire.read()<<8|Wire.read();  
  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(AcX);
  Serial.print(" | Y = "); Serial.print(AcY);
  Serial.print(" | Z = ");  Serial.println(AcZ); 

  count++;
  if(count == MAX)
  {
    flag = true;
  }

  if(flag)
  {
    avg = sum/10;
    if(avg )
    //Serial.print("Avg: "); 
    Serial.println(avg); 
    int dequeVal = dequeue();
    enqueue(AcX);
    sum += AcX;
    sum -= dequeVal;    
    //Serial.print("X = "); Serial.println(AcX);
    //Serial.print("dequeVal = "); Serial.println(dequeVal);
    //Serial.print("sum = "); Serial.println(sum);

    if(val <= 50)
    {
      if((avg >= -3000) && (avg <= -2000))
      {
        val = 0;
      }
      else if((avg > 3000) && (avg < 4000))
      {
        val = 10;
      }
      else if((avg > 2000) && (avg < 3000))
      {
        val = 20;
      }
      else if((avg > 1000) && (avg < 2000))
      {
        val = 30;
      }
      else if((avg > 0) && (avg < 1000))
      {
        val = 40;
      }
      else if((avg > -1000) && (avg < 0))
      {
        val = 50;
      }
    }
    else if((val >= 50) && (val <= 90))
    {
      if((avg > -4000) && (avg < -3000))
      {
        val = 60;
      }
      else if((avg > -3000) && (avg < -2000))
      {
        val = 70;
      }
      else if((avg > -2000) && (avg < -1000))
      {
        val = 80;
      }
      else if((avg > -1000) && (avg < 1000))
      {
        val = 90;
      }
    }       
  }
  else
  {
    //Serial.print("Count = "); Serial.println(count);
    enqueue(AcX);
    sum += AcX;
  }
  myservo.write(val);                  // sets the servo position according to the scaled value

  delay(15);
}