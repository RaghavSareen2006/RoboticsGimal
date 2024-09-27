#include<Wire.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int val;        // variable to control servo degree
const int MPU=0x68; 
const int MAX = 11;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

int32_t sum = 0;
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
char tmp_str[7]; // temporary variable used in convert function

char* convert_int16_to_str(int16_t i) { // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void  setup(){
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  val = 0;
  Serial.begin(9600);
}
void  loop(){
   Wire.beginTransmission(MPU);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
  Wire.requestFrom(MPU, 7*2, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
  AcX = Wire.read()<<8 | Wire.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read()<<8 | Wire.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read()<<8 | Wire.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
  temperature = Wire.read()<<8 | Wire.read(); // reading registers: 0x41 (TEMP_OUT_H) and 0x42 (TEMP_OUT_L)
  GyX = Wire.read()<<8 | Wire.read(); // reading registers: 0x43 (GYRO_XOUT_H) and 0x44 (GYRO_XOUT_L)
  GyY = Wire.read()<<8 | Wire.read(); // reading registers: 0x45 (GYRO_YOUT_H) and 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read()<<8 | Wire.read(); // reading registers: 0x47 (GYRO_ZOUT_H) and 0x48 (GYRO_ZOUT_L)
  
  // print out data
  Serial.print("aX = "); Serial.print(convert_int16_to_str(AcX));
  Serial.print(" | aY = "); Serial.print(convert_int16_to_str(AcY));
  Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(AcZ));
  // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
  Serial.print(" | tmp = "); Serial.print(temperature/340.00+36.53);
  Serial.print(" | gX = "); Serial.print(convert_int16_to_str(GyX));
  Serial.print(" | gY = "); Serial.print(convert_int16_to_str(GyY));
  Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(GyZ)); 
  Serial.println(); 

  count++;
  if(count == MAX)
  {
    flag = true;
  }

  if(flag)
  {
    avg = sum/(MAX-1);
    //Serial.print("Avg: "); 
    Serial.println(avg); 
    int dequeVal = dequeue();
    enqueue(AcX);
    sum += AcX;
    sum -= dequeVal;  
    //Serial.print("X = "); Serial.println(AcX);
    //Serial.print("dequeVal = "); Serial.println(dequeVal);
    //Serial.print("sum = "); Serial.println(sum);

      if(avg >= 17500)
      {
        val = 0;
      }
      else if((avg >= 16900) && (avg <= 17499))
      {
        val = 10;
      }
      else if((avg >= 16200) && (avg <= 16899))
      {
        val = 20;
      }
      else if((avg >= 15700) && (avg <= 16199))
      {
        val = 30;
      }
      else if((avg >= 14700) && (avg <= 15699))
      {
        val = 40;
      }
      else if((avg >= 13000) && (avg <= 14699))
      {
        val = 50;
      }
      else if((avg >= 11500) && (avg <= 12999))
      {
        val = 60;
      }
      else if((avg >= 9000) && (avg <= 11499))
      {
        val = 70;
      }
      else if((avg >= 7000) && (avg <= 8999))
      {
        val = 80;
      }
      else if((avg >= 3500) && (avg <= 6999))
      {
        val = 90;
      }
      else if((avg >= 500) && (avg <= 3499))
      {
        val = 100;
      }
      else if((avg >= -2000) && (avg <= 499))
      {
        val = 100;
      }
      else if((avg >= -4500) && (avg <= -2001))
      {
        val = 110;
      }
      else if((avg >= -5500) && (avg <= -4501))
      {
        val = 120;
      }
      else if((avg >= -6500) && (avg <= -5501))
      {
        val = 130;
      }
      else if((avg >= -10000) && (avg <= -6501))
      {
        val = 140;
      }
      else if((avg >= -11500) && (avg <= -10001))
      {
        val = 150;
      }
      else if((avg >= -13500) && (avg <= -11501))
      {
        val = 160;
      }
      else if((avg >= -15500) && (avg <= -13501))
      {
        val = 170;
      }
      else if((avg >= -16000) && (avg <= -15501))
      {
        val = 180;
      }
  }
  else
  {
    //Serial.print("Count = "); Serial.println(count);
    enqueue(AcX);
    sum += AcX;
  }
  myservo.write(val);                  // sets the servo position according to the scaled value

  delay(50);
}
