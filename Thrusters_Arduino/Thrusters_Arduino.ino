unsigned int Vspeed = 0;  // Maximum Vertical thruster speed
unsigned int Hspeed = 0;  // Maximum Horizontal thruster speed

const bool analog_speeds = false;  // Whether are speeds taken from joystick analog
const int cutoff_joystick = 10;  // Up to 10% is considered deadzone

const int no_thrusters = 8;  // Number of connected thrusters
const int no_directions = 4; // Number of axes of movement

#define DEBUG 2  // Debug level (the bigger the number the more it is verbose)

// Thrusters pins
#define ThrusterPwm_A 0
#define ThrusterDir_A 0
#define ThrusterPwm_B 0
#define ThrusterDir_B 0
#define ThrusterPwm_C 0
#define ThrusterDir_C 0
#define ThrusterPwm_D 0
#define ThrusterDir_D 0
#define ThrusterPwm_E 5
#define ThrusterDir_E 4
#define ThrusterPwm_F 3
#define ThrusterDir_F 2
#define ThrusterPwm_G 5
#define ThrusterDir_G 4
#define ThrusterPwm_H 3
#define ThrusterDir_H 2

// Putting them in arrays for easier usage
int thrusters_pwm_pins[no_thrusters] = {ThrusterPwm_A,ThrusterPwm_B,ThrusterPwm_C,ThrusterPwm_D,ThrusterPwm_E,ThrusterPwm_F,ThrusterPwm_G,ThrusterPwm_H};
int thrusters_dir_pins[no_thrusters] = {ThrusterDir_A,ThrusterDir_B,ThrusterDir_C,ThrusterDir_D,ThrusterDir_E,ThrusterDir_F,ThrusterDir_G,ThrusterDir_H};

// Array holding percentages in each of the directions from the joystick 
int vals [no_directions] = {};

// Array holding each thruster actual speed in range [0,1000]
double thruster_speeds[no_thrusters] = {};

// Translates each axis direction into direction of thrusters
// Numbered from 0 to 7 clockwise starting from the top right corner horizontal thruster, then the vertical thrusters
const int directions[no_directions][no_thrusters]=
{
  {-1,1,-1,1,0,0,0,0}, // RL
  {1,1,1,1,0,0,0,0},   // FB
  {0,0,0,0,1,1,1,1},   // UD
  {-1,-1,1,1,0,0,0,0}  // ><
};

// Adds two arrays in place of the first array
template<int size,typename T1,typename T2>
void self_vec_add(T1 *self,T2 *other)
{
  for(int i=0;i<size;i++)
    self[i]+=other[i];  
}

// Multiplies an array by a value in place of the first array
template<int size,typename T1,typename T2>
void self_vec_mult(T1 *self,T2 other)
{
  for(int i=0;i<size;i++)
    self[i]*=other;  
}


void setup() 
{
  // Begins serial UART
  Serial.begin(9600);

  Serial.setTimeout(50);
}

// Adjusts thrusters speeds 
void adjust_thrusters()
{
  bool exceed_max = false;
  if(analog_speeds) // If using the analog velocity from the joystick
  {
    // Muliplies by a const ratio 
    for(int i=0;i<4;i++) thruster_speeds[i] = thruster_speeds[i] * Hspeed / 100;
    for(int i=4;i<8;i++) thruster_speeds[i] = thruster_speeds[i] * Vspeed / 100;


    // If combined speeds exceed max, then flag to be scaled down
    for(int i=0;i<4;i++) 
    {
      if (thruster_speeds[i]>Hspeed)
      {
        exceed_max = true; 
        break;
      }

      if (thruster_speeds[i+4]>Vspeed)
      {
        exceed_max = true; 
        break;
      }
    }
  }
  
  if(!analog_speeds||exceed_max) // If only using its direction or need to scale it because it exceeded the limit
  { 
    double max_H=0,max_V=0;

    // Gets the max Horizontal speed and Vertical speed of the thrusters
    for(int i=0;i<4;i++) max_H = max(max_H,thruster_speeds[i]);
    for(int i=4;i<8;i++) max_V = max(max_V,thruster_speeds[i]);

    // Scales them, so that they keep the same ratio, but thrusters are maxed to the set max speed
    if(max_H) for(int i=0;i<4;i++) thruster_speeds[i] = thruster_speeds[i] * Hspeed / max_H;
    if(max_V) for(int i=4;i<8;i++) thruster_speeds[i] = thruster_speeds[i] * Vspeed / max_V;
  }
}

// Applies the set thruster speeds to the driver
void apply_to_driver()
{
  for (int i=0;i<no_thrusters;i++) 
  {
    digitalWrite(thrusters_dir_pins[i],thruster_speeds[i]>0);  // Set direction pin
    analogWrite(thrusters_pwm_pins[i],map(abs(thruster_speeds[i]),0,1000,0,255));  // Set pwm pin
  }
  
}

void loop() 
{
  // If there any serial data
  while(Serial.available())
  {
    // Read the first char
    char ind  = Serial.read();
    if(ind=='\n'||ind=='\0') continue;

    // Parse then next int
    int num = Serial.parseInt();

    if(DEBUG>=3)  // Print them for debugging
    {
      Serial.println(ind);
      Serial.println(num);
    }

    int changed = -1;  // The changed direction
    switch(ind)
    {
      case 'V':  // Change Veritcal max speed
        Vspeed = num; changed = -2; break;
      case 'H':  // Change Horizontal max speed
        Hspeed = num; changed = -2; break;
      case 'R':  // Right
        changed = 0; break;
      case 'L':  // Left
        changed = 1; break;
      case 'F':  // Forward
        changed = 2; break;
      case 'B':  // Backwards
        changed = 3; break;
      case 'U':  // Upwards
        changed = 4; break;
      case 'D':  // Downwards
        changed = 5; break;
      case '>':  // Rotate clockwise (from top view)
        changed = 6; break;
      case '<':  // Rotate anti-clockwise (from top view)
        changed = 7; break;

    }
    
    if(changed>=0)  // If changed a valid direction
    {
      if(changed%2==1) num*=-1;  // if odd direction then make the value negative
      changed = changed/2;  // Scale it to our 4 axes system

      // If bigger than cutoff apply it
      if(abs(num)>=cutoff_joystick) vals[changed]= num;
      else vals[changed] = 0;
    }

    
    if(changed!=-1)  // If any change happened
    {
      // Reset old speeds
      for(int i=0;i<no_thrusters;i++) thruster_speeds[i]=0;

      // Add all thruster directions multiplied by their magnitudes
      for(int i = 0; i<no_directions; i++) 
      {
        double dir[no_thrusters];
        for(int j=0;j<no_thrusters;j++) dir[j]= directions[i][j];  // Make a thrusters' speed direction array
        
        self_vec_mult<no_thrusters>(dir,vals[i]);  // Multiply it with magnitude
        self_vec_add<no_thrusters>(thruster_speeds,dir);  // Add it to the thruster speeds
        
      }

      if(DEBUG>=3)  // For debugging purposes
      {
        Serial.print("\nBefore adjustment\n");
        Serial.print("Thruster A : ");Serial.println(thruster_speeds[0]);
        Serial.print("Thruster B : ");Serial.println(thruster_speeds[1]);
        Serial.print("Thruster C : ");Serial.println(thruster_speeds[2]);
        Serial.print("Thruster D : ");Serial.println(thruster_speeds[3]);
        Serial.print("Thruster E : ");Serial.println(thruster_speeds[4]);
        Serial.print("Thruster F : ");Serial.println(thruster_speeds[5]);
        Serial.print("Thruster G : ");Serial.println(thruster_speeds[6]);
        Serial.print("Thruster H : ");Serial.println(thruster_speeds[7]);
        delay(1000);
      }
      
      adjust_thrusters();  // Adjusts thruster speeds
      apply_to_driver();  // Applies them to thrusters driver
    }
  }

  if(DEBUG>=2)  // For debugging purposes
  {
    Serial.print("H speed : ");Serial.println(Hspeed);
    Serial.print("V speed : ");Serial.println(Vspeed);
  
    double direction_norm =sqrt(vals[0]*vals[0]+vals[1]*vals[1]);
    if(direction_norm)
    {
      Serial.print("direction ratio: ");Serial.print(vals[0]/direction_norm);Serial.print(":");Serial.println(vals[1]/direction_norm);
    }
    Serial.print("\nAfter adjustment \n");
    Serial.print("Thruster A : ");Serial.println(thruster_speeds[0]);
    Serial.print("Thruster B : ");Serial.println(thruster_speeds[1]);
    Serial.print("Thruster C : ");Serial.println(thruster_speeds[2]);
    Serial.print("Thruster D : ");Serial.println(thruster_speeds[3]);
    Serial.print("Thruster E : ");Serial.println(thruster_speeds[4]);
    Serial.print("Thruster F : ");Serial.println(thruster_speeds[5]);
    Serial.print("Thruster G : ");Serial.println(thruster_speeds[6]);
    Serial.print("Thruster H : ");Serial.println(thruster_speeds[7]);
  }
  

  delay(10);
}
