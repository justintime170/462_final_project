#include <FileIO.h>
#include <Console.h>
#include <Mailbox.h>
#include <NewPing.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>

int trig[] = {8,7,11};
int echo[] = {9,6,10};
int stepCount = 40*4;
int statusLED = A2;


//int motor1_speed = 9;
//int motor2_speed = 10;
//int motor1_dir = 12;
//int motor2_dir = 13;

//NewPing sonarLeft(2, 3);
//NewPing sonarRight(4, 5);
//NewPing sonarFront(7, 6);

int motor1_speed = A0;
int motor2_speed = 4;
int motor1_dir = A1;
int motor2_dir = 5;
String lastMessage;
int lineCounter = 0;

NewPing sonarFront(8, 9);
NewPing sonarRight(7, 6);
NewPing sonarLeft(11, 10);

unsigned int dist[3] = {0,0,0};

bool frwrd = false;
bool moveNow = false;

Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_LSM303_Mag_Unified   mag   = Adafruit_LSM303_Mag_Unified(30302);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);

Adafruit_9DOF                dof   = Adafruit_9DOF();

sensors_event_t accel_event;
sensors_event_t mag_event;
sensors_vec_t   orientation;

void displaySensorDetails(void)
{
  sensor_t sensor;
  
  accel.getSensor(&sensor);
  Console.println(F("----------- ACCELEROMETER ----------"));
  Console.print  (F("Sensor:       ")); Console.println(sensor.name);
  Console.print  (F("Driver Ver:   ")); Console.println(sensor.version);
  Console.print  (F("Unique ID:    ")); Console.println(sensor.sensor_id);
  Console.print  (F("Max Value:    ")); Console.print(sensor.max_value); Console.println(F(" m/s^2"));
  Console.print  (F("Min Value:    ")); Console.print(sensor.min_value); Console.println(F(" m/s^2"));
  Console.print  (F("Resolution:   ")); Console.print(sensor.resolution); Console.println(F(" m/s^2"));
  Console.println(F("------------------------------------"));
  Console.println(F(""));

  gyro.getSensor(&sensor);
  Console.println(F("------------- GYROSCOPE -----------"));
  Console.print  (F("Sensor:       ")); Console.println(sensor.name);
  Console.print  (F("Driver Ver:   ")); Console.println(sensor.version);
  Console.print  (F("Unique ID:    ")); Console.println(sensor.sensor_id);
  Console.print  (F("Max Value:    ")); Console.print(sensor.max_value); Console.println(F(" rad/s"));
  Console.print  (F("Min Value:    ")); Console.print(sensor.min_value); Console.println(F(" rad/s"));
  Console.print  (F("Resolution:   ")); Console.print(sensor.resolution); Console.println(F(" rad/s"));
  Console.println(F("------------------------------------"));
  Console.println(F(""));
  
  mag.getSensor(&sensor);
  Console.println(F("----------- MAGNETOMETER -----------"));
  Console.print  (F("Sensor:       ")); Console.println(sensor.name);
  Console.print  (F("Driver Ver:   ")); Console.println(sensor.version);
  Console.print  (F("Unique ID:    ")); Console.println(sensor.sensor_id);
  Console.print  (F("Max Value:    ")); Console.print(sensor.max_value); Console.println(F(" uT"));
  Console.print  (F("Min Value:    ")); Console.print(sensor.min_value); Console.println(F(" uT"));
  Console.print  (F("Resolution:   ")); Console.print(sensor.resolution); Console.println(F(" uT"));  
  Console.println(F("------------------------------------"));
  Console.println(F(""));

  delay(500);
}

void read9DOF()
{
  
  double sum = 0;

  /* Calculate pitch and roll from the raw accelerometer data */
  accel.getEvent(&accel_event);
  if (dof.accelGetOrientation(&accel_event, &orientation))
  {
    /* 'orientation' should have valid .roll and .pitch fields */
    Console.print(F("Roll: "));
    Console.print(orientation.roll);
    Console.print(F("; "));
    Console.print(F("Pitch: "));
    Console.print(orientation.pitch);
    Console.print(F("; "));
  }
  
  /* Calculate the heading using the magnetometer */
  for(int i = 0; i < 20; i++)
  {
    mag.getEvent(&mag_event);
    if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
    {
      
      /* 'orientation' should have valid .heading data now */
      /*Console.print(F("Heading: "));
      Console.print(orientation.heading);
      Console.print(F("; "));*/
      sum += orientation.heading;
    }
  }
  double avg = sum/20;
  Console.print(F("Heading: "));
  Console.print(avg);
  Console.print(F("; "));
  Console.println(F(""));
}

void readUltrasonics()
{
  int distL = sonarLeft.ping_median(5);
  dist[0] = sonarLeft.convert_cm(distL);
  //Console.println(dist[0]);
  int distR = sonarRight.ping_median(5);
  dist[1] = sonarRight.convert_cm(distR);
  //Console.println(dist[1]);
  int distF = sonarFront.ping_median(5);
  dist[2] = sonarFront.convert_cm(distF);
  //Console.println(dist[2]);
  //Console.println("Read Complete\n");
}

void dataLogger(double magAv)
{
  readUltrasonics();
  int irs[3];
  irs[0] = analogRead(3);
  irs[1] = analogRead(5);
  irs[2] = analogRead(4);
  
  
  //irs[0] = map(analogRead(3), 0, 800, 33, 0);
  //irs[1] = map(analogRead(5), 0, 800, 33, 0);
  //irs[2] = map(analogRead(4), 0, 800, 33, 0);
  
  String dataString = String(lineCounter);
  dataString = "{ ultra: {\"left\" : ";
  dataString += dist[0];
  dataString += ", \"right\" : ";
  dataString += dist[1];
  dataString += ", \"front\" : ";
  dataString += dist[2];
  dataString += "}, ir : {\"left\" : ";
  dataString += irs[0];
  dataString += ", \"right\" : ";
  dataString += irs[1];
  dataString += ", \"front\" : ";
  dataString += irs[2];
  dataString += "}, \"heading\" : ";
  dataString += magAv;
  dataString += "}";

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // The FileSystem card is mounted at the following "/mnt/FileSystema1"
  File dataFile = FileSystem.open("/mnt/sda1/distlog.txt", FILE_APPEND);

  // if the file is available, write to it:
  if (dataFile)
  {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    //Console.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else
  {
    Console.println("error opening distlog.txt");
  }
}

void stepOnce()
{
  digitalWrite(motor1_speed, HIGH);
  digitalWrite(motor2_speed, HIGH);
  delayMicroseconds(250);
  digitalWrite(motor1_speed, LOW);
  digitalWrite(motor2_speed, LOW);
  delay(5);
}
void stepOpposite()
{
  digitalWrite(motor2_speed, HIGH);
  digitalWrite(motor1_speed, HIGH);
  delayMicroseconds(250);
  digitalWrite(motor2_speed, LOW);
  digitalWrite(motor1_speed, LOW);
  delay(5);
}
void stepCorrect(int motor)
{
  switch(motor)
  {
    case 1:
    {
      digitalWrite(motor1_speed, HIGH);
      delayMicroseconds(250);
      digitalWrite(motor1_speed, LOW);
      delay(5);
    }
    case 2:
    {
      digitalWrite(motor2_speed, HIGH);
      delayMicroseconds(250);
      digitalWrite(motor2_speed, LOW);
      delay(5);
    }
  }
}

void forward()
{
  digitalWrite(motor1_dir,HIGH);
  digitalWrite(motor2_dir,LOW);
}
void backward()
{
  digitalWrite(motor1_dir,LOW);
  digitalWrite(motor2_dir,HIGH);
}
void left()
{
  digitalWrite(motor1_dir,HIGH);
  digitalWrite(motor2_dir,HIGH);
}
void right()
{
  digitalWrite(motor1_dir,LOW);
  digitalWrite(motor2_dir,LOW);
}
void ten(char dir)
{
  switch(dir)
  {
    case 'r':
    {
      right();
      break;
    }
    case 'l':
    {
      left();
      break;
    }
  }
  for(int a = 0; a < 25; a++)
  {
    stepOnce();
  }
  if(frwrd)
  {
    forward();
  }
  else
  {
    backward();
  }
}
void ninety(char dir)
{
  switch(dir)
  {
    case 'r':
    {
      right();
      break;
    }
    case 'l':
    {
      left();
      break;
    }
  }
  for(int a = 0; a < 446; a++)
  {
    stepOnce();
  }
  if(frwrd)
  {
    forward();
  }
  else
  {
    backward();
  }
}

void setup()
{
  // Initialize the Bridge and the Serial
  Bridge.begin();
  FileSystem.begin();
  Console.begin();
  Mailbox.begin();
  File dataFile = FileSystem.open("/mnt/sda1/distlog.txt", FILE_WRITE);
  dataFile.println("New Run\n");
  dataFile.close();
  //while(!Console.connected()){}
  //Console.println("Connected!");
  for(int a = 0; a < 3; a++)
  {
    pinMode(trig[a], OUTPUT);
    digitalWrite(trig[a], HIGH);
    pinMode(echo[a], INPUT);
  }
  //while(!Serial);  // wait for Serial port to connect.
  //Serial.println("Filesystem datalogger and movements\n");
  Console.println(F("Adafruit 9DOF Tester")); Console.println("");
  
  /* Initialise the sensors */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Console.println(F("Ooops, no LSM303 detected ... Check your wiring!"));
    while(1);
  }
  if(!mag.begin())
  {
    /* There was a problem detecting the LSM303 ... check your connections */
    Console.println("Ooops, no LSM303 detected ... Check your wiring!");
    while(1);
  }
  if(!gyro.begin())
  {
    /* There was a problem detecting the L3GD20 ... check your connections */
    Console.print("Ooops, no L3GD20 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  //displaySensorDetails();
  forward();
  frwrd = true;
  pinMode(statusLED, OUTPUT);
  analogWrite(statusLED, 0); 
  
}
bool stepSide = false;
void loop ()
{
  if(Mailbox.messageAvailable())
  {
    while(Mailbox.messageAvailable())
    {
      Mailbox.readMessage(lastMessage);
      Console.println(lastMessage);
      if(lastMessage.equals("f"))
      {
        moveNow = true;
        forward();
        frwrd = true;
      }
      else if(lastMessage.equals("b"))
      {
        moveNow = true;
        backward();
        frwrd = false;
      }
      else if(lastMessage.equals("sl"))
      {
        //moveNow = true;
        ten('l');
      }
      else if(lastMessage.equals("fl"))
      {
        //moveNow = true;
        ninety('l');
      }
      else if(lastMessage.equals("sr"))
      {
        //moveNow = true;
        ten('r');
      }
      else if(lastMessage.equals("fr"))
      {
        //moveNow = true;
        ninety('r');
      }
      else if(lastMessage.equals("st"))
      {
        moveNow = false;
      }
      else if(lastMessage.equals("go"))
      {
        moveNow = true;
      }
      else if(lastMessage.equals("con"))
      {
        analogWrite(statusLED, 1024);
      }
      else if(lastMessage.equals("dis"))
      {
        analogWrite(statusLED, 0);
      }
      else if(lastMessage.equals("datareset"))
      {
        File dataFile = FileSystem.open("/mnt/sda1/distlog.txt", FILE_WRITE);
        dataFile.println(String(" "));
        dataFile.close();
      }
    }
  }
  if(moveNow)
  {
    
    //read9DOF();
    
    double magSum;
    double yawSteps[stepCount];
    if(stepSide)
    {
      for(int i = 0; i < stepCount; i++)
      {
        stepOnce();
        mag.getEvent(&mag_event);
        if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
        {
          magSum += orientation.heading;
        }
        /*accel.getEvent(&accel_event);
        if (dof.accelGetOrientation(&accel_event, &orientation))
        {
          yawSteps[i] = orientation.yaw;
        }*/
      }
    }
    else
    {
      for(int i = 0; i < stepCount; i++)
      {
        stepOpposite();
        mag.getEvent(&mag_event);
        if (dof.magGetOrientation(SENSOR_AXIS_Z, &mag_event, &orientation))
        {
          magSum += orientation.heading;
        }
        /*accel.getEvent(&accel_event);
        if (dof.accelGetOrientation(&accel_event, &orientation))
        {
          yawSteps[i] = orientation.yaw;
        }*/
      }
    }
    double magAvg = magSum/stepCount;
    dataLogger(magAvg);
    stepSide = !stepSide;
    //Console.println("Stepped 20 steps");
    //delay(500);
  }
}
