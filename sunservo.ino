//-------------------------------------------------------------
// Solar model rotator - Sun driven servo motor
// NMBU 2021 TEL100 GROUP 2
//-------------------------------------------------------------

//DEFINES - USER MODIFIABLE------------------------------------
#define DELAYINHOURS          0.0003 //Hours between each new rotation
#define LUXCUTOFF             200    //For which lux value the system will assume night time (SKRIV HVA SOM ER MAKS MIN OG HVA SOM ER MER MINDRE)
#define LUXDIFERENCETOLERANCE 10     //How much difference in lux the two sensors can have before deciding to move in either direction (EXCLUSIVE)! CANNOT BE 0!
#define SERVO_PIN             9      //Which pin the servo signal is connected to 
#define LEFT_PHOTORES         A0     //Which pin left photoresistor is connected to
#define RIGHT_PHOTORES        A1     //Which pin right photoresistor is connected to
#define GLOBAL_PHOTORES       A2     //Which pin global photoresistor is connected to
//DO NOT MODIFY BEYOND THIS LINE

//INCLUDES----------------------------------------------------
#include <Servo.h>

//VARIABLE DEFINITIONS
Servo Panelservo;
byte angle = 90;
enum Control
{
  NONE, LEFT, STAY, RIGHT
}; //Which direction the model will rotate


//FUNCTIONS---------------------------------------------------
void setup() //Setup function 
{
  Serial.begin(9600);             //Initialization
  Panelservo.attach(SERVO_PIN);   //Attaches servo to pin
  Panelservo.write(angle);        //Moves servo to 90 degrees NOTE: THIS WILL HAPPEN SUDDENLY AND MIGHT DAMAGE PANEL IF CONNECTED DURING STARTUP
  pinMode(LEFT_PHOTORES, INPUT);  //Sets analog pin to input
  pinMode(RIGHT_PHOTORES, INPUT); //Sets analog pin to input
  pinMode(GLOBAL_PHOTORES, INPUT);//Sets analog pin to input
  delay(2000);                    //Delay before starting to prevent sudden movement after startup
}

bool checkfordaytime()
{
  int value = analogRead(GLOBAL_PHOTORES); //Reads light from global photoresistor
  if (value > LUXCUTOFF) //If this light is greater then preset cutoffvalue
  {
    return true;         //There is daylight outside
  }
  else
  {
    return false;       //It is night time
  }
}
void errorHandle() //Called in case the code can't determine which direction to move
{
  Serial.println("NONE WAS RETURNED!"); //Debug message
}
Control directionofmostsunlight()
{
  int right = analogRead(RIGHT_PHOTORES); //Reads light from right photoresistor
  int left = analogRead(LEFT_PHOTORES);   //Reads light from left photoresistor
  int difference = abs(right-left);       //Finds difference between right and left photoresistor
  if (difference < LUXDIFERENCETOLERANCE) //Checks if difference between right and left is within tolerance 
  {
    return STAY;
  }
  else if (right > left)
  {
    return RIGHT;
  }
  else if (left > right)
  {
    return LEFT;
  }
  if (right == left)
  {
    Serial.print("RIGHT = LEFT BUT NONE IS RETURNED! MISTAKE IN FUNCTION directionofmostsunlight"); //Debug message. This will return none and should not be reached due to difference check 
  }
  return NONE; //This should not happen
}

void moveLeft(Control* modelControl, byte* iterations) //Movement to the left
{
  if (angle<=180) //Prevents servo from moving further than servo range of motion
  {
    angle += 1;   //Increments angle (higher angle means movement to the left)
    Panelservo.write(angle); //Writes angle to servo
  }
  Serial.print("LEFT");     //For debugging
  movement(modelControl, iterations); //Recursivly calls movement()
}

void moveRight(Control* modelControl, byte* iterations) //Movement to the right
{
  if (angle>10) //Prevents servo from moving further than servo range of motion
  {
    angle -= 1; //Decrements angle (lower angle means movement to the right)
    Panelservo.write(angle); //Writes angle to servo
  }
  Serial.print("RIGHT");     //For debugging
  movement(modelControl, iterations); //Recursivly calls movement()
  
}
void movement(Control* modelControl, byte* iterations) //Calls which direction to move after checking the direction of most sunlight
{
  *iterations+=1; //Increments iterations
  delay(100);     //Delay to prevent the servo from moving to fast and damaging the model
  Serial.print(*iterations); //FOR DEBUG: Prints iterations and the angle of servo
  Serial.print(" ");
  Serial.println(angle);
  if (*iterations < 180) //Is only run if the servo has not moved 180 times or more
  {

    *modelControl = directionofmostsunlight(); //Tells the modelcontrol which direction the panel needs to move
    
    if (*modelControl == LEFT)
    { 
      moveLeft(modelControl, iterations); //Moves left if left side has more sunlight
    }
    else if (*modelControl == STAY)       
    {
      Serial.println("STAY");            //Prints STAY if both sides have about equal amount of sunlight
    }
    else if (*modelControl == RIGHT)
    {
      moveRight(modelControl, iterations); //Moves right if right side has more sunlight
    }
    else
    {
      errorHandle();                       //If modelcontrol is not set to anything, something is wrong
    }
  }
}


void loop() //Arduino loop function
{

  bool isDayTime = checkfordaytime(); //Returns whether it is day or night
  
  if (isDayTime) //Executes only if it is daytime
  {
    byte iterations = 0; //Measures how often movement() is called
    
    Control modelControl; //Creates new modelcontrol which will be deleted after iteration

    movement(&modelControl, &iterations); //Calls movement() with modelControl and iterations as reference
    
  }
  delay(DELAYINHOURS*3600000); //Delays next execution by preset hours, delay() needs to get miliseconds as input, and hours is therefore multiplied by hour-to-miliseconds constant
}
