#define _GPRMC_ 1
#define _GPGGA_ 2
#define _OTHER_ 3
#define time_offset 3600 // define a clock offset of 3600 seconds (1 hour) ==> UTC + 1

#include <stdint.h>

// LCD module connections
sbit LCD_RS at RD0_bit;
sbit LCD_EN at RD1_bit;
sbit LCD_D4 at RD2_bit;
sbit LCD_D5 at RD3_bit;
sbit LCD_D6 at RD4_bit;
sbit LCD_D7 at RD5_bit;

sbit LCD_RS_Direction at TRISD0_bit;
sbit LCD_EN_Direction at TRISD1_bit;
sbit LCD_D4_Direction at TRISD2_bit;
sbit LCD_D5_Direction at TRISD3_bit;
sbit LCD_D6_Direction at TRISD4_bit;
sbit LCD_D7_Direction at TRISD5_bit;

void stringcpy(char *str1, char *str2);
uint8_t GPSRead();
uint8_t GPSSecond();
uint8_t GPSMinute();
uint8_t GPSHour();
uint8_t GPSDay();
uint8_t GPSMonth();
uint8_t GPSYear();
float Latitude();
float Longitude();
float Latitude();
float Altitude();
uint8_t Satellites();
float Speed();
float Course();
void float_to_str(char *f_str, float f_number);
float str_to_float(char *str_);

int8_t Lat_dir, Lon_dir;
uint8_t char_number, SentenceType = 0, Term, GPRMC_ok = 0, GPGGA_ok = 0;
char sentence[6], rawTime[11], rawDate[7], rawSpeed[6], rawCourse[6], rawSatellites[3],
    rawLatitude[12], rawLongitude[12], rawAltitude[7], buffer[12];

void stringcpy(char *str1, char *str2)
{
  uint8_t chr = 0;
  do
  {
    str2[chr] = str1[chr];
  } while (str1[chr++] != '\0');
}

uint8_t GPSRead()
{
  uint8_t chr = UART1_Read();

  switch (chr)
  {
  case '\r': // sentence end
    if (SentenceType == _GPRMC_)
      GPRMC_ok = 1;
    if (SentenceType == _GPGGA_)
      GPGGA_ok = 1;
    if (GPRMC_ok && GPGGA_ok)
    {
      GPRMC_ok = GPGGA_ok = 0;
      return 1;
    }
    break;

  case '$': // sentence start
    Term = char_number = 0;
    break;

  case ',': // term end (new term start)
    buffer[char_number] = '\0';
    if (Term == 0)
    {
      stringcpy(buffer, sentence);
      if (strcmp(sentence, "GPRMC") == 0)
        SentenceType = _GPRMC_;
      else if (strcmp(sentence, "GPGGA") == 0)
        SentenceType = _GPGGA_;
      else
        SentenceType = _OTHER_;
    }

    // Time
    if (Term == 1 && SentenceType == _GPRMC_)
    {
      stringcpy(buffer, rawTime);
    }

    // Latitude
    if ((Term == 3) && (SentenceType == _GPRMC_))
    {
      stringcpy(buffer, rawLatitude);
    }
    // Latitude N/S
    if ((Term == 4) && (SentenceType == _GPRMC_))
    {
      if (buffer[0] == 'N')
        Lat_dir = 1;
      else
        Lat_dir = -1;
    }

    // Longitude
    if ((Term == 5) && (SentenceType == _GPRMC_))
    {
      stringcpy(buffer, rawLongitude);
    }
    // Longitude E/W
    if ((Term == 6) && (SentenceType == _GPRMC_))
    {
      if (buffer[0] == 'E')
        Lon_dir = 1;
      else
        Lon_dir = -1;
    }

    // Speed
    if ((Term == 7) && (SentenceType == _GPRMC_))
    {
      stringcpy(buffer, rawSpeed);
    }

    // Course
    if ((Term == 8) && (SentenceType == _GPRMC_))
    {
      stringcpy(buffer, rawCourse);
    }

    // Date
    if (Term == 9 && SentenceType == _GPRMC_)
    {
      stringcpy(buffer, rawDate);
    }

    // Satellites
    if ((Term == 7) && (SentenceType == _GPGGA_))
    {
      stringcpy(buffer, rawSatellites);
    }

    // Altitude
    if ((Term == 9) && (SentenceType == _GPGGA_))
    {
      stringcpy(buffer, rawAltitude);
    }
    Term++;
    char_number = 0;
    break;

  default:
    buffer[char_number++] = chr;
    break;
  }

  return 0;
}
uint8_t convert(char i, char j)
{
  if (i >= '0' && i <= '9' && j >= '0' && j <= '9')
    return (i - '0') * 10 + (j - '0');
  else
    return 0;
}
uint8_t GPSSecond()
{
  return convert(rawTime[4], rawTime[5]);
}
uint8_t GPSMinute()
{
  return convert(rawTime[2], rawTime[3]);
}
uint8_t GPSHour()
{
  return convert(rawTime[0], rawTime[1]);
}

uint8_t GPSDay()
{
  return convert(rawDate[0], rawDate[1]);
}
uint8_t GPSMonth()
{
  return convert(rawDate[2], rawDate[3]);
}
uint8_t GPSyear()
{
  return convert(rawDate[4], rawDate[5]);
}

float parse_rawDegree(char *term_)
{
  uint8_t i = 0;
  uint16_t i_part = 0;
  uint32_t multiplier = 1;
  uint32_t float_part = 0;

  while (term_[i] != '.' && term_[i] >= '0' && term_[i] <= '9')
  {
    i_part = i_part * 10 + (term_[i] - '0');
    i += 1;
  }

  i += 1;
  while (term_[i] != '\0' && term_[i] >= '0' && term_[i] <= '9')
  {
    float_part = float_part * 10 + (term_[i] - '0');
    multiplier *= 10;
    i++;
  }
  float_part += multiplier * (i_part % 100);
  i_part /= 100;
  return ((float)float_part / (multiplier * 60)) + i_part;
}

float Latitude()
{
  return (parse_rawDegree(rawLatitude) * Lat_dir);
}

float Longitude()
{
  return (parse_rawDegree(rawLongitude) * Lon_dir);
}

float Altitude()
{
  return str_to_float(rawAltitude);
}

uint8_t Satellites()
{
  uint8_t sat_n = 0, i = 0;
  while (rawSatellites[i] != '\0' && rawSatellites[i] >= '0' && rawSatellites[i] <= '9')
  {
    sat_n = sat_n * 10 + rawSatellites[i] - '0';
    i++;
  }
  return sat_n;
}

float Speed()
{
  return (str_to_float(rawSpeed) * 1.852);
}

float Course()
{
  return str_to_float(rawCourse);
}

float str_to_float(char *str_)
{
  uint8_t i = 0;
  uint16_t i_part = 0;
  uint32_t multiplier = 1;
  uint32_t float_part = 0;

  while (str_[i] != '.' && str_[i] >= '0' && str_[i] <= '9')
  {
    i_part = i_part * 10 + (str_[i] - '0');
    i += 1;
  }

  i += 1;
  while (str_[i] != '\0' && str_[i] >= '0' && str_[i] <= '9')
  {
    float_part = float_part * 10 + (str_[i] - '0');
    multiplier *= 10;
    i++;
  }
  return (float)float_part / multiplier + i_part;
}

// mikroC time library variables
long unix_time;
TimeStruct utc_time, my_time;

// other variables
char txt[3];

char *codetxt_to_ramtxt(const char *ctxt)
{
  static char text_[18];
  char i;
  for (i = 0; text_[i] = ctxt[i]; i++)
    ;
  return text_;
}

void print_wday(short wday)
{
  switch (wday)
  {
  case 0:
    lcd_out(2, 6, codetxt_to_ramtxt(" MONDAY  "));
    break;
  case 1:
    lcd_out(2, 6, codetxt_to_ramtxt(" TUESDAY "));
    break;
  case 2:
    lcd_out(2, 6, codetxt_to_ramtxt("WEDNESDAY"));
    break;
  case 3:
    lcd_out(2, 6, codetxt_to_ramtxt("THURSDAY "));
    break;
  case 4:
    lcd_out(2, 6, codetxt_to_ramtxt(" FRIDAY  "));
    break;
  case 5:
    lcd_out(2, 6, codetxt_to_ramtxt("SATURDAY "));
    break;
  default:
    lcd_out(2, 6, codetxt_to_ramtxt(" SUNDAY  "));
  }
}

// convert byte to string
void byte_to_str(unsigned short nub, char *str)
{
  str[0] = nub / 10 + '0';
  str[1] = nub % 10 + '0';
  str[2] = '\0';
}

void main()
{
  OSCCON = 0X70; // set internal oscillator to 8MHz
  Lcd_Init();
  Lcd_Cmd(_LCD_CURSOR_OFF);
  Lcd_Cmd(_LCD_CLEAR);
  lcd_out(1, 2, codetxt_to_ramtxt("GPS CLOCK (UTC+1)"));
  lcd_out(3, 3, codetxt_to_ramtxt("TIME:   :  :"));
  lcd_out(4, 3, codetxt_to_ramtxt("DATE:   -  -20"));

  UART1_Init(9600);

  while (1)
  {
    if (UART1_Data_Ready() == 1)
      if (GPSRead())
      {
        // read UTC time and date and save them to utc_time structure
        utc_time.hh = GPSHour();   // hour 0-23
        utc_time.mn = GPSMinute(); // minutes 0-59
        utc_time.ss = GPSSecond(); // seconds 0-59

        utc_time.md = GPSDay();         // month day 1-31
        utc_time.mo = GPSMonth();       // month 1-12
        utc_time.yy = GPSYear() + 2000; // four digits year (2018, 2019 ...)

        // get unix time
        unix_time = Time_dateToEpoch(&utc_time);

        // add the offset (in seconds) to the unix time
        unix_time += time_offset;

        // get the local time
        Time_epochToDate(unix_time, &my_time);

        // print day of the week
        print_wday(my_time.wd);
        // print time
        byte_to_str(my_time.hh, txt);
        lcd_out(3, 9, txt);
        byte_to_str(my_time.mn, txt);
        lcd_out(3, 12, txt);
        byte_to_str(my_time.ss, txt);
        lcd_out(3, 15, txt);
        // print date
        byte_to_str(my_time.md, txt);
        lcd_out(4, 9, txt);
        byte_to_str(my_time.mo, txt);
        lcd_out(4, 12, txt);
        byte_to_str((my_time.yy - 2000), txt);
        lcd_out(4, 17, txt);
      }
  }
}