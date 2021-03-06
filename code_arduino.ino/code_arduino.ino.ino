/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # 
# RFID MFRC522 / RC522  : https://github.com/miguelbalboa/rfid       # 
#                                                                     # 
#                 cài đặt :                                           # 
# NodeMCU ESP8266/ESP12E    RFID MFRC522 / RC522                      #
#         D2       <---------->   SDA/SS                              #
#         D5       <---------->   SCK                                 #
#         D7       <---------->   MOSI                                #
#         D6       <---------->   MISO                                #
#         GND      <---------->   GND                                 #
#         D1       <---------->   RST                                 #
#         3V/3V3   <---------->   3.3V                                #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

*/

//----------------------------------------thư viện NodeMCU ESP8266 --------------------------------------//
//---------------------------------------- https://www.youtube.com/watch?v=8jMr94B8iN0 để thêm thư viện và mạch NodeMCU ESP8266 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

WiFiClient wifiClient;







//----------------------------------------thư viện SPI and MFRC522 -----------------------------------------------------//
//----------------------------------------tải thư viện MFRC522 / RC522 : https://github.com/miguelbalboa/rfid  -------------------------//
#include <SPI.h>
#include <MFRC522.h>
//-----------------------------------------------------------------------------------------------------------//

#define SS_PIN D2           //--> SDA / SS nối chân D2
#define RST_PIN D1          //--> RST nối chân D1
MFRC522 mfrc522(SS_PIN, RST_PIN);   //--> tạo lập MFRC522 

#define ON_Board_LED 2  //--> Xác định đèn LED On Board, được sử dụng để báo khi quá trình kết nối với bộ định tuyến Wi-Fi




//----------------------------------------thông tin về wi-fi ---------------------------------------------------------//
const char* ssid = "Vusang1303";
const char* password = "13032000";
//-----------------------------------------------------------------//

ESP8266WebServer server(80);        //--> Server cổng 80

int readsuccess;
byte readcard[4];
char str[32] = "";
String StrUID;






//--------------------------------------------------- cài đặt --------------------------------------------------//
void setup() {
  Serial.begin(115200);     //--> tạo giao tiếp với máy tính
  SPI.begin();              //-->  SPI bus
  mfrc522.PCD_Init();       //-->  MFRC522 card

  delay(500);

  WiFi.begin(ssid, password);     //--> kết nối WiFi
  Serial.println("");
    
  pinMode(ON_Board_LED,OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH);   //--> tắt Led On Board



  //---------------------------------------chờ kết nối ---------//
  Serial.print("ĐANG KẾT NỐI");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");


    //--------------------------Làm cho đèn LED nhấp nháy trên bo mạch KHI kết nối với  wifi--------------//
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH);     //--> Tắt led khi kết nối xong
 



 //-----------------Nếu kết nối thành công với  wifi, Địa chỉ IP sẽ được hiển thị trên màn hình --------------//
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Please tag a card or keychain to see the UID !");  //--> yêu cầu nhập thẻ
  Serial.println("");
}
//----------------------------------------------------------------------------------------------------//






//--------------------------------------------LOOP-----------------------------------------//
void loop() {
  // -- main code --- //
  readsuccess = getid();
 
  if(readsuccess) {  
  digitalWrite(ON_Board_LED, LOW);
    HTTPClient http;        //Khai báo đối tượng của HTTPClient//
 
    String UIDresultSend, postData;
    UIDresultSend = StrUID;
   
    // ---- dữ liệu ---- //
    postData = "UIDresult=" + UIDresultSend;
  
    String apiGetData="http://192.168.0.101:80/NodeMCU_RC522_Mysql/getUID.php";   //chỉ định đích đến
    http.begin(wifiClient, apiGetData);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //chỉ định header
   
    int httpCode = http.POST(postData);   //gửi request
    String payload = http.getString();    //nhận phản hồi
  
    Serial.println(UIDresultSend);
    Serial.println(httpCode);   // in mã code http
    Serial.println(payload);    // in phản hồi



    http.end();        // đóng kết nối
    delay(1000);
  digitalWrite(ON_Board_LED, HIGH);
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------//
  



//-------------------------đọc thẻ-----------------------//
int getid() {  
  if(!mfrc522.PICC_IsNewCardPresent()) {
    return 0;
  }
  if(!mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }
 
  Serial.print("THE UID OF THE SCANNED CARD IS : ");
  
  for(int i=0;i<4;i++){
    readcard[i]=mfrc522.uid.uidByte[i];     //lưu trữ UID của thẻ trong thẻ đọc
    array_to_string(readcard, 4, str);
    StrUID = str;
  }
  mfrc522.PICC_HaltA();
  return 1;
}
//--------------------------------------------------------------------------------------------------//




//----------------------------------------thay đổi mảng UID thành chuỗi (array UID to string)---------------------------------------------------//
void array_to_string(byte array[], unsigned int len, char buffer[]) {
    for (unsigned int i = 0; i < len; i++)
    {
        byte nib1 = (array[i] >> 4) & 0x0F;
        byte nib2 = (array[i] >> 0) & 0x0F;
        buffer[i*2+0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
        buffer[i*2+1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
    }
    buffer[len*2] = '\0';
}
//----------------------------------------------------------------------------------//
