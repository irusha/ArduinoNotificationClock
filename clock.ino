
#include <LiquidCrystal.h>
#include <DS3231.h>
#include <Wire.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
unsigned long currMillis = 0;
bool isLightOn = false;
String currentNotification = "";
byte lightOnIcon[] = {
    B10101,
    B01110,
    B00000,
    B01110,
    B11001,
    B10011,
    B01110,
    B01110};

byte bluetoothIcon[] = {
    B00100,
    B00110,
    B10101,
    B01110,
    B01110,
    B10101,
    B00110,
    B00100};

byte check[] = {
    B00000,
    B00000,
    B00001,
    B00010,
    B10100,
    B01000,
    B00000,
    B00000};

byte lightOffIcon[] = {
    B00000,
    B01110,
    B10001,
    B10001,
    B10001,
    B01110,
    B01110,
    B00000};

byte noNotifications[] = {
    B10000,
    B00100,
    B01010,
    B01110,
    B01010,
    B11111,
    B00101,
    B00000};

byte cross[] = {
    B00000,
    B00000,
    B10001,
    B01010,
    B00100,
    B01010,
    B10001,
    B00000};

String str = "";
int backlightMode = 0;
bool isRanOnce = true;
DS3231 clock;

bool h12Flag;
bool pmFlag;
bool century = false;
int btpin = 7;

int defaultBrightness = 25;

void setup()
{
  Wire.begin();
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  lcd.createChar(1, lightOnIcon);
  lcd.createChar(3, noNotifications);
  lcd.createChar(4, bluetoothIcon);
  lcd.createChar(2, lightOffIcon);
  lcd.createChar(5, check);
  lcd.createChar(6, cross);

  pinMode(btpin, INPUT);
  pinMode(6, OUTPUT);
  pinMode(13, OUTPUT);
  analogWrite(6, 100);
  pinMode(9, OUTPUT);
  analogWrite(9, 0);
  // initialize the serial communications:
  Serial.begin(115200);
  defaultLcd();
}

void loop()
{
  if (!isRanOnce)
  {
    if (backlightMode % 3 == 0)
    {
      analogWrite(9, defaultBrightness);
    }
    if (millis() - currMillis > 10000)
    {
      defaultLcd();
      isRanOnce = true;
      currMillis = millis();
    }
  }

  else
  {
    if (backlightMode % 3 == 0 || backlightMode % 3 == 1)
    {
      digitalWrite(9, LOW);
    }
    timeDisplay();
  }

  if (Serial.available() > 0)
  {
    str = Serial.readStringUntil('\n');
    currMillis = millis();
    isRanOnce = false;

    if (str.charAt(0) == '#')
    {

      if (str.charAt(1) == 'a')
      {
        changeLight();
      }
      if (str.charAt(1) == 'b')
      {
        backLightModeChanger();
      }
      if (str.charAt(1) == 'd')
      {
        String number = str.substring(2);
        if (number.length() == 10)
        {
          int year = number.substring(0, 2).toInt();
          int month = number.substring(2, 4).toInt();
          int date = number.substring(4, 6).toInt();
          int hour = number.substring(6, 8).toInt();
          int minute = number.substring(8, 10).toInt();
          clock.setYear(year);
          clock.setMonth(month);
          clock.setDate(date);
          clock.setHour(hour);
          clock.setMinute(minute);
          lcd.clear();
          lcd.print("Time set to:");
          lcd.setCursor(0,1);
          lcd.print(numberFormatter(hour));
          lcd.print(":");
          lcd.print(numberFormatter(minute));
          lcd.print(" ");
          lcd.print(numberFormatter(date));
          lcd.print("/");
          lcd.print(numberFormatter(month));
          lcd.print("/");
          lcd.print("20" + numberFormatter(year));
          lcd.print(" ");

        }
      }
      if (str.charAt(1) == 'c')
      {
        String brightnessStr = str.substring(2);
        int brightnessInt = brightnessStr.toInt();
        if (!(brightnessInt <= 0 || brightnessInt > 255))
        {
          defaultBrightness = brightnessInt;
          analogWrite(9, defaultBrightness);
          lcd.clear();
          lcd.print("Brightness:");
          lcd.setCursor(0, 1);
          lcd.print(defaultBrightness);
        }
      }
    }
    else
    {
      currentNotification = str;
      timeDisplay();
      defaultLcd();
    }
  }
}

String numberFormatter(int number)
{
  if (number < 10)
  {
    return "0" + String(number);
  }
  else
  {
    return String(number);
  }
}

void changeLight()
{
  isLightOn = !isLightOn;
  if (isLightOn)
  {
    lcd.clear();
    lcd.print("Room light");
    lcd.setCursor(0, 1);
    lcd.print("On");
    digitalWrite(13, HIGH);
  }

  else
  {
    lcd.clear();
    lcd.print("Room light");
    digitalWrite(13, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Off");
  }
}

void backLightModeChanger()
{
  backlightMode++;
  setBacklight();
}

void setBacklight()
{
  if (backlightMode % 3 == 0)
  {
    lcd.clear();
    lcd.print("Backlight:");
    lcd.setCursor(0, 1);
    lcd.print("Intelligent mode");
  }
  else if (backlightMode % 3 == 1)
  {
    lcd.clear();
    lcd.print("Backlight:");
    lcd.setCursor(0, 1);
    lcd.print("Off");
    digitalWrite(9, LOW);
  }
  else if (backlightMode % 3 == 2)
  {
    lcd.clear();
    lcd.print("Backlight:");
    lcd.setCursor(0, 1);
    lcd.print("On");
    analogWrite(9, defaultBrightness);
  }
}

void defaultLcd()
{
  if (currentNotification == "")
  {

    if (isLightOn)
    {
      lcd.setCursor(0, 1);
      lcd.print("          ");
      lcd.setCursor(10, 1);
      lcd.write(4);
      if (digitalRead(btpin) == HIGH)
      {
        lcd.write(5);
      }
      else
      {
        lcd.write(6);
      }

      lcd.print(' ');
      lcd.write(3);
      lcd.print(' ');
      lcd.write(1);
    }

    else
    {

      lcd.setCursor(0, 1);
      lcd.print("          ");
      lcd.setCursor(10, 1);
      lcd.write(4);
      if (digitalRead(btpin) == HIGH)
      {
        lcd.write(5);
      }
      else
      {
        lcd.write(6);
      }
      lcd.print(' ');
      lcd.write(3);
      lcd.print(' ');
      lcd.write(2);
    }
  }
  else
  {
    lcd.setCursor(0, 1);
    String printString = "";
    if (currentNotification.length() <= 16)
    {
      printString = currentNotification;
      for (int i = 0; i < 16 - currentNotification.length(); i++)
      {
        printString += " ";
      }
    }
    else{
      printString = currentNotification.substring(0,16);
    }
      lcd.print(printString);
  }
}

void timeDisplay()
{
  lcd.setCursor(0, 0);
  int hour = clock.getHour(h12Flag, pmFlag);
  int minute = clock.getMinute();
  int month = clock.getMonth(century);
  int date = clock.getDate();

  lcd.print(numberFormatter(hour));
  lcd.print(":");
  lcd.print(numberFormatter(minute));
  lcd.print(" ");
  lcd.print(numberFormatter(date));
  lcd.print("/");
  lcd.print(numberFormatter(month));
  lcd.print(" ");
  lcd.print(clock.getTemperature(), 0);
  lcd.print((char)223);
  lcd.write('C');
}
