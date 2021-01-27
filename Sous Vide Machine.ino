#include <OneWire.h>
#include <DallasTemperature.h>
double thetemperature=55;//希望達到的溫度
double scontroltemprange=0.3;//使密特比較器的range
double pcontroltemprange=0.4;//P控制的溫度range
int ilowercount=0;
int ihighercount=0;
int icount=60;//積分次數 delay
double dtemp=0.4;//差<pcontroltemprange delay
double lasttemp=20;
int iflag=0;
int start=1;
//int stable=0;
int condition=2;
int laststate=1;//使密特比較器laststate
//double icontrol(double icontrolinputtemp);
//double dcontrol(double dcontrolinputtemp);
//int icontroltotaltemp=0;
//int time0;//每次loop的millis()
//腳位們
int relay_in1_pin=3;
int relay_in2_pin=10;
int ONE_WIRE_BUS1=2;
int ONE_WIRE_BUS2=4;
int ONE_WIRE_BUS3=7;
int ONE_WIRE_BUS4=8;
//溫度們
double smoothtemperature1;
double smoothtemperature2;
double smoothtemperature3;
double smoothtemperature4;
OneWire oneWire1(ONE_WIRE_BUS1);
DallasTemperature sensors1(&oneWire1);
OneWire oneWire2(ONE_WIRE_BUS2);
DallasTemperature sensors2(&oneWire2);
OneWire oneWire3(ONE_WIRE_BUS3);
DallasTemperature sensors3(&oneWire3);
OneWire oneWire4(ONE_WIRE_BUS4);
DallasTemperature sensors4(&oneWire4);
void setup(void)
{
  pinMode(relay_in1_pin,OUTPUT);
  pinMode(relay_in2_pin,OUTPUT);
  pinMode(ONE_WIRE_BUS1,INPUT);
  pinMode(ONE_WIRE_BUS2,INPUT);
  pinMode(ONE_WIRE_BUS3,INPUT);
  pinMode(ONE_WIRE_BUS4,INPUT);  
  Serial.begin(9600);
  Serial.print("theTemperature ");
  Serial.println(thetemperature);
  Serial.print("scontroltemprange=");
  Serial.println (scontroltemprange);
  Serial.print("pcontroltemprange=");
  Serial.println (pcontroltemprange);
  Serial.print("icount=");
  Serial.println(icount);
  Serial.print("dcontroltemprange=");
  Serial.println(dtemp);
  sensors1.begin();
  sensors2.begin();
  sensors3.begin();
  sensors4.begin();
  digitalWrite(relay_in1_pin,HIGH);//斷電初始狀態
  digitalWrite(relay_in2_pin,HIGH);//斷電初始狀態
  
 }
void loop(void)
{
 ///////////Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~");  
  smoothtemperature1=0;
  smoothtemperature2=0;
  smoothtemperature3=0;
  smoothtemperature4=0;
  sensors1.requestTemperatures();
  sensors2.requestTemperatures();
  sensors3.requestTemperatures();  
  sensors4.requestTemperatures();    
 int smooth=10;
  for(int i=0;i<smooth;i++)
  {
     smoothtemperature1+=sensors1.getTempCByIndex(0);
     smoothtemperature2+=sensors2.getTempCByIndex(0);
     smoothtemperature3+=sensors3.getTempCByIndex(0);
     smoothtemperature4+=sensors4.getTempCByIndex(0);
   }
  smoothtemperature1=smoothtemperature1/smooth;
  smoothtemperature2=smoothtemperature2/smooth;
  smoothtemperature3=smoothtemperature3/smooth;
  smoothtemperature4=smoothtemperature4/smooth;
  /////////////Serial.print("sensor1:");
  Serial.println(smoothtemperature1); 
 /* Serial.print("sensor2:");
  Serial.println(smoothtemperature2);
  Serial.print("sensor3:");
  Serial.println(smoothtemperature3);
  Serial.print("sensor4:");
  Serial.println(smoothtemperature4);  
  */
  //if(smoothtemperature1>thetemperature-scontroltemprange&&smoothtemperature1<thetemperature+scontroltemprange)
  //stable=1;
  //------------------------------------------------------------------------dcontrolstart
   if(smoothtemperature1-lasttemp>0&&start==0&&iflag==0&&condition==2)
  {
    
    if(smoothtemperature1-lasttemp>dtemp)
    {
      Serial.println("dworking too hot");
      digitalWrite(relay_in1_pin,HIGH);//斷電
      digitalWrite(relay_in2_pin,HIGH);//斷電
      delay(60000);
    }
  }
    if(smoothtemperature1-lasttemp<0&&start==0&&iflag==0&&condition==2)
  {
  
    if(lasttemp-smoothtemperature1>dtemp)
    {
      Serial.println("dworking too cold");
      digitalWrite(relay_in1_pin,LOW);//通電
      digitalWrite(relay_in2_pin,LOW);//通電
      delay(60000);
    }
  }  
  iflag=0;
  //---------------------------------------------------------------------------dcontrolend 
  if(smoothtemperature1<thetemperature&&start==1)//初始化
  {
    laststate=0;
    start=0;
     ////////////Serial.println("initial");  
 
  } 
  //------------------------------------------------------------------- pcontrol
  if(smoothtemperature1>thetemperature+scontroltemprange&& laststate==0)
  {
   //////////// Serial.println("too hot laststate=0 ");
    condition=2;
     ilowercount=0;
     ihighercount++;    
    if(smoothtemperature1>thetemperature+pcontroltemprange)
    {
      condition=0;//Pcontrol
        ///////////////  Serial.println("fucking too hot laststate=0 ");
    }   
    laststate=1;
  }
    if(smoothtemperature1>thetemperature+scontroltemprange&& laststate==1)
  {
    ///////////////Serial.println("too hot lastestate=1");
    condition=2;
    ilowercount=0;
    ihighercount++; 
    if(smoothtemperature1>thetemperature+pcontroltemprange)
    {
      condition=0;//Pcontrol
       ///////////////// Serial.println("fucking too hot lastestate=1");
    } 
    //laststate=1;
  }
    if(smoothtemperature1<thetemperature-scontroltemprange&& laststate==0)//INITIAL
  {
    ///////////Serial.println(" fucking too cold laststate=0");
    ihighercount=0;
    ilowercount++; 
    condition=4; 
      if(smoothtemperature1>thetemperature-pcontroltemprange)
    {           
      condition=2;//Pcontrol
       ////////////Serial.println("too cold laststate=0");
    } 
  }
   if(smoothtemperature1<thetemperature-scontroltemprange&& laststate==1)
  {
     ////////////Serial.println("fucking too cold laststate=1");
     ilowercount++;
     ihighercount=0;
     condition=4; 
       if(smoothtemperature1>thetemperature-pcontroltemprange)
    {
      
      condition=2;//Pcontrol
      ////////////Serial.println(" too cold laststate =1");
    }
         laststate=0;
  }  
  //-------------------------------------------------------icontrol
  /*
  Serial.print("ilowercount= ");
  Serial.println(ilowercount); 
   Serial.print("ihighercount= ");
  Serial.println(ihighercount); 
  */
  if(ilowercount>icount)
  {
    Serial.println("iworking too cold");
    digitalWrite(relay_in1_pin,LOW);//通電
    digitalWrite(relay_in2_pin,LOW);//通電
    delay(80000);
    ilowercount=0;
    iflag=1;
  }
  if(ihighercount>icount)
  {
    Serial.println("iworking too hot");
    digitalWrite(relay_in1_pin,HIGH);//斷電
    digitalWrite(relay_in2_pin,HIGH);//斷電
    delay(80000);
    ihighercount=0;
    iflag=1;
  }
  //------------------------------------------------------- icontrol
  lasttemp=smoothtemperature1;
  ///////////////////////////////////////////////////////
    //Serial.print("the final condition=");
    //Serial.println(condition);
  if(condition==0)
  {
    //Serial.print("in condition=");
    //Serial.println(condition);
    digitalWrite(relay_in1_pin,HIGH);//斷電
    digitalWrite(relay_in2_pin,HIGH);//斷電
  }
  if(condition==2)
  {
    //Serial.print("in condition=");
    //Serial.println(condition);
    digitalWrite(relay_in1_pin,LOW);//通電
    digitalWrite(relay_in2_pin,HIGH);//斷電
  }
  if(condition==4)
  {
    //Serial.print("in condition=");
    //Serial.println(condition);
    digitalWrite(relay_in1_pin,LOW);//通電
    digitalWrite(relay_in2_pin,LOW);//通電
  }   

  delay(1000);
}



/*
double icontrol(double icontrolinputtemp)
{
int icontroltime1=millis();
int icontroltimevalue;
int icontroltempvalue;  
icontroltotaltemp+=icontrolinputtemp-thetemperature;
if(icontroltotaltemp<0)
  icontroltotaltemp*=-1;
if(icontroltime1-time0<icontroltimevalue)
return 0;
if(icontroltime1-time0>=icontroltimevalue)
{
  if(icontroltotaltemp<icontroltempvalue)
    return 0;
  if(icontroltotaltemp>=icontroltempvalue)
  {
    icontroltotaltemp=0;
    return 123;
  }
} 
}
double dcontrol( double dcontrolinputtemp)
{
  int dcontroltime1=millis();
  int dcontroltimevalue;
  int dcontroltempvalue;  

  return 0;
}
*/

