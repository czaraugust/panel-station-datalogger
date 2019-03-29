//==========================================================
// TODAS AS LIBS DEVEM SER INSTALADAS PELO PLATFORMIO
//==========================================================
#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h> //INSTALL 161
#include <Adafruit_ADS1015.h>

//CRIA O OBJETO RTC
RTC_DS1307 rtc;

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
//const int chipSelect = 17;
int anterior =0;

//CRIA A VARIÁVEL QUE RECEBE O VALOR DE HORA ATUAL
DateTime now = rtc.now();

//==============================================================
//FUNCAO QUE ATUALIZA AS VARIÁVEIS GLOBAIS E RETORNA A HORA
//==============================================================

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
  time = day;
  time += "/";
  time +=month;
  time += "/";
  time += year;
  time += "/";
  time += " ";
  time += hour;
  time += ":";
  time += minutes;
  time += ":";
  time += seconds;
  time += "\n";
  // A STRING DATA É: "DIA/MES/ANO HORAS:MINUTOS:SEGUNDOS"
  return time;

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
void setup() {

    Serial.begin(115200);
    delay(1000);
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
    appendFile(SD, path.c_str(), " HORA:MINUTO:SEGUNDO \n");
    anterior = seconds;


}

void loop() {


  // RECEBE A STRING DATA
  String data = getTime();
  //ANTERIOR COMEÇA COM ZERO E SE QUANDO A SEGUNDO MUDAR, ELE RECEBE O VALOR DO SEGUNDO ATUALIZADO. EVITA DE USAR DELAY E GARANTE QUE SEMPRE O VALOR DOS SEGUNDOS ATUALIZADOS
  if (seconds !=  anterior){
      anterior = seconds;
      String data = getTime();
      String path = "/";
      path += day;
      path += "_";
      path += month;
      path += ".csv";
      //CONDICAO DE INICIO DE UM DIA = 00 HORAS:00 MINUTOS
      if (hour ==0 && minutes == 0 && seconds == 0 ){
        //ESCREVE O CABEÇALHO DO ARQUIVO QUANDO INICIA UM DIA
        writeFile(SD, path.c_str(), "HORA:MINUTO:SEGUNDO \n");
      }
      //SE NÃO FOR UM INICIO DE DIA ESCREVE NO FIM
      appendFile(SD, path.c_str(), data.c_str());
      Serial.print(data);

    }

}


//=============================================================================
//CODIGO PARA VERIFICAR ESTADO DO I2C. SERÁ UTIL MAIS PARA FRENTE, NÃO SE PREOCUPAR AGORA
//=============================================================================


/* I2C slave Address Scanner
for 5V bus
 * Connect a 4.7k resistor between SDA and Vcc
 * Connect a 4.7k resistor between SCL and Vcc
for 3.3V bus
 * Connect a 2.4k resistor between SDA and Vcc
 * Connect a 2.4k resistor between SCL and Vcc

 */
// #include <Arduino.h>
// #include <Wire.h>
//
// void scan(){
// Serial.println(" Scanning I2C Addresses");
// uint8_t cnt=0;
// for(uint8_t i=0;i<128;i++){
//   Wire.beginTransmission(i);
//   uint8_t ec=Wire.endTransmission(true);
//   if(ec==0){
//     if(i<16)Serial.print('0');
//     Serial.print(i,HEX);
//     cnt++;
//   }
//   else Serial.print("..");
//   Serial.print(' ');
//   if ((i&0x0f)==0x0f)Serial.println();
//   }
// Serial.print("Scan Completed, ");
// Serial.print(cnt);
// Serial.println(" I2C Devices found.");
// }
//
// bool i2cReady(uint8_t adr){
// uint32_t timeout=millis();
// bool ready=false;
// while((millis()-timeout<100)&&(!ready)){
// 	Wire.beginTransmission(adr);
// 	ready=(Wire.endTransmission()==0);
// 	}
// return ready;
// }
//
// void eepromSize(){
// Serial.println("Discovering eeprom sizes 0x50..0x57");
// uint8_t adr=0x50,i;
// uint16_t size;
// char buf[64];
// while(adr<0x58){
// 	i=0;
// 	size = 0x1000; // Start at 4k
// 	i += sprintf_P(&buf[i],PSTR("0x%02X: "),adr);
// 	if(i2cReady(adr)) { // EEPROM answered
// 		uint8_t zeroByte;
// 		Wire.beginTransmission(adr);
// 		Wire.write((uint8_t)0); // set address ptr to 0, two bytes High
// 		Wire.write((uint8_t)0); // set address ptr to 0, two bytes Low
// 		uint8_t err=Wire.endTransmission();
// 		if(err==0){// worked
// 		  err=Wire.requestFrom(adr,(uint8_t)1);
// 			if(err==1){// got the value of the byte at address 0
// 				zeroByte=Wire.read();
// 				uint8_t saveByte,testByte;
// 				do{
// 					if(i2cReady(adr)){
// 						Wire.beginTransmission(adr);
// 						Wire.write(highByte(size)); // set next test address
// 						Wire.write(lowByte(size));
// 						Wire.endTransmission();
// 						err=Wire.requestFrom(adr,(uint8_t)1);
// 						if(err==1){
// 							saveByte=Wire.read();
// 							Wire.beginTransmission(adr);
// 							Wire.write(highByte(size)); // set next test address
// 							Wire.write(lowByte(size));
// 							Wire.write((uint8_t)~zeroByte); // change it
// 							err=Wire.endTransmission();
// 							if(err==0){ // changed it
// 								if(!i2cReady(adr)){
// 									i+=sprintf_P(&buf[i],PSTR(" notReady2.\n"));
// 									Serial.print(buf);
// 									adr++;
// 									break;
// 									}
// 								Wire.beginTransmission(adr);
// 								Wire.write((uint8_t)0); // address 0 byte High
// 								Wire.write((uint8_t)0); // address 0 byte Low
// 								err=Wire.endTransmission();
// 								if(err==0){
// 									err=Wire.requestFrom(adr,(uint8_t)1);
// 									if(err==1){ // now compare it
// 									  testByte=Wire.read();
// 										}
// 									else {
// 										testByte=~zeroByte; // error out
// 										}
// 									}
// 								else {
// 									testByte=~zeroByte;
// 									}
// 								}
// 							else {
// 								testByte = ~zeroByte;
// 								}
// 							//restore byte
// 							if(!i2cReady(adr)){
// 								i+=sprintf_P(&buf[i],PSTR(" notReady4.\n"));
// 								Serial.print(buf);
// 								adr++;
// 								break;
// 								}
//
// 							Wire.beginTransmission(adr);
// 							Wire.write(highByte(size)); // set next test address
// 							Wire.write(lowByte(size));
// 							Wire.write((uint8_t)saveByte); // restore it
// 							Wire.endTransmission();
// 							}
// 						else testByte=~zeroByte;
// 						}
// 					else testByte=~zeroByte;
// 					if(testByte==zeroByte){
// 						size = size <<1;
// 						}
// 					}while((testByte==zeroByte)&&(size>0));
// 				if(size==0) i += sprintf_P(&buf[i],PSTR("64k Bytes"));
// 				else i+=sprintf_P(&buf[i],PSTR("%dk Bytes"),size/1024);
// 				if(!i2cReady(adr)){
// 					i+=sprintf_P(&buf[i],PSTR(" notReady3.\n"));
// 					Serial.print(buf);
// 					adr++;
// 					continue;
// 					}
// 				Wire.beginTransmission(adr);
// 				Wire.write((uint8_t)0); // set address ptr to 0, two bytes High
// 				Wire.write((uint8_t)0); // set address ptr to 0, two bytes Low
// 				Wire.write(zeroByte);  //Restore
// 				err=Wire.endTransmission();
// 				}
// 			else i+=sprintf_P(&buf[i],PSTR("Read 0 Failure"));
// 			}
// 		else i+=sprintf_P(&buf[i],PSTR("Write Adr 0 Failure"));
//
// 	  }
// 	else i+=sprintf_P(&buf[i],PSTR("Not Present."));
// 	Serial.println(buf);
// 	adr++;
// 	}
// }
//
// void setup(){
// Serial.begin(115200);
// Wire.begin();
// scan();
// Serial.println();
// eepromSize();
// }
//
// void loop(){}
