//
// //==========================================================
// // TODAS AS LIBS DEVEM SER INSTALADAS PELO PLATFORMIO
// //==========================================================
//
// //==========================================================================
// //                              MAPA I2C
// //BME =    76
// //RTC =    68
// //ADC's =  48 49 4A 4B
// //SENSOR DE TEMPERATURA = ?
// //==========================================================================
//
// //==========================================================================
// //                              MAPA SPI
// //SD= 17
// //
// //==========================================================================
//
//
// //===========================================================================
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h> //INSTALL 161
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_ADS1015.h>
#include "OneWire.h"
#include "DallasTemperature.h"
#define SEALEVELPRESSURE_HPA (1012.00)
void printValues();
float getTemp(int sensor);
void appendFile(fs::FS &fs, const char * path, const char * message);
void writeFile(fs::FS &fs, const char * path, const char * message);

//CRIA O OBJETO bme
Adafruit_BME280 bme; // I2C
//CRIA O OBJETO RTC
RTC_DS1307 rtc;
//CRIA A VARIÁVEL QUE RECEBE O VALOR DE HORA ATUAL
DateTime now = rtc.now();
Adafruit_ADS1115 ads1(0x48);  /* Use this for the 16-bit version */
Adafruit_ADS1115 ads2(0x49);  /* Use this for the 16-bit version */
Adafruit_ADS1115 ads3(0x4A);  /* Use this for the 16-bit version */
Adafruit_ADS1115 ads4(0x4B);  /* Use this for the 16-bit version */
float_t factor2 = 0.18750;
float_t factor = 0.0078125;

//CRIA VARIÁVEIS GLOBAIS
int day;
int month;
int year;
int hour;
int minutes;
int seconds;
int dayOfWeek;
String weekDay;
char WeekDays[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
int anterior =0;



const int anemoPin = 25;
const float pi = 3.14159265;           // Numero pi
int period = 5000;               // Tempo de medida(miliseconds)
int radius = 147;      // Raio do anemometro(mm)
unsigned int Sample = 0;   // Sample number
unsigned int counter = 0; // magnet counter for sensor
int RPM = 0;          // Revolutions per minute
float speedwind = 0;           // Wind speed (km/h)
float vel =0;
long readTime = 0;


const int WDIR = 32;


const int PLUV = 33;
int reedcount = 0;              //This is the variable that hold the count of switching

//const int chipSelect = 17;
#define ONE_WIRE_BUS 16
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
DeviceAddress addtempsensor = {0x28, 0xFF, 0x3E, 0xC4, 0xB4, 0x16, 0x03, 0xDF};
//==============================================================
//FUNCAO QUE ATUALIZA AS VARIÁVEIS GLOBAIS E RETORNA A HORA
//==============================================================
int get_wind_direction();

void addReedcount(){
  reedcount++;

}
void  addcount(){
  counter++;
}

String getTime() {
  //NOW OBEJTO QUE CONTEM A DATA ATUAL
  now = rtc.now();
  day = now.day();
  month = now.month();
  year = now.year();
  dayOfWeek = now.dayOfTheWeek();
  weekDay = WeekDays[dayOfWeek];
  hour = now.hour();
  minutes = now.minute();
  seconds = now.second();

  String time = "";
  // time = day;
  // time += "/";
  // time +=month;
  // time += "/";
  // time += year;
  // time += "/";
  // time += " ";
  time += hour;
  time += ":";
  time += minutes;
  time += ":";
  time += seconds;
  //time += "\n";
  // A STRING DATA É: "DIA/MES/ANO HORAS:MINUTOS:SEGUNDOS"
  return time;

}
String getData(){
String  temperatura =    String(bme.readTemperature());
String  pressao     =    String(bme.readPressure() / 100.0F);
String  umidade     =    String(bme.readHumidity());
String  irradiancia =    String(ads1.readADC_Differential_2_3()*factor/(1.69/100),5);
String  direcao     =    String(get_wind_direction());
tempSensor.requestTemperaturesByAddress(addtempsensor);
String temp_panel   =    String(tempSensor.getTempC(addtempsensor));
String  tensao =   String((-1)*ads3.readADC_Differential_0_1() * 200.00000*0.18750/1000.00000,5);
String corrente = String(ads4.readADC_Differential_2_3()*0.18750*3.00000/1000.00000,5 );//* 2.00000*factor2,5
// Serial.println(String(ads3.readADC_Differential_0_1() *0.1875/1000));
//
// Serial.println(String(ads4.readADC_Differential_2_3() *0.1875/1000));


String data = getTime();

  data += "|";
  data += temperatura;
  data += "|";
  data += pressao;
  data += "|";
  data += umidade;
  data += "|";
  data += irradiancia;
  data += "|";
  //Serial.print("counter: ");Serial.println(counter);
  RPM=((counter)*60)/(1000/1000);  // Calculate revolutions per minute (RPM)
  counter = 0;
  speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6;  // Calculate wind speed on km/h
  data += speedwind/5;
  speedwind = 0;
  attachInterrupt(digitalPinToInterrupt(anemoPin), addcount, RISING);
  data += "|";
  data +=direcao;
  data +="|";
  data += reedcount*0.25;
  data += "|";
  data +=temp_panel;
  data  += "|";
  data += tensao;
  data += "|";
  data += corrente;
  data += '\n';
  //Serial.println(tempSensor.getTempC(0));
  return data;

}

int get_wind_direction()
{
    unsigned int adc;

    adc = analogRead(WDIR); // get the current reading from the sensor

    // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
    // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
    // Note that these are not in compass degree order! See Weather Meters datasheet for more information.
  //  Serial.print("adc: ");Serial.println(adc);
    //float voltage = ((3.3/4095)*adc);
    //Serial.print("voltage: ");Serial.println(voltage);
    if (adc < 320) return (315);
    if (adc < 400) return (270);
    if (adc < 470) return (225);
    if (adc < 600) return (180);
    if (adc < 720) return (135);
    if (adc < 1000) return (90);
    if (adc < 1400) return (45);
    if (adc < 2000) return (0);
    return (-1); // error, disconnected?
}



void printAddress(DeviceAddress deviceAddress)

{

  for (uint8_t i = 0; i < 8; i++)

  {

    if (deviceAddress[i] < 16) Serial.print("0");

    Serial.print(deviceAddress[i], HEX);

  }

}
int numberOfDevices = 1;
DeviceAddress tempDeviceAddress;

void getDeviceAddress(void) {
  byte i;
  byte addr[8];

  Serial.println("Getting the address...\n\r");
  /* initiate a search for the OneWire object we created and read its value into
  addr array we declared above*/

  while(oneWire.search(addr)) {
    Serial.print("The address is:\t");
    //read each byte in the address array
    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      // print each byte in the address array in hex format
      Serial.print(addr[i], HEX);
      if (i < 7) {
        Serial.print(", ");
      }
    }
    // a check to make sure that what we read is correct.
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!\n");
        return;
    }
  }
  oneWire.reset_search();
  return;
}
///28 FF 3E C4 B4 16 03 DF



void setup() {

    Serial.begin(115200);
    delay(1000);
    //getDeviceAddress();
    tempSensor.setResolution(addtempsensor,9);




    ads1.setGain(GAIN_SIXTEEN);
    ads1.begin();
    ads2.setGain(GAIN_TWOTHIRDS);
    ads2.begin();
    ads3.setGain(GAIN_TWOTHIRDS);
    ads3.begin();
    ads4.setGain(GAIN_TWOTHIRDS);
    ads4.begin();

    //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2019, 3, 6, 14, 57, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)

    //VERIFICA SE O RTC FOI INICIALIZADO
    if (! rtc.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ
     Serial.println("DS1307 não encontrado"); //IMPRIME O TEXTO NO MONITOR SERIAL
     while(1); //SEMPRE ENTRE NO LOOP
   }
   //VERIFICA SE O RTC ESTA FUNCIONANDO
   if (!rtc.isrunning()) { //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
    Serial.println("DS1307 not running!!"); //IMPRIME O TEXTO NO MONITOR SERIAL
     //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
     //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
     //rtc.adjust(DateTime(2019, 2, 6, 17, 35, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
   }
   //VERIFICA SE O SD FOI INICIALIZADO
   if(!SD.begin(17)){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    // INFORMA O TIPO DO CARTAO DE MEMORIA INSERIDO
    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    bool status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }


    pinMode(anemoPin, INPUT_PULLUP);
    pinMode(PLUV, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(anemoPin), addcount, RISING);
    attachInterrupt(digitalPinToInterrupt(PLUV),addReedcount, FALLING);

    readTime = millis();



    //OBTEM O HORARIO ATUAL
    getTime();
    String data = getTime();
    //CRIA UM ARQUIVO DENTRO DO CARTAO = dia_mes.csv
    String path = "/";
    path += day;
    path += "_";
    path += month;
    path += ".csv";
    //ESCREVE O CABEÇALHO DO ARQUIVO
    appendFile(SD, path.c_str(), "HORA:MINUTO:SEGUNDO|TEMPERATURA|PRESSAO|UMIDADE|IRRADIANCIA|VELOCIDADE|DIRECAO|CHUVA|TEMP_PAINEL|TENSAO|CORRENTE\n");
    anterior = seconds;
    Serial.println("FIM DO SETUP");


}



void loop(){
  attachInterrupt(digitalPinToInterrupt(anemoPin), addcount, RISING);
  attachInterrupt(digitalPinToInterrupt(PLUV),addReedcount, FALLING);
  String data = getTime();
  //ANTERIOR COMEÇA COM ZERO E SE QUANDO A SEGUNDO MUDAR, ELE RECEBE O VALOR DO SEGUNDO ATUALIZADO. EVITA DE USAR DELAY E GARANTE QUE SEMPRE O VALOR DOS SEGUNDOS ATUALIZADOS

  if (seconds !=  anterior){
      anterior = seconds;
      String data = getData();
      String path = "/";
      path += day;
      path += "_";
      path += month;
      path += ".csv";
      Serial.print(data);
      //CONDICAO DE INICIO DE UM DIA = 00 HORAS:00 MINUTOS
      if (hour ==0 && minutes == 0 && seconds == 0 ){
        //ESCREVE O CABEÇALHO DO ARQUIVO QUANDO INICIA UM DIA
         reedcount =0;

        writeFile(SD, path.c_str(), "HORA:MINUTO:SEGUNDO|TEMPERATURA|PRESSAO|UMIDADE|IRRADIANCIA|VELOCIDADE|DIRECAO|CHUVA|TEMP_PAINEL|TENSAO|CORRENTE\n");
      }
      //SE NÃO FOR UM INICIO DE DIA ESCREVE NO FIM
      appendFile(SD, path.c_str(), data.c_str());
      }

}

//==============================================================
// FUNÇÕES DO SD CARD. NÃO ALTERAR
//==============================================================
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        //Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }

    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
