#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

// conexion al router
const char* ssid = "user_AP";
const char* password = "password_AP";

//server local del esp
ESP8266WebServer server(80);
String webString="";

//***********************************************************

   
   char *url1="http://api.openweathermap.org/data/2.5/weather?";  
   //char *lat1="-37.8814";    //modificar con la latitud de tu ciudad si no usas la api de google en caso de que la geo de google te de error
   //char *lon1="-67.8277";   //modificar con la longitud de tu ciudad si no usas la api de google en caso de que la geo de google te de error
   char *key="ACA VA TU API KEY de OPENWEATHERMAP";   //modificar con tu api_key
   char *url3="&units=metric&lang=es";
   

char *api_openw(char *url1,const char *lat1, const char *lon1, char *key,char*url3){
  char buffer1[1024];
  String response;
   
   char cad1[255]; 
   
   
  strcpy(cad1,"");
  strcpy(cad1,url1);
  strcat(cad1,"lat=");
  strcat(cad1,lat1);
  strcat(cad1,"&lon=");
  strcat(cad1,lon1);
  strcat(cad1,"&APPID=");
  strcat(cad1,key);
  strcat(cad1,url3);
  
  HTTPClient http;
   http.begin(cad1);
   int httpCode = http.GET();
   if(httpCode == HTTP_CODE_OK)
   {
    Serial.print("HTTP codigo de respuesta ");
    Serial.println(httpCode);
    response = http.getString();
    buffer1[response.length() + 1];
    response.toCharArray(buffer1, sizeof(buffer1));     // transforma el string en un array de cacteres usado por ArduinoJson
    buffer1[response.length() + 1] = '\0';
    
    }else{
          Serial.println("Error en recepcion HTTP ");
          }
 
    http.end(); 
   return buffer1;
}


//***********************************************************
char *api_google(){
  char buffer1[1024];
  
  const char* host = "www.googleapis.com";
  const int httpsPort = 443;
  const char* fingerprint = "9C D5 56 DD F9 90 61 9D 26 46 AA 85 09 9A C9 AC 4E B3 72 96"; //https://www.grc.com/fingerprints.htm
  
  String url1="/geolocation/v1/geolocate?key=";
  String api_key="ACA VA TU API de Google";
  String line,line2;
    
  WiFiClientSecure client;
   if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    //return;
   }

   if (client.verify(fingerprint, host)) {
    Serial.println("el certificado coincide");
  } else {
    Serial.println("el certificado no coincide"); 
  }
  
  
  // aca puede funcionar si en cuentra el punto de acceso sino te toma la ip para darte la georeferencia
  //ver refencia d ela api de geolocalizacion de google https://developers.google.com/maps/documentation/geolocation/intro?hl=en_US
  //hay que mejorar esta parte

  String data ="{\"macAddress\": \"D0:FC:CC:A5:7B:F9 \", \"signalStrength\": -43,\"age\": 0,\"channel\": 11,\"signalToNoiseRatio\": 0}";
  Serial.print("Peticion URL: ");
  
    client.print(String("POST ") + url1 + api_key + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Content-Type: application/json\r\n" +
                 "Content-Length: " + data.length() + "\r\n" +
                 "\r\n" + // This is the extra CR+LF pair to signify the start of a body
                 data + "\n");
   
      int waitcount = 0;
   while (!client.available() && waitcount++ < 10000) {
     delay(10);
    }

   /* Serial.println("Response: \n");
    while(client.available()){
     line = client.readStringUntil('\r');
    Serial.print(line);
    }*/
    line=client.readString();
    line2=line.substring(line.indexOf("{"),(line.lastIndexOf("}")+1));
    
    buffer1[line2.length() + 1];
    line2.toCharArray(buffer1, sizeof(buffer1));     // transforma el string en un array de cacteres usado por ArduinoJson
    buffer1[line2.length() + 1] = '\0';
    Serial.println(buffer1);
   return buffer1;
 
}

//***********************************************************
void handle_root() {
    server.send(200, "text/plain", "Estacion metereologica con openweathermap y geolocalizacion de google :");
    delay(100);
    }
//***********************************************************

void setup(){
char cad[5];
   Serial.begin(115200);
   Serial.print("Conectando al WiFi ");
   WiFi.begin(ssid,password);
   while(WiFi.status() != WL_CONNECTED)
   {
     delay(500);
     Serial.print(".");
   }
    Serial.println("Conectado");

server.on("/", handle_root);
server.on("/estacion", [](){

//*******************************************************************
 const size_t bufferSize1 = 2*JSON_OBJECT_SIZE(2) + 70;
DynamicJsonBuffer jsonBuffer1(bufferSize1);

//const char* json = "{\"location\":{\"lat\":-34.6036844,\"lng\":-58.3815591},\"accuracy\":8870}";

JsonObject& root1 = jsonBuffer1.parseObject(api_google());

const char* location_lat = root1["location"]["lat"]; // -34.6036844
const char* location_lng = root1["location"]["lng"]; // -58.3815591

int accuracy = root1["accuracy"]; // 8870
    
 //*************************** webstring 
      webString+="www.googleapis.com";
      webString+="<br>Geolocalizacion Api Google - Catriel RN by SPP";
      webString+="<br>";

      webString+="<br>Latitud: ";  
      webString+=location_lat ;
      webString+="<br>";
      
      webString+="<br>Longitud: ";  
      webString+=location_lng;
      webString+="<br>";
      
      webString+="<br>Exactitud: ";  
      webString+=accuracy;
      webString+=" %";
      webString+="<br>";



    //*****************json api by gen json arduino maker
const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(11) + 410;
   DynamicJsonBuffer jsonBuffer(bufferSize);
   JsonObject& root = jsonBuffer.parseObject(api_openw(url1,location_lat, location_lng,key,url3));
float coord_lon = root["coord"]["lon"]; // -58.54
float coord_lat = root["coord"]["lat"]; // -34.63

JsonObject& weather0 = root["weather"][0];
int weather0_id = weather0["id"]; // 800
const char* weather0_main = weather0["main"]; // "Clear"
const char* weather0_description = weather0["description"]; // "cielo claro"
const char* weather0_icon = weather0["icon"]; // "01n"
const char* base = root["base"]; // "stations"

JsonObject& main = root["main"];
float main_temp = main["temp"]; // 27.07
float main_pressure = main["pressure"]; // 1024.62
int main_humidity = main["humidity"]; // 56
float main_temp_min = main["temp_min"]; // 27.07
float main_temp_max = main["temp_max"]; // 27.07
float main_sea_level = main["sea_level"]; // 1027.32
float main_grnd_level = main["grnd_level"]; // 1024.62
float wind_speed = root["wind"]["speed"]; // 2.66
float wind_deg = root["wind"]["deg"]; // 46.0005
int clouds_all = root["clouds"]["all"]; // 0
long dt = root["dt"]; // 1487635240

JsonObject& sys = root["sys"];
float sys_message = sys["message"]; // 0.002
const char* sys_country = sys["country"]; // "AR"
long sys_sunrise = sys["sunrise"]; // 1487669664
long sys_sunset = sys["sunset"]; // 1487716827
long id = root["id"]; // 3435376
const char* name = root["name"]; 
int cod = root["cod"]; // 200
       //*****************json api 

      //*************************** webstring 
      webString+="<br>";
      webString+="api.openweathermap.org";
      webString+="<br>Estacion Metereologica - Catriel RN by SPP";
      webString+="<br>";

      webString+="<br>Pais: ";  
      webString+=sys_country;
      webString+="<br>";
      
      webString+="<br>Ciudad: ";  
      webString+=name;
      webString+="<br>";
      
      webString+="<br>Longitud: ";  
      webString+=coord_lon;
      webString+="&#176;";
      webString+="<br>";
      
      webString+="<br>Latitud: ";  
      webString+=coord_lat;
      webString+="&#176;";
      webString+="<br>";
      
      webString+="<br>Descripcion del clima: ";  
      webString+=weather0_description;
      webString+="<br>";
      
      webString+="<br>Temperatura: ";  
      webString+=main_temp;
      webString+=" &#176;C";
      webString+="<br>";
      
      webString+="<br>Presion :";  
      webString+=main_pressure;
      webString+=" mBar";
      webString+="<br>";
         
      webString+="<br>Humedad: ";  
      webString+=main_humidity;
      webString+=" %";
      webString+="<br>";
      
      webString+="<br>Nivel del mar: ";  
      webString+=main_sea_level;
      webString+=" mts";
      webString+="<br>";
      
      webString+="<br>Velocidad del viento: ";  
      webString+=wind_speed;
      webString+=" km/h";
      webString+="<br>";
      
  
 
      
     server.send(200, "text/html", webString);
     webString="";     
    });


          //*********************************************
    server.begin();
    Serial.println("HTTP Servidor iniciado by SPP");
    
    
}
//***********************************************************


void loop(){
   
   server.handleClient();
  
  }

