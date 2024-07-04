#include <Arduino.h>
// #include "AudioTools.h"
#include "a2dp/BluetoothA2DPSink.h"
#include "xfont.h"


// #define TFT_SCLK   22
// #define TFT_MOSI  21
// #define TFT_RST     25
// #define TFT_DC      14
// #define TFT_CS   	 15


XFont *_xFont;
BluetoothA2DPSink a2dp_sink;
// Then somewhere in your sketch:
void read_data_stream(const uint8_t *data, uint32_t length)
{
  i2s_write(I2S_NUM_0,data,length,NULL,0);
}
void rssi(esp_bt_gap_cb_param_t::read_rssi_delta_param &rssiParam)
{
  Serial.print("rssi value: ");
  Serial.println(rssiParam.rssi_delta);
}

String song_singer = "";
String song_album = "";
float totalSongTime=1.0;

// 显示播放的百分比，其实可以优化的更好
void avrc_rn_play_pos_callback(uint32_t play_pos) {
  Serial.printf("Play position is %d (%d seconds)\n", play_pos, (int)round(play_pos/1000.0));
  if(totalSongTime>0){
    float per=play_pos/totalSongTime*100;
    _xFont->tft.fillRect(0, 102, 400, 40, TFT_BLACK);
    _xFont->DrawChineseEx(0, 102, "" + String(per,2)+"%", TFT_SILVER,TFT_BLACK);
  }
}

void avrc_metadata_callback(uint8_t data1, const uint8_t *data2)
{
  // Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);

  String strData2 = (const char *)data2;

  if(strData2!="0")Serial.printf("data1: %d data2: %s \n",data1,strData2);
  if (data1 == 2)
  {
    // Serial.printf("歌名：%s", data2);
    if (song_singer != strData2)
    {
      _xFont->tft.fillRect(0, 2, 400, 40, TFT_BLACK);
      _xFont->DrawChineseEx(0, 2, "" + strData2,TFT_GREEN,TFT_BLACK);
     
      song_singer = strData2;
    }
  }
  if (data1 == 4)
  {
    // Serial.printf("歌手：%s", data2);
    if (song_album != strData2)
    {
      _xFont->tft.fillRect(0, 46, 400, 40, TFT_BLACK);
      _xFont->DrawChineseEx(0, 46, "" + strData2, TFT_RED,TFT_BLACK);
      song_album = strData2;
    }
  }
  if (data1 == 1)
  {
    // Serial.printf("专辑：%s", data2);
    _xFont->tft.fillRect(0, 71, 400, 30, TFT_BLACK);
    
    _xFont->DrawChineseEx(0, 71, "" + strData2, TFT_LIGHTGREY,TFT_BLACK);
  }
  if(data1==64)totalSongTime=strData2.toFloat();
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
    _xFont = new XFont(true);

   _xFont->DrawChineseEx(0, 2, "欢迎使用。。", TFT_GREEN);
 
  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_TRACK_NUM | ESP_AVRC_MD_ATTR_NUM_TRACKS|ESP_AVRC_MD_ATTR_PLAYING_TIME|ESP_AVRC_MD_ATTR_GENRE);
  // a2dp_sink.
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  a2dp_sink.set_on_connection_state_changed(connection_state_changed);
  a2dp_sink.set_on_audio_state_changed(audio_state_changed);
  a2dp_sink.set_avrc_rn_play_pos_callback(avrc_rn_play_pos_callback);
  i2s_pin_config_t my_pin_config = {
        .bck_io_num = 32,//输入
        .ws_io_num = 33,//输入
        .data_out_num = 23,//输出
        .data_in_num = I2S_PIN_NO_CHANGE
    };
  a2dp_sink.set_pin_config(my_pin_config);
  a2dp_sink.start("my music");
  
   
 

}

void loop()
{
  delay(100);
}
