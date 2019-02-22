#include <TimerOne.h>
//ws2812灯珠的初始化
#include <Adafruit_NeoPixel.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 13
#define NUMPIXELS 60
#define delaytime 16667

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

char s1,s2,s3;//读取蓝牙模块的按键
int divide;//矩阵键盘时钟模式下的时间调节
int pattern=0;//模式转换的初始化

//蓝牙模块初始化
#include <SoftwareSerial.h>
SoftwareSerial BT(8,9);//数字口模拟成串口
char val; //存储接收的变量
int onoff=1;//蓝牙模块实现灯亮灭

//灯的颜色设定
uint32_t Millisecond=strip.Color(200,200,0);
uint32_t Second=strip.Color(0,255,0);
uint32_t Minute=strip.Color(0,0,255);
uint32_t Hourday=strip.Color(255,0,0);
uint32_t Hournight=strip.Color(255,0,255);
uint32_t extinguish=strip.Color(0, 0, 0, 255);
uint32_t Index=strip.Color(20,20,20);
//白天黑夜选择
int daynight=0;

//灯珠初始化
int led[NUMPIXELS];
int m;

//触摸传感器
#define touchPin 3
int distinguish=1;
//蜂鸣器初始化
#define buzzerPin 2
int alarmclock;
//各个时间指针初始化
int millisecond=0;
int second=0,minute=30,hour=35;
int minute2=-1;
//闹钟各个时间指针初始化
int minute1=-1,hour1=-1,daynight1=-1;
int spot1;
//模式间的衔接
int linkup=0;

  
void setup() {
//蜂鸣器信号管脚
  pinMode(buzzerPin, OUTPUT);
//蓝牙模块
   Serial.begin(9600);//与电脑的串口连接
   Serial.println("BT is ready!");
   BT.begin(9600);  //设置波特率
//ws2812灯珠初始化
 for(m=0;m<NUMPIXELS;m++) led[m]=m;

strip.begin();
  strip.show(); 

  Timer1.initialize(delaytime);
   Timer1.attachInterrupt(Count);
 
}

void loop() {  
//模式识别
  Pattern();

//调时间
  if(pattern==0) Timechange(s1,s2);
//装饰模式
 if(pattern==1) Decorate();
 
//触摸传感器
   Distinguish();
   OnOff();
//蓝牙模块
  BlueTooth();


}

void Count(void)
{
  //蓝牙模块
   BlueTooth();
   //模式识别
  Pattern();
  //触摸传感器
  Distinguish();
  OnOff();
  //调时间
  if(pattern==0) Timechange(s1,s2);
  //模式的衔接
  Linkup(pattern,0);
 //毫秒针
      
      if(millisecond!=second&&(distinguish==onoff)) Light(led[millisecond],Millisecond);
     
      if(millisecond>0&&Recognition1(millisecond-1,second,minute,hour)&&Recognition2(millisecond-1,hour-1,hour+1)) Light(led[millisecond-1],extinguish);
      else Light(led[NUMPIXELS-1],extinguish);
      if(millisecond==0) second++;
      if(millisecond<NUMPIXELS) millisecond++;
      if(millisecond==NUMPIXELS) millisecond=0;
      Pattern();
 //秒针
      if(second==NUMPIXELS) {second=0;minute++;}
      if(distinguish==onoff) Light(led[second],Second);
      if(second!=millisecond&&Recognition2(second-1,minute,hour)&&Recognition2(second-1,hour-1,hour+1)&&Recognition2(second-1,NUMPIXELS-1,hour+1))
      {
        if(second<60&&second>0) Light(led[second-1],extinguish);
        else if(second==(NUMPIXELS-1)) Light(led[second-1],extinguish);
        else Light(led[NUMPIXELS-1],extinguish);
      }
      Pattern();  
     
 //分针
     if(minute==NUMPIXELS) {minute=0;hour+=5;}
     if(distinguish==onoff) Light(led[minute],Minute);
     if(second!=minute-1&&Recognition1(minute-1,hour-1,hour,hour+1)&&Recognition2(minute-1,NUMPIXELS-1,hour+1))
     {
      if(minute<(NUMPIXELS-1)&&minute>0)  Light(led[minute-1],extinguish);
      else if(minute==(NUMPIXELS-1)) Light(led[minute-1],extinguish);
      else Light(led[NUMPIXELS-1],extinguish);
     }
     Pattern();  
 //时针
 if(daynight%2==0){
    if(hour==NUMPIXELS) {hour=0;daynight++;}
    if(daynight==2) daynight==0;
    if(distinguish==onoff)
    {
        if(hour>0) Light(led[hour-1],Hourday);
          else Light(led[59],Hourday);
        Light(led[hour],Hourday);
        Light(led[hour+1],Hourday);
        if(hour>=10&&hour<=55) {Light(led[hour-6],extinguish);Light(led[hour-5],extinguish);Light(led[hour-4],extinguish);}
        else if(hour==5) {Light(led[59],extinguish);Light(led[hour-5],extinguish);Light(led[hour-4],extinguish);}
        else {Light(led[54],extinguish);Light(led[55],extinguish);Light(led[56],extinguish);}
    }
 }
 if(daynight%2==1){
    if(hour==NUMPIXELS) {hour=0;daynight++;}
    if(daynight==2) daynight==0;
    if(distinguish==onoff)
    {
        if(hour>0) Light(led[hour-1],Hournight);
          else Light(led[59],Hournight);
        Light(led[hour],Hournight);
        Light(led[hour+1],Hournight);
        if(hour>=10&&hour<=55) {Light(led[hour-6],extinguish);Light(led[hour-5],extinguish);Light(led[hour-4],extinguish);}
        else if(hour==5) {Light(led[59],extinguish);Light(led[hour-5],extinguish);Light(led[hour-4],extinguish);}
        else {Light(led[54],extinguish);Light(led[55],extinguish);Light(led[56],extinguish);}
    }
 }
    if(distinguish==onoff)
    {
     
      for(uint32_t  index=0;index<59;index+=5)
         if(index!=hour&&index!=minute&&index!=second) Light(led[index],Index);
      Light(led[second],Second);
      Light(led[minute],Minute);
      if(millisecond>0) Light(led[millisecond-1],Millisecond);
      if(millisecond==0) Light(led[NUMPIXELS-1],Millisecond);
    }
    Pattern();
    CloseModeOne();strip.show(); 
      CloseModeOne();
       
}
//时钟模式下灯按指定颜色亮
void Light(int w,uint32_t e) {if(pattern==0) strip.setPixelColor(w,e);}

//两种识别
bool Recognition1(int r,int t,int y,int u)
{
  if(r==t||r==y||r==u) return false;
  else return true;
}
bool Recognition2(int r,int t,int y)
{
  if(r==t||r==y) return false;
  else return true;
}
//蓝牙模块
void BlueTooth()
{
  //如果串口接收到数据，就输出到蓝牙串口
   if (Serial.available()) {
    val = Serial.read();
    BT.print(val);
  }
 //如果接收到蓝牙模块的数据，输出到屏幕
  if (BT.available()) {
    val = BT.read();
    Serial.print(val);
  }  
}
//模式调节
void Pattern()
{  
  s1 = BT.read();
   s2 = s1;
  if(s2=='*') {pattern=0;linkup=0;}
  if(s2=='#') {pattern=1;linkup=0;}
}
//调整时间
void Timechange(char s1,char s2)
{
  
  if(s2=='?') divide=1;
  if(s2=='!') divide=2;
  if(s2=='.') divide=3;

  if(divide==1){if(s2=='+') second++;if(s2=='%') second--;if(second==-1) second=59;}
  if(divide==2){if(s2=='+') minute++;if(s2=='%') minute--;if(minute==-1) minute=59;}
  if(divide==3){if(s2=='+') hour+=5;if(s2=='%') hour-=5;if(hour==-5) hour=55;}
  if(divide==2){if('0'<=s2&&s2<='9') minute2=ChartoInt(s2)*10;}
  
    
}

//char改为int
int ChartoInt(char s1)
{
  if(s1=='0') return 0;
  if(s1=='1') return 1;
  if(s1=='2') return 2;
  if(s1=='3') return 3;
  if(s1=='4') return 4;
  if(s1=='5') return 5;
  if(s1=='6') return 6;
  if(s1=='7') return 7;
  if(s1=='8') return 8;
  if(s1=='9') return 9;
}
//触摸传感器判断
void Distinguish()
{
  if(digitalRead(touchPin)==LOW) distinguish=1;
  else distinguish=0; 
}
//蓝牙模块开关灯
void OnOff()
{
  s3=BT.read();
  if(s3=='@') onoff=1;
  if(s3=='&') onoff=0;
}
//所有灯不亮
void Close()
{
 for(uint32_t off=0;off<NUMPIXELS;off++) strip.setPixelColor(off,0);
}
//关闭模式一下的灯
void CloseModeOne()
{
  if(distinguish!=onoff) Close();
}
//关闭模式二下的灯
void CloseLed()
{
  if(distinguish!=onoff) {pattern=0;linkup=0;}
}

//模式的衔接
void Linkup(int pattern,int thispattern)
{
  if(pattern==thispattern&&linkup==0) {Close();linkup=1;}
}
//装饰模式
void Decorate()
{
   //模式的衔接
   Linkup(pattern,1);
   // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
  colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue
  theaterChase(strip.Color(0, 127, 0), 50); //Green
  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

//装饰模式调用函数
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    Pattern();Distinguish(); OnOff();CloseLed();
    if(pattern==0) break;
    if(pattern==1&&distinguish==onoff) strip.setPixelColor(i, c);
    strip.show();CloseLed();
    if(pattern==1&&distinguish==onoff) delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
   if(pattern==0) break;
    for(i=0; i<strip.numPixels(); i++) {
      Pattern();Distinguish(); OnOff();CloseLed();
      if(pattern==0) break;
      if(pattern==1&&distinguish==onoff) strip.setPixelColor(i, Wheel((i+j) & 255)); 
    }
    strip.show();CloseLed();
    if(pattern==1&&distinguish==onoff) delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
   if(pattern==0) break;
    for(i=0; i< strip.numPixels(); i++) {
     Pattern();Distinguish(); OnOff();CloseLed();
     if(pattern==0) break;
      if(pattern==1&&distinguish==onoff) strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();CloseLed();
    if(pattern==1&&distinguish==onoff) delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) { //do 10 cycles of chasing
    
    if(pattern==0) break;
    for (int q=0; q < 3; q++) { 
      
      if(pattern==0) break;
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        Pattern();Distinguish(); OnOff();CloseLed();
        if(pattern==0) break;
        if(pattern==1&&distinguish==onoff) strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();CloseLed();

      if(pattern==1&&distinguish==onoff) delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        Pattern();Distinguish(); OnOff();CloseLed();
        if(pattern==0) break;
        if(pattern==1&&distinguish==onoff) strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {  // cycle all 256 colors in the wheel
    
    if(pattern==0) break;
    for (int q=0; q < 3; q++) {
     
      if(pattern==0) break;
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        Pattern();Distinguish(); OnOff();CloseLed();
        if(pattern==0) break;
        if(pattern==1&&distinguish==onoff) strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();CloseLed();

      if(pattern==1) delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
       Pattern();Distinguish(); OnOff();CloseLed();
       if(pattern==0) break;
       if(pattern==1&&distinguish==onoff) strip.setPixelColor(i+q, 0);//turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

