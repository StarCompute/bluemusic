# bluemusic

通过蓝牙协议播放音乐，有的时候需要显示歌词，这里就是a2dp库获取了歌词

值得注意的是要想正确获取到歌词，必须打开各种播放器的字幕（歌词）开关

本项目用了三个开源库 a2dp，tft_espi,xfont.

a2dp ：蓝牙传输音频资料和字幕信息

tft_espi : tft 屏幕驱动 

xfont  : 显示汉字 ,详细使用请参考 https://github.com/StarCompute/tftziku 

注意，xfont的初始化必须先执行。


项目内的代码通过网易云音乐 播放正常获取到歌词。

