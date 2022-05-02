#include <LiquidCrystal.h>
#include <avr/eeprom.h>;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
int mode;
boolean btn_flag, rain_flag;
unsigned long last_time = 0;
unsigned long timer;
unsigned long temp=0;
struct MyStruct {
  int days;
  int times;
  int dur;
};
MyStruct par;
#define BTN_UP   1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_RIGHT 4
#define BTN_SELECT 5
#define BTN_NONE 10

int detectButton() {
  int keyAnalog =  analogRead(A2);
  if (keyAnalog < 100) {
    // Значение меньше 100 – нажата кнопка right
    return BTN_RIGHT;
  } else if (keyAnalog < 200) {
    // Значение больше 100 (иначе мы бы вошли в предыдущий блок результата сравнения, но меньше 200 – нажата кнопка UP
    return BTN_UP;
  } else if (keyAnalog < 400) {
    // Значение больше 200, но меньше 400 – нажата кнопка DOWN
    return BTN_DOWN;
  } else if (keyAnalog < 600) {
    // Значение больше 400, но меньше 600 – нажата кнопка LEFT
    return BTN_LEFT;
  } else if (keyAnalog < 800) {
    // Значение больше 600, но меньше 800 – нажата кнопка SELECT
    return BTN_SELECT;
  } else {
    // Все остальные значения (до 1023) будут означать, что нажатий не было
    return BTN_NONE;
  }
}
void setup ()
{


  eeprom_read_block((void*)&par,0,sizeof(par));
  mode = 0;
 timer=((unsigned long)par.days*3600*24*1000)/(unsigned long)par.times;
 
  lcd.begin(16, 2);
  pinMode(A6,OUTPUT);
  Serial.begin(9600);
}
void loop()
{
  Serial.println(timer-(millis() -temp));
  printMenu();
//Serial.println(temp);

//настройка mode
  if (detectButton() == BTN_SELECT && btn_flag == 0)
  {
    mode = mode + 1;
    btn_flag = 1;
    if (mode > 3)
    {
      mode = 0;
    }
  }
  if ((millis() - last_time) > 1000)
  {
    last_time = millis();
    btn_flag = 0;
  }


  switch (mode) {
    case 0: 
    clean(4,0);
    clean(13,0);
    clean(8,1);
    break;
    case 1:
      lcd.setCursor(4, 0);
      lcd.print(">");
    clean(13,0);
    clean(8,1);
      break;
    case 2:
      lcd.setCursor(13, 0);
      lcd.print(">");
      clean(4,0);
    clean(8,1);
      break;
    case 3:
      lcd.setCursor(8, 1);
      lcd.print(">");
      clean(4,0);
    clean(13,0);
      break;

  }
  


//настройка days
if (detectButton() == BTN_UP && btn_flag == 0 && mode==1)
  {
    par.days = par.days + 1;
    btn_flag = 1;
    if (par.days > 7)
    {
      par.days = 1;
    }
  }
  if ((millis() - last_time) > 1000)
  {
    last_time = millis();
    btn_flag = 0;
  }
  if (detectButton() == BTN_DOWN && btn_flag == 0 && mode==1)
  {
    par.days = par.days - 1;
    btn_flag = 1;
    if (par.days < 1)
    {
      par.days = 7;
    }
  }
  if ((millis() - last_time) > 1000)
  {
    last_time = millis();
    btn_flag = 0;
  }



//настройка times
  if (detectButton() == BTN_UP && btn_flag == 0 && mode==2)
  {
    par.times = par.times + 1;
    btn_flag = 1;
    if (par.times > 60)
    {
      par.times = 1;
      clean(15,0);
    }
  }
  if ((millis() - last_time) > 1000)
  {
    last_time = millis();
    btn_flag = 0;
  }
  if (detectButton() == BTN_DOWN && btn_flag == 0 && mode==2)
  {
    par.times = par.times - 1;
    btn_flag = 1;
    if (par.times < 1)
    {
      par.times = 60;
    }
    if (par.times < 10)
    {
      clean(15,0);
    }
  }
  if ((millis() - last_time) > 1000)
  {
    last_time = millis();
    btn_flag = 0;
  }




  //настройка dur
  if (detectButton() == BTN_UP && btn_flag == 0 && mode==3)
  {
    par.dur = par.dur + 5;
    btn_flag = 1;
    if (par.dur > 90)
    {
      par.dur = 5;
      clean(10,1);
    }
  }
  if ((millis() - last_time) > 1000)
  {
    last_time = millis();
    btn_flag = 0;
  }
  if (detectButton() == BTN_DOWN && btn_flag == 0 && mode==3)
  {
    par.dur = par.dur - 5;
    btn_flag = 1;
    if (par.dur < 5)
    {
      par.dur = 90;
    }
    if (par.dur < 10)
    {
      clean(10,1);
    }
  }
  if ((millis() - last_time) > 1000)
  {
    last_time = millis();
    btn_flag = 0;
  }




//настройка saved
if(mode==0 && detectButton()==BTN_RIGHT && btn_flag==0)
{
  printSave();
  eeprom_update_block((void*)&par,0,sizeof(par));
  btn_flag=1;
  delay(3000);
    btn_flag = 0;
    cleanAll();
}

//настройка timer
if(mode==0 && detectButton()==BTN_LEFT && btn_flag==0)
{
  printTime();
  btn_flag=1;
  delay(3000);
    btn_flag = 0;
    cleanAll();
}



if((millis() -temp)>timer)
{
 temp=millis();
 rain_flag=1;
}


if((millis() -temp)>(par.dur*1000))
{
  digitalWrite(A6, HIGH);
}
else
{
  digitalWrite(A6, LOW);
}
}
void printMenu()
{
  lcd.setCursor(0, 0);
  lcd.print("days");
  lcd.setCursor(5, 0);
  lcd.print(par.days);
  lcd.setCursor(7, 0);
  lcd.print(" times");
  lcd.setCursor(14, 0);
  lcd.print(par.times);
  lcd.setCursor(0, 1);
  lcd.print("duration");
  lcd.setCursor(9, 1);
  lcd.print(par.dur);

}

void clean(int a, int b)
{
  lcd.setCursor(a, b);
      lcd.print(" ");
}

void printSave()
{
  lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Saved");
}
void cleanAll()
{
  lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
}

void printTime ()
{
  
  unsigned long hour=(timer-(millis()-temp))/(3600000);
  unsigned long minute=(timer-(millis()-temp))/(60000)-(hour*60);
  Serial.print(hour);
  Serial.print(" ");
  Serial.println(minute);
  lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Time until rain");
      lcd.setCursor(0, 1);
      lcd.print("hours:");
      lcd.setCursor(6, 1);
      lcd.print(hour);
      lcd.setCursor(10, 1);
      lcd.print("min");
      lcd.setCursor(14, 1);
      lcd.print(minute);
}
