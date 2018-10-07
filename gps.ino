#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <LCD5110_Basic.h>

// Polaczenia 
//      SCK  - Pin 8
//      MOSI(Din) - Pin 9
//      DC   - Pin 10
//      RST  - Pin 11
//      CS(ce)   - Pin 12
//      GPS(rx) - Pin 3
//      GPS(rx) - Pin 4
//      Button - Pin 7, GND

const long interval = 1000;
const int button = 7;
const int items = 6;
LCD5110 myGLCD(8,9,10,11,12);
SoftwareSerial serial_c(3, 4);
TinyGPSPlus gps;
int menuItem = 1, state = 0;
double distance = 0;
unsigned long lastTime = 0, currentTime;
extern uint8_t SmallFont[];
bool found = false;

void setup()
{
  myGLCD.InitLCD();
  serial_c.begin(9600);
  myGLCD.setFont(SmallFont);
  pinMode(button, INPUT_PULLUP);
}

void _cords()
{
  double dlng = gps.location.lng();
  double mlng = ((dlng - (int)dlng) * 60);
  //double slng = (dlng - int(dlng) - (double(mlng)/60)) * 3600;
  double dlat = gps.location.lat();
  double mlat = ((dlat - (int)dlat) * 60);
  //double slat = (dlat - int(dlat) - (double(mlat)/60)) * 3600;
  myGLCD.print("Wspolrzedne", CENTER, 0);
  myGLCD.print(String(int(dlat)) + " " + String(mlat) + "' ", LEFT, 16);   
  myGLCD.print(String(int(dlng)) + " " + String(mlng) + "' ", LEFT, 26);
}

void _sat()
{
  myGLCD.print("Licznik sat.", CENTER, 0);
  myGLCD.print(String(gps.satellites.value()), LEFT, 16); 
}

void _speed()
{
  myGLCD.print("Predkosc", CENTER, 0);
  myGLCD.print(String(gps.speed.kmph()) + " km/h", LEFT, 16); 
}


void _altitude()
{
  myGLCD.print("Wysokosc NPM", CENTER, 0);
  myGLCD.print(String(gps.altitude.meters()) + " m", LEFT, 16);  
}

void _gpstime()
{
  myGLCD.print("Czas GPS", CENTER, 0);
  String hours = (gps.time.hour() >=10) ? String(gps.time.hour()) : "0" + String(gps.time.hour()); 
  String minutes = (gps.time.minute() >=10) ? String(gps.time.minute()) : "0" + String(gps.time.minute());
  String seconds = (gps.time.second() >=10) ? String(gps.time.second()) : "0" + String(gps.time.second());
  myGLCD.print(hours + ":" + minutes + ":" + seconds, LEFT, 16); 
  myGLCD.print(String(gps.date.day()) + '.' + String(gps.date.month()) + '.' + String(gps.date.year()), LEFT, 26);   
}

void _distance()
{
  myGLCD.print("Pokonana trasa", CENTER, 0);
  myGLCD.print(String(distance) + " km", LEFT, 16);  
}

void loop()
{
  state = digitalRead(button);
  currentTime = millis();
  if(state == LOW)
  {
    menuItem++;
  }
  if(menuItem > items)
    menuItem = 1;
  while(serial_c.available())
    gps.encode(serial_c.read());

  if((currentTime - lastTime) >= interval) 
  {
    lastTime = currentTime;
    if(gps.location.isUpdated())
    {
      myGLCD.clrScr();
      switch(menuItem)
      {
        case 1:
          _speed();
          break;
        case 2:
          _cords();
          break;
        case 3:
          _altitude();
          break;
        case 4:
          _gpstime();
          break;
        case 5:
          _distance();
          break;
        case 6:
          _sat();
          break;
        default:
          break;
      }
      if(!found)
        found = true;
    }
    if(!found)
    {
      myGLCD.print("Wyszukiwanie", CENTER, 0); 
      myGLCD.print("Czas: " + String(currentTime/interval), CENTER, 16);
    }
    else
      distance += (gps.speed.kmph() * double(1.0/3600.0));
  }
}
