// Pin
int AM1=2; //PD2 
int BM1=3; //PD3
int AIN1=7; 
int AIN2=8;
int PWMA=9; //PB1
int PWMB=10; //PB2
int cur_pwm_pin=9; // default
// Read Rotate Speed
const int d_time=100;//设定单位时间
int flagA=0;
int flagB=0;//标志位设定
int valA=0; //A相脉冲计数
int valB=0; //B相脉冲计数
unsigned long times;
unsigned long newtime;//时间变量

// PID
double P=2;
double I=0.5;
double D=0.2;
double rotate_speed;//存储转速的变量
double target=20; // default target
double error=0;
double pre_error=0;
double integral=0;
double differential=0;
double pwm_val=10;

// Master
char receive_str[20]={};
int str_len=0;
bool target_changed=false;
bool is_clockwise=false;
void serialEvent() 
{
  memset(receive_str, 0, sizeof receive_str);
  int i=0;
  while (Serial.available()) 
  {
    receive_str[i] = Serial.read();
    i++;
  }
  target_changed = true;
}
void PID_control()
{
  pwm_val=P*(error+I*integral+D*differential);
  if(pwm_val>255)
  {
    pwm_val=255;
  }
  else if(pwm_val<0)
  {
    pwm_val=0;
  }
}

void setup()
{
   Serial.begin(115200);//串口初始化
   pinMode(AIN1,OUTPUT);
   pinMode(AIN2,OUTPUT);
   pinMode(PWMA,OUTPUT);//PWMA引脚的输出方式
   pinMode(PWMB,OUTPUT);//PWMB引脚的输出方式
   pinMode(AM1,INPUT);
   pinMode(BM1,INPUT);//AM1、BM2引脚的输入方式
   digitalWrite(PWMB,LOW);
   digitalWrite(PWMA,LOW);
}
/* IN1 IN2 OUT1 OUT2 说明
    0 0 Z Z 滑行，休眠
    1 0 H L 正向
    0 1 L H 反向
    1 1 L L 刹车
*/
void loop()
{
   PID_control();
//   Serial.print("PID output pwm value:");
//   Serial.println(pwm_val);
   analogWrite(cur_pwm_pin,pwm_val);
   newtime=times=millis();
   while((newtime-times)<d_time)
   {
      if(digitalRead(AM1)==HIGH && flagA==0)
      {
        valA++;
        flagA=1;
      }
      if(digitalRead(AM1)==LOW && flagA==1)
      {
        valA++;
        flagA=0;
      }
      if(digitalRead(BM1)==HIGH && flagB==0)
      {
        valB++;
        flagB=1;
      }
      if(digitalRead(BM1)==LOW && flagB==1)
      {
        valB++;
        flagB=0;
      }
      newtime=millis();
    }//计算AB两相的脉冲数
    rotate_speed=(valA+valB)/(1.496*d_time);//计算转速
    if(target_changed == true)
    {
//      Serial.print("receive:");
//      Serial.println(receive_str);
      str_len=strlen(receive_str);
      is_clockwise=receive_str[str_len-1]-'0';
      if(is_clockwise==0){
        digitalWrite(PWMB,LOW);
        cur_pwm_pin=9;
      }
      else{
        digitalWrite(PWMA,LOW);
        cur_pwm_pin=10;
      }
      receive_str[str_len-1]='\0';
      target=strtod(receive_str,NULL);
//      Serial.print("new target:");
//      Serial.print(target);
//      Serial.print(",is clockwise:");
//      Serial.println(is_clockwise);
      target_changed=false;
    }
//    Serial.print("rotate speed:");
    Serial.print(rotate_speed);
//    Serial.println("rad/s");//输出转速数值
    valA=valB=0;//清零储存脉冲数的变量
    pre_error=error;
    error=target-rotate_speed;
    integral+=error;
    differential=error-pre_error;
}
