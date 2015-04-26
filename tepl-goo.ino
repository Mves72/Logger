#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <dht11.h> 


#undef int
#undef abs
#undef double
#undef float
#undef round
#define DHT11PIN 8
#define ONE_WIRE_BUS 2


//nastavení čidla DHT 11
dht11 DHT11;


// Setup a oneWire instance to communicate with any OneWire devices 
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
int dp = 2; //pocet dallas cidel

//nastavení sítě    
byte mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };  //Replace with your Ethernet shield MAC
byte ip[] = {192,168,1,177};     // Your Arduino device IP address

//nastavení přístupu k serveru PushInBox
char devid[] = "v69407214FD58BE3";  // THIS IS THE DEVICE ID FROM PUSHINGBOX
int del=30;  // Amount of seconds delay between posting to google docs.
char server[] = "api.pushingbox.com";
EthernetClient client;

 

//char postmsg[100];
int k=0; //kontrola odeslání dat na internet
float temp_av[3]; //průměrná teplota
float avtemp[3]; //pro výpočet pr. teploty
float tempd[2];
int hum_av = 0; //průměrná vlhkost

 
 
void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.println("merim...");
  
  sensors.begin();
}
 
void loop(){
  
  
  
  
  // average temp reading for 'del' time.........................................
 
  for(int j=0; j<del;j++)
  {
    // Read local temp........................................
    int chk = DHT11.read(DHT11PIN);
    int temp = DHT11.temperature;
    int hum = DHT11.humidity;
    temp_av[0]=temp_av[0]+temp;
    hum_av=hum_av+hum;
    
    sensors.requestTemperatures(); // Send the command to get temperatures
   for(int i=0 ;i<dp;i++)
   {
     tempd[i]=sensors.getTempCByIndex(i);
     temp_av[i+1]=temp_av[i+1]+tempd[i];
   } 
    delay(10000);
    Serial.print(j);
    Serial.print(":");
    Serial.print(hum);   // vlhkost
    Serial.print(":");
    Serial.print(temp);
    Serial.println(">");
    for(int i=0 ;i<dp;i++)
    { 
    Serial.print(tempd[i]); //teploty
    Serial.print(":");
    }
    Serial.println("");//odřádkování
  }
  
  int avhum=hum_av/(del);
  hum_av=0;

  for(int i=0 ;i<dp+1;i++)
  {
  avtemp[i]=temp_av[i]/(del);
  temp_av[i]=0;
  }
  
  
  Serial.print("namereno:");
  Serial.print(":");
  Serial.print(avhum);  
  Serial.println(">");
  for(int i=0 ;i<dp+1;i++)
  {
  Serial.println(i);
  Serial.print(avtemp[i],1);
  Serial.print(":");
  }
  Serial.println("");
  Serial.print("Connecting...");  
    
  
 // Post to Google Form.............................................
  if (client.connect(server, 80)) 
  {
    k=0;
    Serial.println("connected");
    Serial.println(devid);
    
    String postmsg;
    postmsg="POST /pushingbox?devid=v69407214FD58BE3&temp=";
    postmsg=postmsg+String(avtemp[0]);
    postmsg=postmsg+"&humi=";
    postmsg=postmsg+String(avhum);
    postmsg=postmsg+"&temp1=";
    postmsg=postmsg+String(avtemp[1],1);
    postmsg=postmsg+"&temp2=";
    postmsg=postmsg+String(avtemp[2],1);
    postmsg=postmsg+" HTTP/1.1";

    //sprintf(postmsg,"GET /pushingbox?devid=%c&status=%d HTTP/1.1",devid,avtemp);  // NOTE** In this line of code you can see where the temperature value is inserted into the wed address. It follows 'status=' Change that value to whatever you want to post.
    client.println(postmsg);
    client.println("Host: api.pushingbox.com");
    client.println("Connection: close");
    client.println();

    Serial.println(postmsg);
    Serial.println("Host: api.pushingbox.com");
    Serial.println("Connection: close");
    Serial.println();
 
     delay(1000);
     client.stop();
  }
  delay(1000);
  
  if (!client.connected()) 
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    k==1;
    return;
  }
  
 
}



