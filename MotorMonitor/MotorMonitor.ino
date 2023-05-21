#include <Wire.h>
// OLED0.96所需的库
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// 按键消抖
#include <Bounce2.h>
#define SCREEN_WIDTH 128 // 设置OLED宽度,单位:像素
#define SCREEN_HEIGHT 64 // 设置OLED高度,单位:像素
 
// 自定义重置引脚,虽然教程未使用,但却是Adafruit_SSD1306库文件所必需的
#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Bounce2::Button inc_but = Bounce2::Button();
Bounce2::Button dec_but = Bounce2::Button();
Bounce2::Button dir_but = Bounce2::Button();
double target=20;
int is_clockwise=0;
double cur_rot_speed;
char receive_str[20]={};
void serialEvent()
{
  memset(receive_str, 0, sizeof receive_str);
  int i=0;
  while (Serial.available()) 
  {
    receive_str[i] = Serial.read();
    i++;
  }
}
void setup()
{
  // 初始化OLED并设置其IIC地址为 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // 配置按键消抖和引脚模式
  inc_but.attach(9,INPUT);
  dec_but.attach(10,INPUT);
  dir_but.attach(8,INPUT);

  inc_but.interval(10);
  dec_but.interval(10);
  dir_but.interval(10);
  
  inc_but.setPressedState(HIGH);
  dec_but.setPressedState(HIGH);
  dir_but.setPressedState(HIGH);
  // 初始化串口
  Serial.begin(115200);//串口初始化
}

void loop()
{
  inc_but.update();
  dec_but.update();
  dir_but.update();
  if(inc_but.pressed()){
    target+=0.5;
    Serial.print(target);
    Serial.print(is_clockwise);
  }
  if(dec_but.pressed()){
    target-=0.5;
    Serial.print(target);
    Serial.print(is_clockwise);
  }
  if(dir_but.pressed()){
    if(is_clockwise==0) is_clockwise=1;
    else is_clockwise=0;
    Serial.print(target);
    Serial.print(is_clockwise);
  }
  words_display();
  display.display();
}
 
void words_display()
{
  // 清除屏幕
  display.clearDisplay();
 
  // 设置字体颜色,白色可见
  display.setTextColor(WHITE);
 
  //设置字体大小
  display.setTextSize(1.5);
 
  //设置光标位置
  display.setCursor(0, 0);
  display.print("target: ");
  display.print(target);
 
  display.setCursor(0, 20);
  display.print("speed: ");
  display.print(strtod(receive_str,NULL));
 
  display.setCursor(0, 40);
  display.print("dir: ");
  display.print(is_clockwise?"clockwise":"anticlockwise");
}
