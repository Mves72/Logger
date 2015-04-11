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
dht11 DHT11;
#define DHT11PIN 8
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices 
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

///////////////////////////////
///      EDIT THIS STUFF     //
///////////////////////////////

byte mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };  //Replace with your Ethernet shield MAC
byte ip[] = {192,168,1,177};     // Your Arduino device IP address

char devid[] = "v69407214FD58BE3";  // THIS IS THE DEVICE ID FROM PUSHINGBOX

int del=30;  // Amount of seconds delay between posting to google docs.

///////////////////////////////
//       DONE EDITING        //
///////////////////////////////


 

//char postmsg[100];
int k=0;
int temp_av = 0;
int hum_av = 0;
char server[] = "api.pushingbox.com";
EthernetClient client;
 
 
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
    
    sensors.requestTemperatures(); // Send the command to get temperatures
     
    temp_av=temp_av+temp;
    hum_av=hum_av+hum;
    delay(10000);
    Serial.print(j);
    Serial.print(":");
    Serial.print(temp);
    Serial.print(":");
    Serial.print(hum);
    Serial.print(":");
    Serial.print(sensors.getTempCByIndex(0)); //Dallas 0
    Serial.print(":");
    Serial.println(sensors.getTempCByIndex(1)); //Dallas 1
  }
  
  int avtemp=temp_av/(del);
  int avhum=hum_av/(del);
  hum_av=0;
  temp_av=0;
  Serial.print("namereno:");
  Serial.print(avtemp);
  Serial.print(":");
  Serial.println(avhum);  
  Serial.print("Connecting...");  
    
  
 // Post to Google Form.............................................
  if (client.connect(server, 80)) 
  {
    k=0;
    Serial.println("connected");
    Serial.println(devid);
    
    String postmsg;
    postmsg="POST /pushingbox?devid=v69407214FD58BE3&status=";
    postmsg=postmsg+String(avtemp);
    postmsg=postmsg+"&humi=";
    postmsg=postmsg+String(avhum);
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


double Fahrenheit(double celsius) // Function to convert to Fahrenheit
{
	return 1.8 * celsius + 32;
}

