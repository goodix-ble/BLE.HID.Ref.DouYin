# BLE.HID.Ref.DouYin
> 创意无处不在



## 1. 功能说明

参考Demo 是BLE HID Profile的一个趣味化程序, 基于GR533x SK板开发, 可以轻松移植到Goodix 的所有蓝牙SK板. Demo通过 GR533x SK板使用抖音app, 模拟了上划、下划、点赞等功能. 简单演示了空中手势功能。大家可以基于参考程序, 扩展更多趣味实现, 找到BLE更多的创意场景.



## 2. 构建程序

本仓库是一个示例工程, 需要依赖完整的GR533x SDK。

1. 先clone GR533x SDK到本地工作区, 点击查看[GR533x.SDK](../../../GR533x.SDK)
2. 再将本仓库工程后,工程目录ble_app_hids_douyin拷贝到 ${GR533x.SDK}\projects\ble\ble_peripheral 目录下. 完整路径为:   ${GR533x.SDK}\projects\ble\ble_peripheral\ble_app_hids_douyin
3. 使用 keil环境构建编译
4. 下载到 GR533x SK开发板体验即可. 
5. 如果有Goodix其他的SK板, 可以轻松移植到对应的SDK下.



## 3. 关键技术
- HID map：该项目中HID Map  rep_map_data 包含鼠标，手指，音量键三种功能的描述。
- Android和iOS对HID的适配不一致。该项目中Android的滑动和点击使用手指模拟，iOS的滑动和点击使用鼠标模拟，因此需要在BLE连接上后发现一次Ancs服务，如果服务存在则是iOS系统，否则是Android系统。

- 模拟滑动时，在每个动作之间必须要有延时。一般手机的刷新率为60HZ，因此该项目中每个动作之间的延时控制在15ms，来匹配手机的刷新率。确保动作可以被识别。



### 4. 玩法

- 编译ble_app_hids_douyin工程，并下载代码到GR533x SK板中
- 打开手机系统蓝牙，可发现Goodix_DY设备。连接配对
- 打开抖音，单击Key1可上滑，单击Key2可下滑，双击Key1/Key2可点赞



### 5. 其他

### 5.1 视频演示

- 可以通过如下链接, 在线观看程序的视频演示效果



<iframe src="//player.bilibili.com/player.html?aid=963219849&bvid=BV1aH4y1B7Bb&cid=1325208178&p=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>