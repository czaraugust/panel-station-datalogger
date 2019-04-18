
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 34
#define TEMPERATURE_PRECISION 9 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");
  pinMode(ONE_WIRE_BUS, INPUT);
  // Start up the library
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  // locate devices on the bus
  Serial.print("Locating devices...");

  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		Serial.print("Found device ");
		Serial.print(i, DEC);
		Serial.print(" with address: ");
		printAddress(tempDeviceAddress);
		Serial.println();

		Serial.print("Setting resolution to ");
		Serial.println(TEMPERATURE_PRECISION, DEC);

		// set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);

		Serial.print("Resolution actually set to: ");
		Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
		Serial.println();
	}else{
		Serial.print("Found ghost device at ");
		Serial.print(i, DEC);
		Serial.print(" but could not detect address. Check power and cabling");
	}
  }

}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");


  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	{
		// Output the device ID
		Serial.print("Temperature for device: ");
		Serial.println(i,DEC);

		// It responds almost immediately. Let's print out the data
		printTemperature(tempDeviceAddress); // Use a simple function to print out the data
	}
	//else ghost device! Check your power requirements and cabling

  }
}

// function to print a device address


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
// #include <Arduino.h>
// #include <Wire.h>
// #include <RTClib.h>
// #include <SPI.h>
// #include <SD.h> //INSTALL 161
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>
// #include <Adafruit_ADS1015.h>
// #define SEALEVELPRESSURE_HPA (1012.00)
// void printValues();
// void appendFile(fs::FS &fs, const char * path, const char * message);
// void writeFile(fs::FS &fs, const char * path, const char * message);
//
// //CRIA O OBJETO bme
// Adafruit_BME280 bme; // I2C
// //CRIA O OBJETO RTC
// RTC_DS1307 rtc;
// //CRIA A VARIÁVEL QUE RECEBE O VALOR DE HORA ATUAL
// DateTime now = rtc.now();
// Adafruit_ADS1115 ads1(0x48);  /* Use this for the 16-bit version */
// float_t factor = 0.0078125;;
//
// //CRIA VARIÁVEIS GLOBAIS
// int day;
// int month;
// int year;
// int hour;
// int minutes;
// int seconds;
// int dayOfWeek;
// String weekDay;
// char WeekDays[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
// int anterior =0;
//
//
//
// const int anemoPin = 25;
// const float pi = 3.14159265;           // Numero pi
// int period = 5000;               // Tempo de medida(miliseconds)
// int radius = 147;      // Raio do anemometro(mm)
// unsigned int Sample = 0;   // Sample number
// unsigned int counter = 0; // magnet counter for sensor
// int RPM = 0;          // Revolutions per minute
// float speedwind = 0;           // Wind speed (km/h)
// float vel =0;
// long readTime = 0;
//
//
// const int WDIR = 32;
//
//
// const int PLUV = 33;
// int reedcount = 0;              //This is the variable that hold the count of switching
//
// //const int chipSelect = 17;
//
//
// //==============================================================
// //FUNCAO QUE ATUALIZA AS VARIÁVEIS GLOBAIS E RETORNA A HORA
// //==============================================================
// int get_wind_direction();
//
// void addReedcount(){
//   reedcount++;
//
// }
// void  addcount(){
//   counter++;
// }
//
// String getTime() {
//   //NOW OBEJTO QUE CONTEM A DATA ATUAL
//   now = rtc.now();
//   day = now.day();
//   month = now.month();
//   year = now.year();
//   dayOfWeek = now.dayOfTheWeek();
//   weekDay = WeekDays[dayOfWeek];
//   hour = now.hour();
//   minutes = now.minute();
//   seconds = now.second();
//
//   String time = "";
//   // time = day;
//   // time += "/";
//   // time +=month;
//   // time += "/";
//   // time += year;
//   // time += "/";
//   // time += " ";
//   time += hour;
//   time += ":";
//   time += minutes;
//   time += ":";
//   time += seconds;
//   //time += "\n";
//   // A STRING DATA É: "DIA/MES/ANO HORAS:MINUTOS:SEGUNDOS"
//   return time;
//
// }
// float velocidade(){
//   if(millis() > readTime){
//     RPM=((counter)*60)/(period/1000);  // Calculate revolutions per minute (RPM)
//     counter = 0;
//     speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6;  // Calculate wind speed on km/h
//     vel = speedwind;
//     speedwind = 0;
//     attachInterrupt(digitalPinToInterrupt(anemoPin), addcount, RISING);
//     readTime = millis() + period;
//
//   }
//     return vel/5;
//   }
//
//
//
// String getData(){
// String  temperatura =    String(bme.readTemperature());
// String  pressao     =    String(bme.readPressure() / 100.0F);
// String  umidade     =    String(bme.readHumidity());
// String  irradiancia =    String(ads1.readADC_Differential_2_3()*factor/(1.69/100),5);
// String  direcao     =    String(get_wind_direction());
// //String  velocidadeVento = String()
//
// String data = getTime();
//
//   data += "|";
//   data += temperatura;
//   data += "|";
//   data += pressao;
//   data += "|";
//   data += umidade;
//   data += "|";
//   data += irradiancia;
//   data += "|";
//   //Serial.print("counter: ");Serial.println(counter);
//   RPM=((counter)*60)/(1000/1000);  // Calculate revolutions per minute (RPM)
//   counter = 0;
//   speedwind = (((4 * pi * radius * RPM)/60) / 1000)*3.6;  // Calculate wind speed on km/h
//   data += speedwind/5;
//   speedwind = 0;
//   attachInterrupt(digitalPinToInterrupt(anemoPin), addcount, RISING);
//   data += "|";
//   data +=direcao;
//   data +="|";
//   data += reedcount*0.25;
//
//   data += '\n';
//   return data;
//
// }
//
// int get_wind_direction()
// {
//     unsigned int adc;
//
//     adc = analogRead(WDIR); // get the current reading from the sensor
//
//     // The following table is ADC readings for the wind direction sensor output, sorted from low to high.
//     // Each threshold is the midpoint between adjacent headings. The output is degrees for that ADC reading.
//     // Note that these are not in compass degree order! See Weather Meters datasheet for more information.
//   //  Serial.print("adc: ");Serial.println(adc);
//     //float voltage = ((3.3/4095)*adc);
//     //Serial.print("voltage: ");Serial.println(voltage);
//     if (adc < 320) return (315);
//     if (adc < 400) return (270);
//     if (adc < 470) return (225);
//     if (adc < 600) return (180);
//     if (adc < 720) return (135);
//     if (adc < 1000) return (90);
//     if (adc < 1400) return (45);
//     if (adc < 2000) return (0);
//     return (-1); // error, disconnected?
// }
//
//
//
//
// void setup() {
//
//     Serial.begin(115200);
//     delay(1000);
//     ads1.setGain(GAIN_SIXTEEN);
//     ads1.begin();
//     //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
//     //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//     //rtc.adjust(DateTime(2019, 3, 6, 14, 57, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
//
//     //VERIFICA SE O RTC FOI INICIALIZADO
//     if (! rtc.begin()) { // SE O RTC NÃO FOR INICIALIZADO, FAZ
//      Serial.println("DS1307 não encontrado"); //IMPRIME O TEXTO NO MONITOR SERIAL
//      while(1); //SEMPRE ENTRE NO LOOP
//    }
//    //VERIFICA SE O RTC ESTA FUNCIONANDO
//    if (!rtc.isrunning()) { //SE RTC NÃO ESTIVER SENDO EXECUTADO, FAZ
//     Serial.println("DS1307 not running!!"); //IMPRIME O TEXTO NO MONITOR SERIAL
//      //REMOVA O COMENTÁRIO DE UMA DAS LINHAS ABAIXO PARA INSERIR AS INFORMAÇÕES ATUALIZADAS EM SEU RTC
//      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //CAPTURA A DATA E HORA EM QUE O SKETCH É COMPILADO
//      //rtc.adjust(DateTime(2019, 2, 6, 17, 35, 00)); //(ANO), (MÊS), (DIA), (HORA), (MINUTOS), (SEGUNDOS)
//    }
//    //VERIFICA SE O SD FOI INICIALIZADO
//    if(!SD.begin(17)){
//         Serial.println("Card Mount Failed");
//         return;
//     }
//     uint8_t cardType = SD.cardType();
//
//     if(cardType == CARD_NONE){
//         Serial.println("No SD card attached");
//         return;
//     }
//
//     // INFORMA O TIPO DO CARTAO DE MEMORIA INSERIDO
//     Serial.print("SD Card Type: ");
//     if(cardType == CARD_MMC){
//         Serial.println("MMC");
//     } else if(cardType == CARD_SD){
//         Serial.println("SDSC");
//     } else if(cardType == CARD_SDHC){
//         Serial.println("SDHC");
//     } else {
//         Serial.println("UNKNOWN");
//     }
//
//     bool status = bme.begin(0x76);
//     if (!status) {
//         Serial.println("Could not find a valid BME280 sensor, check wiring!");
//         while (1);
//     }
//
//
//     pinMode(anemoPin, INPUT_PULLUP);
//     pinMode(PLUV, INPUT_PULLUP);
//     attachInterrupt(digitalPinToInterrupt(anemoPin), addcount, RISING);
//     attachInterrupt(digitalPinToInterrupt(PLUV),addReedcount, FALLING);
//
//     readTime = millis();
//
//
//
//     //OBTEM O HORARIO ATUAL
//     getTime();
//     String data = getTime();
//     //CRIA UM ARQUIVO DENTRO DO CARTAO = dia_mes.csv
//     String path = "/";
//     path += day;
//     path += "_";
//     path += month;
//     path += ".csv";
//     //ESCREVE O CABEÇALHO DO ARQUIVO
//     appendFile(SD, path.c_str(), "HORA:MINUTO:SEGUNDO|TEMPERATURA|PRESSAO|UMIDADE|IRRADIANCIA|VELOCIDADE|DIRECAO|CHUVA\n");
//     anterior = seconds;
//
//
// }
//
//
//
// void loop() {
//   attachInterrupt(digitalPinToInterrupt(anemoPin), addcount, RISING);
//   attachInterrupt(digitalPinToInterrupt(PLUV),addReedcount, FALLING);
//   String data = getTime();
//   //ANTERIOR COMEÇA COM ZERO E SE QUANDO A SEGUNDO MUDAR, ELE RECEBE O VALOR DO SEGUNDO ATUALIZADO. EVITA DE USAR DELAY E GARANTE QUE SEMPRE O VALOR DOS SEGUNDOS ATUALIZADOS
//   if (seconds !=  anterior){
//       anterior = seconds;
//       String data = getData();
//       String path = "/";
//       path += day;
//       path += "_";
//       path += month;
//       path += ".csv";
//       get_wind_direction();
//       Serial.print(data);
//       //CONDICAO DE INICIO DE UM DIA = 00 HORAS:00 MINUTOS
//       if (hour ==0 && minutes == 0 && seconds == 0 ){
//         //ESCREVE O CABEÇALHO DO ARQUIVO QUANDO INICIA UM DIA
//          reedcount =0;
//         writeFile(SD, path.c_str(), "HORA:MINUTO:SEGUNDO|TEMPERATURA|PRESSAO|UMIDADE|IRRADIANCIA|VELOCIDADE|DIRECAO|CHUVA\n");
//       }
//       //SE NÃO FOR UM INICIO DE DIA ESCREVE NO FIM
//       appendFile(SD, path.c_str(), data.c_str());
//       }
//
// }
//
// //==============================================================
// // FUNÇÕES DO SD CARD. NÃO ALTERAR
// //==============================================================
// void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
//     Serial.printf("Listing directory: %s\n", dirname);
//
//     File root = fs.open(dirname);
//     if(!root){
//         Serial.println("Failed to open directory");
//         return;
//     }
//     if(!root.isDirectory()){
//         Serial.println("Not a directory");
//         return;
//     }
//
//     File file = root.openNextFile();
//     while(file){
//         if(file.isDirectory()){
//             Serial.print("  DIR : ");
//             Serial.println(file.name());
//             if(levels){
//                 listDir(fs, file.name(), levels -1);
//             }
//         } else {
//             Serial.print("  FILE: ");
//             Serial.print(file.name());
//             Serial.print("  SIZE: ");
//             Serial.println(file.size());
//         }
//         file = root.openNextFile();
//     }
// }
//
// void createDir(fs::FS &fs, const char * path){
//     Serial.printf("Creating Dir: %s\n", path);
//     if(fs.mkdir(path)){
//         Serial.println("Dir created");
//     } else {
//         Serial.println("mkdir failed");
//     }
// }
//
// void removeDir(fs::FS &fs, const char * path){
//     Serial.printf("Removing Dir: %s\n", path);
//     if(fs.rmdir(path)){
//         Serial.println("Dir removed");
//     } else {
//         Serial.println("rmdir failed");
//     }
// }
//
// void readFile(fs::FS &fs, const char * path){
//     Serial.printf("Reading file: %s\n", path);
//
//     File file = fs.open(path);
//     if(!file){
//         Serial.println("Failed to open file for reading");
//         return;
//     }
//
//     Serial.print("Read from file: ");
//     while(file.available()){
//         Serial.write(file.read());
//     }
//     file.close();
// }
//
// void writeFile(fs::FS &fs, const char * path, const char * message){
//     Serial.printf("Writing file: %s\n", path);
//
//     File file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//     if(file.print(message)){
//         Serial.println("File written");
//     } else {
//         Serial.println("Write failed");
//     }
//     file.close();
// }
//
// void appendFile(fs::FS &fs, const char * path, const char * message){
//     //Serial.printf("Appending to file: %s\n", path);
//
//     File file = fs.open(path, FILE_APPEND);
//     if(!file){
//         Serial.println("Failed to open file for appending");
//         return;
//     }
//     if(file.print(message)){
//         //Serial.println("Message appended");
//     } else {
//         Serial.println("Append failed");
//     }
//     file.close();
// }
//
// void renameFile(fs::FS &fs, const char * path1, const char * path2){
//     Serial.printf("Renaming file %s to %s\n", path1, path2);
//     if (fs.rename(path1, path2)) {
//         Serial.println("File renamed");
//     } else {
//         Serial.println("Rename failed");
//     }
// }
//
// void deleteFile(fs::FS &fs, const char * path){
//     Serial.printf("Deleting file: %s\n", path);
//     if(fs.remove(path)){
//         Serial.println("File deleted");
//     } else {
//         Serial.println("Delete failed");
//     }
// }
//
// void testFileIO(fs::FS &fs, const char * path){
//     File file = fs.open(path);
//     static uint8_t buf[512];
//     size_t len = 0;
//     uint32_t start = millis();
//     uint32_t end = start;
//     if(file){
//         len = file.size();
//         size_t flen = len;
//         start = millis();
//         while(len){
//             size_t toRead = len;
//             if(toRead > 512){
//                 toRead = 512;
//             }
//             file.read(buf, toRead);
//             len -= toRead;
//         }
//         end = millis() - start;
//         Serial.printf("%u bytes read for %u ms\n", flen, end);
//         file.close();
//     } else {
//         Serial.println("Failed to open file for reading");
//     }
//
//
//     file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("Failed to open file for writing");
//         return;
//     }
//
//     size_t i;
//     start = millis();
//     for(i=0; i<2048; i++){
//         file.write(buf, 512);
//     }
//
//     end = millis() - start;
//     Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
//     file.close();
// }
