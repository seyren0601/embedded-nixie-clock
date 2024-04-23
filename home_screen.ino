#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <TouchScreen.h>
//#include <ThreeWire.h>
#include <DS3231.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


#define MINPRESSURE 200
#define MAXPRESSURE 1000
// TouchScreen
const int XP=6,XM=A2,YP=A1,YM=7; //320x480 ID=0x6814
const int TS_LEFT=929,TS_RT=188,TS_TOP=971,TS_BOT=143;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin
#define LCD_CS A3   // Chip Select goes to Analog 3
#define LCD_CD A2  // Command/Data goes to Analog 2
#define LCD_WR A1  // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410

MCUFRIEND_kbv tft;
// RTC
// CONNECTIONS:
// DSA = DSA (20) 
// DCL = DCL (21)
// VCC = 5v
// GRD = GRD
DS3231 rtc(20, 21);

SoftwareSerial mp3Serial(51, 50);  // RX, TX
DFRobotDFPlayerMini mp3;

extern const uint16_t MusicPlayerButton[];
int musicPlayerButtonXSize = 70;
int musicPlayerButtonYSize = 70;
int musicPlayerButtonX = 120 - (musicPlayerButtonXSize / 2);
int musicPlayerButtonY = 240 - (musicPlayerButtonYSize / 2);
extern const uint16_t AlarmButton[];
int alarmButtonXSize = 70;
int alarmButtonYSize = 70;
int alarmButtonX = 360 - (alarmButtonXSize / 2);
int alarmButtonY = 240 - (alarmButtonXSize / 2);
extern const uint16_t ButtonPlay[];
int playButtonX = 218;
int playButtonY = 79;
int playButtonXSize = 60;
int playButtonYSize = 61;
extern const uint16_t ButtonPause[];
int pauseButtonX = 218;
int pauseButtonY = 79;
int pauseButtonXSize = 60;
int pauseButtonYSize = 60;
extern const uint16_t PreviousButton[];
int previousButtonX = 145;
int previousButtonY = 97;
int previousButtonXSize = 40;
int previousButtonYSize = 39;
extern const uint16_t NextButton[];
int nextButtonX = 307;
int nextButtonY = 97;
int nextButtonXSize = 40;
int nextButtonYSize = 40;
extern const uint16_t VolumeDown[];
int volumeDownX = 150;
int volumeDownY = 177;
int volumeDownXSize = 15;
int volumeDownYSize = 22;
extern const uint16_t VolumeUp[];
int volumeUpX = 341;
int volumeUpY = 175;
int volumeUpXSize = 30;
int volumeUpYSize = 24;


int pixel_x, pixel_y;  //Touch_getXY() updates global vars
char currentPage, playStatus;
int iV = 5;
int trackNum = 1, totalTracks;
int b = 16;
int aHours = 0;
int aMinutes = 0;
boolean alarmNotSet = true;
String alarmString = "";
float currentTemperature, temperature;
static word totalTime, elapsedTime, playback, minutes, seconds, lastSeconds, minutesR, secondsR;
String currentClock, currentHours, currentMinutes, currentSeconds, currentDate;
String timeString, hoursString, minutesString, secondsString, hoursS, minutesS, secondsS, dateS;

int secondX = 335;
int secondY = 100;
int minuteX = 185;
int minuteY = 100;
int hourX = 35;
int hourY = 100;

void setup() {
// Initial setup
  Serial.begin(9600);

  // Music
  mp3Serial.begin(9600); // Begin soft serial with 9600 baud
  mp3.begin(mp3Serial, true, true);
  delay(800);
  mp3.volume(iV);
  delay(100);
  mp3.readFileCounts();
  totalTracks = mp3.readFileCounts();

  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(22, 55, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(22, 1, 2024);   // Set the date to January 9st, 2014

  // MCUFRIEND_kbv
  tft.begin(0x6814); //0x9486 0x6814
  tft.setRotation(1);
  tft.setTextSize(5);
  tft.fillScreen(BLACK);
  tft.setTextColor(GREEN);

    // Draws the Home Screen

  drawHomeScreen();
  currentPage = '0'; // Indicates that we are at Home Screen
  playStatus = '0';

  currentTemperature = rtc.getTemp();
  currentDate = rtc.getDateStr();
  currentClock = rtc.getTimeStr();
  timeString = rtc.getTimeStr();
  currentHours = timeString.substring(0, 2);
  currentMinutes = timeString.substring(2, 4);
  currentSeconds = timeString.substring(4, 6);
}

void loop(){
  bool down = Touch_getXY();
   if (currentPage == '0') {
    // Checks for change of the clock
    if ( currentClock != rtc.getTimeStr()) {
      tft.setTextSize(10);
      timeString = rtc.getTimeStr();
      hoursS = timeString.substring(0, 2);
      minutesS = timeString.substring(3, 5);
      secondsS = timeString.substring(6, 8);

      tft.setCursor(secondX, secondY);
      tft.setTextColor(BLACK);
      tft.print(currentSeconds);
      tft.setCursor(secondX, secondY);
      tft.setTextColor(GREEN);
      tft.print(secondsS);
      currentSeconds = secondsS;
      
      if ( currentMinutes != minutesS ) {
        tft.setCursor(minuteX, minuteY);
        tft.setTextColor(BLACK);
        tft.print(currentMinutes);
        tft.setCursor(minuteX, minuteY);
        tft.setTextColor(GREEN);
        tft.print(minutesS);
        currentMinutes = minutesS;
      }
      if ( currentHours != hoursS ) {
        tft.setCursor(hourX, hourY);
        tft.setTextColor(BLACK);
        tft.print(currentHours);
        tft.setCursor(hourX, hourY);
        tft.setTextColor(GREEN);
        tft.print(hoursS);
        currentHours = hoursS;
      }
      // Checks for change of the date
      dateS = rtc.getDateStr();
      delay(10);
      if ( currentDate != dateS){
          tft.setTextSize(3);
          tft.setCursor(153, 7);
          tft.setTextColor(BLACK);
          tft.print(currentDate);
          tft.setCursor(153, 7);
          tft.setTextColor(WHITE);
          tft.print(rtc.getDateStr());
          currentDate = dateS;
      }
      currentClock = rtc.getTimeStr();
    }

    // Music player button touch
      if((pixel_x >= musicPlayerButtonX) && (pixel_x <= musicPlayerButtonX + musicPlayerButtonXSize) && (pixel_y >= musicPlayerButtonY) && (pixel_y <= musicPlayerButtonY + musicPlayerButtonYSize) && down){
        tft.drawCircle(musicPlayerButtonX + (musicPlayerButtonXSize/2), musicPlayerButtonY + (musicPlayerButtonYSize/2), musicPlayerButtonXSize / 2, RED);
        currentPage = '1';
        delay(1000);
        tft.fillScreen(BLACK);
        drawMusicPlayerScreen();
        delay(100);
      }
    
      if((pixel_x >= alarmButtonX) && (pixel_x <= alarmButtonX + alarmButtonXSize) && (pixel_y >= alarmButtonY) && (pixel_y <= alarmButtonY + alarmButtonYSize)){
        tft.drawCircle(alarmButtonX + (alarmButtonXSize / 2), alarmButtonY + (alarmButtonYSize / 2), alarmButtonXSize / 2, RED);
        currentPage = '2';
        tft.fillScreen(BLACK);
      }
  }

  if (currentPage == '1') {
    // Update clock on player screen
    if ( currentClock != rtc.getTimeStr()){
      tft.setCursor(230, 7);
      tft.setTextColor(BLACK);
      tft.print(currentClock);
      currentClock = rtc.getTimeStr();
      tft.setTextColor(WHITE);
      tft.setCursor(230, 7);
      tft.print(currentClock);
    }
    // If we press the Play Button
    if ((pixel_x >= playButtonX) && (pixel_x <= playButtonX + playButtonXSize) && (pixel_y >= playButtonY) && (pixel_y <= playButtonY + playButtonYSize) && down) {
      if (playStatus == '0') {
        //drawFrame(159, 121, 42);
        drawPauseButton();
        mp3.loop(1);
        trackNum = 1;
        ToExcelTrack();
        delay(100);
        playStatus = '2';
        return;
      }
      if (playStatus == '1') {
        //drawFrame(159, 121, 42);
        drawPauseButton();
        ToExcelTrack();
        mp3.start();
        delay(100);
        playStatus = '2';
        return;
      }
      if (playStatus == '2') {
        //drawFrame(159, 121, 42);
        drawPlayButton();
        mp3.pause();
        delay(100);
        playStatus = '1';
        return;
      }
      delay(100);
    }
    // If we press the Previous Button
    if ((pixel_x >= previousButtonX) && (pixel_x <= previousButtonX + previousButtonXSize) && (pixel_y >= previousButtonY) && (pixel_y <= previousButtonY + previousButtonYSize) && down) {
      //drawFrame(70, 121, 26);
      if(trackNum > 1){
        mp3.previous();
        trackNum--;
        ToExcelTrack();
        delay(100);
      }
      delay(100);
      //drawTrackBar();
    }
    // If we press the Next Button
    if ((pixel_x >= nextButtonX) && (pixel_x <= nextButtonX + nextButtonXSize) && (pixel_y >= nextButtonY) && (pixel_y <= nextButtonY + nextButtonYSize) && down) {
      //drawFrame(252, 122, 26);
      if(trackNum < totalTracks){
        mp3.next();
        trackNum++;
        ToExcelTrack();
        delay(100);
      }
      delay(100);
      //drawTrackBar();
    }
      // If we press the VolumeDown Button
    if ((pixel_x >= volumeDownX) && (pixel_x <= volumeDownX + volumeDownXSize) && (pixel_y >= volumeDownY) && (pixel_y <= volumeDownY + volumeDownYSize) && down) {
      //drawUnderline(45, 205, 65, 205);
      if (iV >= 1) {
        iV--;
        drawVolume(iV);
      }
      mp3.volume(iV);
      ToExcelVolume();
      delay(100);
    }
    // If we press the VolumeUp Button
    if ((pixel_x >= volumeUpX) && (pixel_x <= volumeUpX + volumeUpXSize) && (pixel_y >= volumeUpY) && (pixel_y <= volumeUpY + volumeUpYSize) && down) {
      //drawUnderline(235, 205, 275, 205);
      if (iV <= 29) {
        iV++;
        drawVolume(iV);
      }
      mp3.volume(iV);
      ToExcelVolume();
      delay(100);
    }
    // If we press the MENU Button
    if ((pixel_x >= 0) && (pixel_x <= 115) && (pixel_y >= 0) && (pixel_y <= 30) && down) {
      tft.fillScreen(BLACK);
      drawHomeScreen();  // Draws the Home Screen
      currentPage = '0';
      delay(100);
      return;
    }
    delay(100);
  }

  if (currentPage == '2') {
    tft.setTextSize(5);
    tft.setTextColor(WHITE);
    tft.setCursor(5,5);
    tft.print("MENU");
    tft.setCursor(115, 45);
    tft.print("Set Alarm");
    
    // Draws a colon between the hours and the minutes
    tft.fillCircle (138, 110, 4, GREEN);
    tft.fillCircle (138, 125, 4, GREEN);
    tft.setTextColor(GREEN);
    tft.setCursor(62, 100);
    if(aHours < 10){
      tft.print("0");
      tft.print(aHours);
    }
    else { tft.print(aHours); }
    tft.setCursor(158, 100);
    if(aMinutes < 10){
      tft.print("0");
      tft.print(aMinutes);
    }
    else { tft.print(aMinutes); }

    tft.drawRoundRect(65, 150, 50, 50, 10, WHITE);
    tft.drawRoundRect(160, 150, 50, 50, 10, WHITE);

    tft.setTextColor(WHITE);
    tft.setCursor(77, 160);
    tft.print("H");
    tft.setCursor(172, 160);
    tft.print("M");
    
    tft.setTextSize(3);
    tft.drawRoundRect(300, 100, 120, 50, 10, WHITE);
    tft.setCursor(337, 112);
    tft.print("SET");
    tft.drawRoundRect (300, 160, 120, 50, 10, WHITE);
    tft.setCursor(319, 172);
    tft.print("CLEAR");
    
    alarmNotSet = true;
        
    while (alarmNotSet){
      bool down = Touch_getXY();
        //Set hours button
        if ((pixel_x >= 65) && (pixel_x <= 115) && (pixel_y >= 150) && (pixel_y <= 200) && down) {
          //drawRectFrame(42, 115, 82, 145);
          tft.setTextSize(5);
          tft.setTextColor(BLACK);
          tft.setCursor(62, 100);
          if(aHours < 10){
            tft.print("0");
            tft.print(aHours);
          }
          else { tft.print(aHours); }
          aHours++;
          if(aHours >=24){
            aHours = 0;
          }
          tft.setTextColor(GREEN);
          tft.setCursor(62, 100);
          if(aHours < 10){
            tft.print("0");
            tft.print(aHours);
          }
          else { tft.print(aHours); }
          delay(100);
        }
        // Set minutes buttons
        if ((pixel_x >= 160) && (pixel_x <= 210) && (pixel_y >= 150) && (pixel_y <= 200) && down) {
          //drawRectFrame(138, 115, 178, 145);
          tft.setTextColor(BLACK);
          tft.setTextSize(5);
          tft.setCursor(158, 100);
          if(aMinutes < 10){
            tft.print("0");
            tft.print(aMinutes);
          }
          else { tft.print(aMinutes); }
          aMinutes++;
          if(aMinutes >=60){
            aMinutes = 0;
          }
          tft.setTextColor(GREEN);
          tft.setTextSize(5);
          tft.setCursor(158, 100);
          if(aMinutes < 10){
            tft.print("0");
            tft.print(aMinutes);
          }
          else { tft.print(aMinutes); }
          delay(100);
      }

      // Set alarm button
      //300, 100, 120, 50
      if ((pixel_x >= 300) && (pixel_x <= 420) && (pixel_y >= 100) && (pixel_y <= 150) && down) {
        //drawRectFrame(215, 60, 303, 90);
        if (aHours < 10 && aMinutes < 10){
          alarmString = "0" + (String)aHours + ":" + "0"+ (String)aMinutes + ":" + "00";
        }
        else if (aHours < 10 && aMinutes > 9){
          alarmString = "0" + (String)aHours + ":" + (String)aMinutes + ":" + "00";
        }
        else if (aHours > 9 && aMinutes < 10){
          alarmString = (String)aHours + ":" + "0"+ (String)aMinutes + ":" + "00";
        }
        else {
          alarmString = (String)aHours + ":" + (String)aMinutes + ":" + "00";
        }
        tft.setTextSize(3);
        tft.setTextColor(WHITE);
        tft.setCursor(30, 230);
        tft.print("Alarm set for: ");
        tft.print(alarmString);
      }

      // Clear alarm button
      //300, 160, 120, 50
      if ((pixel_x >= 300) && (pixel_x <= 420) && (pixel_y >= 160) && (pixel_y <= 210) && down) {
        //drawRectFrame(215, 115, 303, 145);
        tft.setTextSize(3);
        tft.setTextColor(BLACK);
        tft.setCursor(30, 230);
        tft.print("Alarm set for: ");
        tft.print(alarmString);
        alarmString = "";
      }

      // If we press the MENU Button
      if ((pixel_x >= 0) && (pixel_x <= 115) && (pixel_y >= 0) && (pixel_y <= 30) && down) {
        alarmNotSet = false;
        currentPage = '0';
        tft.fillScreen(BLACK);
        drawHomeScreen();  // Draws the Home Screen
      }
    }
  }
  if (alarmNotSet == false) {
    if (alarmString == rtc.getTimeStr()){
      tft.fillScreen(BLACK);
      mp3.volume(25);
      mp3.loop(1);
      delay(100);
      tft.setTextSize(5);
      tft.setTextColor(WHITE);
      tft.setCursor(180, 10);
      tft.print("ALARM");
      tft.drawRGBBitmap(127, 60, AlarmButton, alarmButtonXSize, alarmButtonYSize);
      tft.setCursor(50, 140);
      tft.print(alarmString);
      tft.drawRoundRect (100, 200, 200, 50, 10, WHITE);
      tft.setCursor(100, 200);
      tft.print("DISMISS");
      boolean alarmOn = true;
      while (alarmOn){
        bool down = Touch_getXY();
        // Stop alarm button
        if ((pixel_x >= 100) && (pixel_x <= 100 + 200) && (pixel_y >= 200) && (pixel_y <= 200 + 50) && down) {
          //drawRectFrame(94, 146, 226, 170);
          alarmOn = false;
          alarmString="";
          tft.fillScreen(BLACK);
          mp3.pause();
          delay(100);
          currentPage = '0';
          playStatus = '0';
          mp3.volume(15);
          drawHomeScreen();
        }
      }
    }
  }
}

void drawColon(){
  tft.fillCircle(162, 120, 8, WHITE);
  tft.fillCircle(162, 145, 8, WHITE);
  tft.fillCircle(314, 120, 8, WHITE);
  tft.fillCircle(314, 145, 8, WHITE);
}

void drawHomeScreen() {
  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.setCursor(153, 7);
  tft.print(rtc.getDateStr());
  tft.setTextColor(GREEN);
  drawMusicPlayerButton();
  drawAlarmButton();
  drawHomeClock();
}

void drawMusicPlayerScreen(){
  tft.setTextSize(5);
  tft.setTextColor(WHITE);
  tft.setCursor(5,5);
  tft.print("MENU");

  tft.setCursor(100, 215);
  tft.setTextSize(3);
  tft.print("by Seyren Windsor");
  tft.setTextSize(5);

  drawVolume(iV);
  //tft.fillRect(178, 184, 150, 8, WHITE);
  //tft.fillRect(178, 184, 25, 8, YELLOW);

  // Add clock in place of track back
  currentClock = rtc.getTimeStr();
  tft.setTextColor(GREEN);
  tft.setCursor(230, 7);
  tft.print(currentClock);

  drawPlayButton();
  if(playStatus == '2'){
    drawPauseButton();
  }

  drawPreviousButton();
  drawNextButton();
  drawVolumeUp();
  drawVolumeDown();
}

void drawMusicPlayerButton(){
  tft.drawRGBBitmap(musicPlayerButtonX, musicPlayerButtonY, MusicPlayerButton, musicPlayerButtonXSize, musicPlayerButtonYSize);
}

void drawAlarmButton(){
  tft.drawRGBBitmap(alarmButtonX, alarmButtonY, AlarmButton, alarmButtonXSize, alarmButtonYSize);
}

void drawPlayButton(){
  tft.drawRGBBitmap(playButtonX, playButtonY, ButtonPlay, playButtonXSize, playButtonYSize);
}

void drawPauseButton(){
  tft.drawRGBBitmap(playButtonX, playButtonY, ButtonPause, playButtonXSize, playButtonYSize);
}

void drawPreviousButton(){
  tft.drawRGBBitmap(previousButtonX, previousButtonY, PreviousButton, previousButtonXSize, previousButtonYSize);
}

void drawNextButton(){
  tft.drawRGBBitmap(nextButtonX, nextButtonY, NextButton, nextButtonXSize, nextButtonYSize);
}

void drawVolumeUp(){
  tft.drawRGBBitmap(volumeUpX, volumeUpY, VolumeUp, volumeUpXSize, volumeUpYSize);
}

void drawVolumeDown(){
  tft.drawRGBBitmap(volumeDownX, volumeDownY, VolumeDown, volumeDownXSize, volumeDownYSize);
}

void drawHomeClock(){
  timeString = rtc.getTimeStr();
  currentHours = timeString.substring(0, 2);
  currentMinutes = timeString.substring(2, 4);
  currentSeconds = timeString.substring(4, 6);
  tft.setTextSize(10);
  tft.setCursor(secondX, secondY);
  tft.print(currentSeconds);

  tft.setCursor(minuteX, minuteY);
  tft.print(currentMinutes);

  tft.setCursor(hourX, hourY);
  tft.print(currentHours);
  drawColon();
}

void drawTrackBar() {
  tft.setTextColor(WHITE);
  tft.fillRect(48, 50, 224, 8, WHITE);
}

void drawVolume(int x) {
  tft.fillRect(178 + (5 * x), 184, 150 - (5 * x), 8, WHITE);
  tft.fillRect(178, 184, 5 * x, 8, YELLOW);
}

bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
      //x = map(p.y, LEFT=971, RT=143, 0, 480)
      //y = map(p.x, TOP=188, BOT=929, 0, 320)
        pixel_x = map(p.y, 971, 143, 0, 480);
        pixel_y = map(p.x, 188, 929, 0, 320);
    }
    return pressed;
}

void ToExcelTrack(){
  Serial.print(currentClock);
  Serial.print(",");
  Serial.print(trackNum);
  Serial.print(",");
  Serial.print(totalTracks);
  Serial.print(",");
  Serial.println(iV);
}

void ToExcelVolume(){
  Serial.print(currentClock);
  Serial.print(",,,");
  Serial.println(iV);
}

void trackPlayTime() {
  /*totalTime = 0;
  delay(10);
  elapsedTime = 0;
  delay(10);
  minutes = (int)elapsedTime / 60;
  seconds = (((float)elapsedTime / 60) - minutes) * 60;
  playback = totalTime - elapsedTime;
  minutesR = (int)playback / 60;
  secondsR = (((float)playback / 60) - minutesR) * 60;

  tft.setTextSize(3);
  tft.setTextColor(WHITE);
  tft.print(minutes);
  //myGLCD.printNumI(minutes, 8, 48);

  tft.print(":", 16, 48);
  myGLCD.printNumI((int)seconds, 24, 48, 2, '0');

  myGLCD.print("-", 276, 48);
  myGLCD.printNumI(minutesR, 284, 48);
  myGLCD.print(":", 292, 48);
  myGLCD.printNumI((int)secondsR, 300, 48, 2, '0');

  int trackBarX = map(elapsedTime, 0, totalTime, 0, 224);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect (48, 50, 48 + trackBarX, 50 + 8);

  if (totalTime == elapsedTime) {
    mp3.nextTrack();
    delay(30);
    myGLCD.setColor(255, 255, 255);
    myGLCD.fillRect (48, 50, 48 + 224, 50 + 8);
  }*/
}