#include <Arduino.h>
#include "Wire.h"
// #include "AudioTools.h"
#include "a2dp/BluetoothA2DPSink.h"
// #include "xfont.h"

// #define TFT_SCLK   22
// #define TFT_MOSI  21
// #define TFT_RST     25
// #define TFT_DC      14
// #define TFT_CS   	 15
#define pin_sda 22
#define pin_scl 19
#define pin_pdn 18

unsigned long myTime;
unsigned long beginTime;

// XFont *_xFont;
BluetoothA2DPSink a2dp_sink;
// Then somewhere in your sketch:
void read_data_stream(const uint8_t *data, uint32_t length)
{
  i2s_write(I2S_NUM_0, data, length, NULL, 0);
}
void rssi(esp_bt_gap_cb_param_t::read_rssi_delta_param &rssiParam)
{
  Serial.print("rssi value: ");
  Serial.println(rssiParam.rssi_delta);
}

String song_singer = "";
String song_album = "";
float totalSongTime = 1.0;
uint32_t pos = 0;

// 显示播放的百分比，其实可以优化的更好
void avrc_rn_play_pos_callback(uint32_t play_pos)
{
  Serial.printf("Play position is %d (%d seconds)\n", play_pos, (int)round(play_pos / 1000.0));
  float per = 0.0;
  if (totalSongTime > 0)
  {
    per = play_pos / totalSongTime * 100;
    // _xFont->tft.fillRect(0, 102, 400, 20, TFT_BLACK);
    // _xFont->DrawChineseEx(0, 102, "" + String(per, 2) + "%", TFT_SILVER, TFT_BLACK);
  }
  if (play_pos == 0)
  {
    // _xFont->tft.fillRect(10, 132, 310, 20, TFT_DARKGREY);
    beginTime = millis();
  }
  else
  {
    int w = (int)309 * (per / 100.0);
    // _xFont->tft.fillRect(11, 133, w, 18, TFT_LIGHTGREY);
  }
  pos = play_pos;
  myTime = millis();
}

void avrc_metadata_callback(uint8_t data1, const uint8_t *data2)
{
  // Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);

  String strData2 = (const char *)data2;

  // if(strData2!="0")Serial.printf("data1: %d data2: %s \n",data1,strData2);
  if (data1 == 2)
  {
    // Serial.printf("歌名：%s", data2);
    if (song_singer != strData2)
    {
      // _xFont->tft.fillRect(0, 2, 400, 40, TFT_BLACK);
      // _xFont->DrawChineseEx(0, 2, "" + strData2, TFT_GREEN, TFT_BLACK);

      song_singer = strData2;
    }
  }
  if (data1 == 4)
  {
    // Serial.printf("歌手：%s", data2);
    if (song_album != strData2)
    {
      // _xFont->tft.fillRect(0, 46, 400, 40, TFT_BLACK);
      // _xFont->DrawChineseEx(0, 46, "" + strData2, TFT_RED, TFT_BLACK);
      song_album = strData2;
    }
  }
  if (data1 == 1)
  {
    // Serial.printf("专辑：%s", data2);
    // _xFont->tft.fillRect(0, 71, 400, 30, TFT_BLACK);

    // _xFont->DrawChineseEx(0, 71, "" + strData2, TFT_LIGHTGREY, TFT_BLACK);
  }
  if (data1 == 64)
    totalSongTime = strData2.toFloat();
  // 0x8 第几首歌
  // 0x10 总共多少首歌
  // 0x20
}

void connection_state_changed(esp_a2d_connection_state_t state, void *ptr)
{
  Serial.println(a2dp_sink.to_str(state));
}

void audio_state_changed(esp_a2d_audio_state_t state, void *ptr)
{
  Serial.println(a2dp_sink.to_str(state));
}

void setup()
{

  myTime = millis();
  beginTime = millis();
  Serial.begin(115200);
  pinMode(pin_pdn, OUTPUT);
  digitalWrite(pin_pdn, LOW);
  Serial.printf("\n请接入12V电源");
  delay(1000);
  Serial.printf("\nPDN上电");
  digitalWrite(pin_pdn, HIGH);
  delay(1);
  
  // 0x58
  // Wire.begin()
  bool ss=Wire.begin(pin_sda,pin_scl);
 
  size_t  ret=0;
  Wire.beginTransmission(0x2c);
    // Serial.println(ret);


  ret=Wire.write(0x02);
  ret=Wire.write(B00000000);
  delay(1);
  // ret=Wire.write(0x03);
  // ret=Wire.write(B00000000);

  // ret=Wire.write(0x0e);
  // ret=Wire.write(B0000011);


  ret=Wire.write(0x04);
  ret=Wire.write(B00000011);
  delay(1);
  ret=Wire.write(0x05);
  ret=Wire.write(B00000000);
  delay(1);
  //   ret=Wire.write(0x0a);
  // ret=Wire.write(B00100001);

  delay(1);
  //   ret=Wire.write(0x0e);
  // ret=Wire.write(B00000011);
// ret=Wire.write(0x12);
byte error;
error = Wire.endTransmission();
Serial.println(error);
      // byte error, address;
      // int nDevices;
      // Serial.println("Scanning...");
      // nDevices = 0;
      // for(address = 1; address < 127; address++ ) {
      //   Wire.beginTransmission(address);
      //   error = Wire.endTransmission();
      //   if (error == 0) {
      //     Serial.print("I2C device found at address 0x");
      //     if (address<16) {
      //       Serial.print("0");
      //     }
      //     Serial.println(address,HEX);
      //     nDevices++;
      //   }
      //   else if (error==4) {
      //     Serial.print("Unknown error at address 0x");
      //     if (address<16) {
      //       Serial.print("0");
      //     }
      //     Serial.println(address,HEX);
      //   }    
      // }
      // if (nDevices == 0) {
      //   Serial.println("No I2C devices foundn");
      // }
      // else {
      //   Serial.println("donen");
      // }
      // delay(5000);  
      // Serial.print("i2c devices found:");
      // Serial.println(nDevices); 

  // _xFont = new XFont(true);

  // _xFont->DrawChineseEx(2, 2, "业精于勤荒于嬉戏", TFT_GREEN);

  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_TRACK_NUM | ESP_AVRC_MD_ATTR_NUM_TRACKS | ESP_AVRC_MD_ATTR_PLAYING_TIME | ESP_AVRC_MD_ATTR_GENRE);
  // a2dp_sink.
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  a2dp_sink.set_on_connection_state_changed(connection_state_changed);
  a2dp_sink.set_on_audio_state_changed(audio_state_changed);
  a2dp_sink.set_avrc_rn_play_pos_callback(avrc_rn_play_pos_callback);
  i2s_pin_config_t my_pin_config = {
      // .mck_io_num=0,
      .bck_io_num = 32,   // 输入
      .ws_io_num = 33,    // 输入
      .data_out_num = 21, // 输出
      .data_in_num = I2S_PIN_NO_CHANGE};
  a2dp_sink.set_pin_config(my_pin_config);
  a2dp_sink.start("ESP S3 Music");
  // _xFont->reInitZhiku("/x_simsun.ttc_24_b64.font");
  // _xFont->DrawChineseEx(2,60, "行成于思毁于随", TFT_GREEN);
}

void loop()
{
  // delay(500);
  // int wasteTime = millis() - beginTime;
  // if (totalSongTime > 0 && pos > 0)
  // {
  //   if (wasteTime > 1000)
  //   {
  //     float p = wasteTime / totalSongTime;
  //     int w = (int)309 * p;
  //     // _xFont->tft.fillRect(0, 102, 400, 20, TFT_BLACK);
  //     // _xFont->DrawChineseEx(0, 102, "" + String(p * 100, 2) + "%", TFT_SILVER, TFT_BLACK);
  //     _xFont->tft.fillRect(11, 133, w, 18, TFT_LIGHTGREY);
  //   }
  // }
}
