#include <Arduino.h>
#include "a2dp/BluetoothA2DPSink.h"
#include <TFT_eSPI.h>
#include <LittleFS.h>
#include "common.h"

TFT_eSPI tft = TFT_eSPI();
// 下面代码在TFT屏幕输出文字
int pX = 16;
int pY = 0;
int fontsize = 12;      // 字号
int screenWidth = 160;
int amountDisplay = int(screenWidth/fontsize); // 每行显示多少汉字，其实这个显示数量应该通过屏幕的宽度来计算字号


int singleStrPixsAmount = fontsize * fontsize;

BluetoothA2DPSink a2dp_sink;


int seconds = 2;
uint32_t end = 0;

// Then somewhere in your sketch:
void read_data_stream(const uint8_t *data, uint32_t length)
{
  //  String strData2 = (const char *)data;

  // Serial.println(strData2);
  if (end == 0)
  {
    end = millis() + seconds * 1000;
  }
  if (millis() < end)
  {
    // process all data
    // int16_t *values = (int16_t *)data;
    // for (int j = 0; j < length / 2; j += 2)
    // {
    //   // print the 2 channel values
    //   Serial.print(values[j]);
    //   Serial.print(",");
    //   Serial.println(values[j + 1]);
    // }
  }
}
void rssi(esp_bt_gap_cb_param_t::read_rssi_delta_param &rssiParam)
{
  Serial.print("rssi value: ");
  Serial.println(rssiParam.rssi_delta);
}
void data_receive_callback()
{
  // Serial.println("data received.");
}
String song_singer="";
String song_album="";
void avrc_metadata_callback(uint8_t data1, const uint8_t *data2)
{
  // Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);
 
  String strData2 = (const char *)data2;

  Serial.println(strData2);
  if (data1 == 2)
  {
    // Serial.printf("歌名：%s", data2);
     if(song_singer!=strData2){
      tft.fillRect(0,2,400,16,TFT_BLACK);
      DrawStr(0, 2, "" + strData2, TFT_GREEN);
      song_singer=strData2;

     }
    
  }
  if (data1 == 4)
  {
    // Serial.printf("歌手：%s", data2);
    if(song_album!=strData2){
      tft.fillRect(0,18,400,32,TFT_BLACK);
       DrawStr(0, 18, "" + strData2, TFT_RED);
      song_album=strData2;
    }
   
  }
  if (data1 == 1)
  {
    // Serial.printf("专辑：%s", data2);
    tft.fillRect(0,50,400,100,TFT_BLACK);
    DrawStr(0, 50, "" + strData2, TFT_DARKGREEN);
  }
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

  Serial.begin(115200);
  a2dp_sink.set_rssi_active(true);
  a2dp_sink.set_rssi_callback(rssi);
  a2dp_sink.set_on_data_received(data_receive_callback);
  a2dp_sink.set_stream_reader(read_data_stream, false);
  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_TRACK_NUM | ESP_AVRC_MD_ATTR_NUM_TRACKS  );
  // a2dp_sink.
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  a2dp_sink.set_on_connection_state_changed(connection_state_changed);
  a2dp_sink.set_on_audio_state_changed(audio_state_changed);
  a2dp_sink.start("my music");

  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN);
}

void loop()
{
  
}



void DrawStr(int x = 0, int y = 0, String str = "星算", int color = TFT_GREEN)
{

  // 下面的代码显示对应的汉字在TFT屏幕上
  String strBinData = getPixBinStrFromString(str, "/x.font");
  amountDisplay = screenWidth / fontsize; // 如果不愿意动态计算显示数量可以注释调这一行
  for (int i = 0; i < strBinData.length(); i++)
  {

    pX = int(i % fontsize) + int(i / (singleStrPixsAmount)) * fontsize - int(i / (singleStrPixsAmount * amountDisplay)) * fontsize * amountDisplay;

    // 对于pY,每fontsize个像素后+1，每singleStrPixsAmount个像素后归0，同时每换一行，pY要加上fontsize个像素；
    pY = int((i - int(i / singleStrPixsAmount) * singleStrPixsAmount) / fontsize) + int(i / (singleStrPixsAmount * amountDisplay)) * fontsize;

    if (strBinData[i] == '1')
    {
      tft.drawPixel(pX + x, pY + y, color);
    }
    else
    {
      tft.drawPixel(pX + x, pY + y, TFT_BLACK);
    }
  }
}