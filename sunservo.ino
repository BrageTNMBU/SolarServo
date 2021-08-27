//-------------------------------------------------------------
// Solar model rotator - Sun driven servo motor
// NMBU 2021 TEL100 GROUP 2
//-------------------------------------------------------------

//DEFINES - USER MODIFIABLE------------------------------------
#define DELAYINHOURS          0.0003 //Hours between each new rotation
#define LUXCUTOFF             700    //For which lux value the system will assume night time (SKRIV HVA SOM ER MAKS MIN OG HVA SOM ER MER MINDRE)
#define STEPS                 3      //How many steps the sensor will search in each direction when the assumed optimal position is found
#define LUXDIFERENCETOLERANCE 10     //How much difference in lux the two sensors can have before deciding to move in either direction (EXCLUSIVE)
#define SERVO_PIN             9      //Which pin the servo signal is connected to 
//DO NOT MODIFY BEYOND THIS LINE

//INCLUDES----------------------------------------------------
#include <Servo.h>

//VARIABLE DEFINITIONS
Servo Panelservo;
int angle = 90;
enum Control
{
  NONE, LEFT, STAY, RIGHT
}; //Which direction the model will rotate


//FUNCTIONS---------------------------------------------------
void setup() //Setup function 
{
  Serial.begin(9600);
  Panelservo.attach(SERVO_PIN); //Attaches servo to pin
  Panelservo.write(angle);      //Moves servo to 90 degrees NOTE: THIS WILL HAPPEN SUDDENLY AND MIGHT DAMAGE PANEL IF CONNECTED DURING STARTUP
  delay(2000);                  //Delay before starting to prevent sudden movement after startup
}

bool checkfordaytime()
{
  return true;
}
void errorHandle() //Called in case the code can't determine which direction to move
{
  Serial.println(""); //Debug message
}
Control directionofmostsunlight()
{
  return LEFT;

  return NONE; //This should not happen
}

void moveLeft(Control* modelControl, int* iterations) //Movement to the left
{
  if (angle<=180) //Prevents servo from moving further than servo range of motion
  {
    angle += 1;   //Increments angle (higher angle means movement to the left)
    Panelservo.write(angle); //Writes angle to servo
  }
  Serial.print("LEFT");     //For debugging
  movement(modelControl, iterations); //Recursivly calls movement()
}

void moveRight(Control* modelControl, int* iterations) //Movement to the right
{
  if (angle>10) //Prevents servo from moving further than servo range of motion
  {
    angle -= 1; //Decrements angle (lower angle means movement to the right)
    Panelservo.write(angle); //Writes angle to servo
  }
  Serial.print("RIGHT");     //For debugging
  movement(modelControl, iterations); //Recursivly calls movement()
  
}
void movement(Control* modelControl, int* iterations) //Calls which direction to move after checking the direction of most sunlight
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
      moveLeft(modelControl, iterations); //Moves left if left side has more sunlight
    else if (*modelControl == STAY){}     //Does nothing if both sides have equal amount of sunlight
    else if (*modelControl == RIGHT)
      moveRight(modelControl, iterations); //Moves right if right side has more sunlight
    else
      errorHandle();                       //If modelcontrol is not set to anything, something is wrong
  }
}


void loop() //Arduino loop function
{
  // put your main code here, to run repeatedly:
  bool isDayTime = checkfordaytime(); //Returns whether it is day or night
  
  if (isDayTime) //Executes only if it is daytime
  {
    int iterations = 0; //Measures how often movement() is called
    
    Control modelControl; //Creates new modelcontrol which will be deleted after iteration

    movement(&modelControl, &iterations); //Calls movement() with modelControl and iterations as reference
    
  }
  delay(DELAYINHOURS*3600000); //Delays next execution by preset hours, delay() needs to get miliseconds as input, and hours is therefore multiplied by hour-to-miliseconds constant
}
