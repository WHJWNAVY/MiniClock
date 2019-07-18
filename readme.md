# MiniClock

## 功能说明

 * 主界面三种模式:计时模式,温度模式,农历模式;按上下键切换,15秒后自动回退到计时模式;长按上下键可以固定显示农历模式或温度模式;
 * 计时模式下短按上下键在 小时分钟 秒 年 月日 周 之间切换;
 * 主界面下长安设置键进入时间设置模式;被设置项闪烁显示;短按设置键在 小时, 分钟, 秒 之间切换; 短按上下键调整时间; 长按上键保存(否则不保存);15秒内无操作自动回退到计时模式;
 * 时间设置模式下长安设置键进入日期设置模式;被设置项闪烁显示;短按设置键在 年, 周, 月, 日 之间切换; 短按上下键调整日期; 长按上键保存(否则不保存);15秒内无操作自动回退到计时模式;
 * 日期设置模式下长安设置键进入自动休眠设置模式;被设置项闪烁显示;短按设置键在 开始休眠小时, 开始休眠分钟, 结束休眠小时, 结束休眠分钟 之间切换; 短按上下键调整自动休眠设置; 长按上键保存(否则不保存);15秒内无操作自动回退到计时模式;
 * 自动休眠设置模式下长安设置键进入系统设置模式;被设置项闪烁显示;短按设置键在 亮度, 屏幕镜像 之间切换; 短按上下键调整系统设置; 长按上键保存(否则不保存);15秒内无操作自动回退到计时模式;

## 设置代码

| 设置代码 |           设置项           |          值范围           |
| :------: | :------------------------: | :-----------------------: |
|    C1    |          小时设置          |          0 - 23           |
|    C2    |          分钟设置          |          0 - 59           |
|    C3    |           秒设置           |          0 - 59           |
|    H1    |           年设置           |          0 - 99           |
|    H2    |           周设置           |     1[周一] - 7[周日]     |
|    H3    |           月设置           |          1 - 12           |
|    H4    |           日设置           |          1 - 31           |
|    A1    | 自动休眠开始时间[小时]设置 |          0 - 23           |
|    A2    | 自动休眠开始时间[分钟]设置 |          0 - 59           |
|    A3    | 自动休眠结束时间[小时]设置 |          0 - 23           |
|    A4    | 自动休眠结束时间[分钟]设置 |          0 - 59           |
|    A5    |      自动休眠开启设置      |     0[关闭] - 1[开启]     |
|    P1    |          亮度设置          |          1 - 12           |
|    P2    |        屏幕镜像设置        | 0[正常显示] - 1[镜像显示] |



## 图片欣赏

![计时模式](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001451.jpg)

![温度模式](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001418.jpg)

![农历模式](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001458.jpg)

![秒显示](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001505.jpg)

![年显示](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001433.jpg)

![月日显示](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001512.jpg)

![星期显示](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001440.jpg)

## 外观

![PCB板](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001341.jpg)

![外观](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001403.jpg)

![1.5寸数码管](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001520.jpg)

![显示效果](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001355.jpg)



## 原理图

![原理图](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001840.png)

![PCB底面](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001807.png)

![PCB顶面](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001753.png)



## 代码

![代码](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428002003.png)

![代码](https://raw.githubusercontent.com/WHJWNAVY/myImage/master/PicGo/20190428001927.png)
