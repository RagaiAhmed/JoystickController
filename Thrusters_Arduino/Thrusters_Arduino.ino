void setup() 
{
  // Begins serial UART
  Serial.begin(9600);
}

unsigned int Vspeed = 0;
unsigned int Hspeed = 0;

long vals [4]= {0,0,0,0};
void loop() 
{
  while(Serial.available())
  {
    char ind  = Serial.read();
    int num = Serial.parseInt();

    switch(ind)
    {
      case 'V':
        Vspeed = num; break;
      case 'H':
        Hspeed = num; break;
      case 'R':
        vals[0]=num*Hspeed; break;
      case 'L':
        vals[0]=-num*Hspeed; break;
      case 'F':
        vals[1]=num*Hspeed; break;
      case 'B':
        vals[1]=-num*Hspeed; break;
      case 'U':
        vals[2]=num*Vspeed; break;
      case 'D':
        vals[2]=-num*Vspeed; break;
      case '>':
        vals[3]=num*Hspeed; break;
      case '<':
        vals[3]=-num*Hspeed; break;
    }
  }

  Serial.print("X speed : ");Serial.println(vals[0]);
  Serial.print("Y speed : ");Serial.println(vals[1]);
  Serial.print("Z speed : ");Serial.println(vals[2]);
  Serial.print("Rotation speed : ");Serial.println(vals[3]);


  delay(10);
}
