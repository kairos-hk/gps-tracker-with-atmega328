#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <SD.h>

#define RXPIN 6
#define TXPIN 5
#define GPSBAUD 9600

File myFile;
TinyGPS gps;
SoftwareSerial uart_gps(RXPIN, TXPIN);
void getgps(TinyGPS &gps);

void setup()
{
  Serial.begin(9600);
  uart_gps.begin(GPSBAUD);

  Serial.println("");
  Serial.println("GPS record machine v.1 made by 잗동");
  Serial.println("       ...waiting for lock...           ");
  Serial.println("");

  Serial.print("SD card 연결중...");

  if (!SD.begin(4)) {
    Serial.println("연결실패! 연결이 잘 되었는지 다시 확인해주세요!");
    return;
  }
  Serial.println("연결 성공!");
}

void loop()
{
  while (uart_gps.available())
  {
    int c = uart_gps.read();
    if (gps.encode(c))
    {
      getgps(gps);
    }
  }
}

void getgps(TinyGPS &gps)
{
  float latitude, longitude;

  gps.f_get_position(&latitude, &longitude);
  Serial.print("Lat/Long: ");
  Serial.print(latitude, 5);
  Serial.print(", ");
  Serial.println(longitude, 5);

  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths);

  Serial.print("Date: "); Serial.print(month, DEC); Serial.print("/");
  Serial.print(day, DEC); Serial.print("/"); Serial.print(year);
  Serial.print("  Time: "); Serial.print(hour, DEC); Serial.print(":");
  Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC);
  Serial.print("."); Serial.println(hundredths, DEC);

  Serial.print("Altitude (meters): "); Serial.println(gps.f_altitude());
  Serial.print("Course (degrees): "); Serial.println(gps.f_course());
  Serial.print("Speed(kmph): "); Serial.println(gps.f_speed_kmph());
  Serial.println();

  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);

  writegps(latitude, longitude);

  delay(3000);

}

void writegps(float lati, float longi)
{
  Serial.print("lati/longi: ");
  Serial.print(lati, 5);
  Serial.print(", ");
  Serial.println(longi, 5);
  Serial.println();

  myFile = SD.open("gpstest1.txt", FILE_WRITE);

  if (myFile) {
    Serial.print("Writing to gpstest1.txt...");
    myFile.print("      <trkpt lat=\"");
    myFile.print(lati, 5);
    myFile.print("\" lon=\"");
    myFile.print(longi, 5);
    myFile.println("\">");
    myFile.println("        <ele></ele>");
    myFile.println("        <time></time>");
    myFile.println("      </trkpt>");
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error! sd카드에 gpstest1.txt파일이 없습니다.");
  }

}
