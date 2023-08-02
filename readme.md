#### 简介

- 使用17h65 ble芯片和涂鸦wbr3模块配合完成的433远程控制器，支持ble+wifi或者纯ble使用，具体工程可配置
- 433模块采用蜂鸟接收发射模块
- 外壳采用我上一个[开源项目](https://oshwhub.com/ccat0663/ty_433_wifi_ble)留下来的外壳

#### 开源连接

- [硬件开源连接](https://oshwhub.com/ccat0663/ty_433_wifi_ble)

#### 代码工程配置

- 在[ty_dev_config.h](https://github.com/ccat0663/h66_ty_433_ctr/blob/master/software/ST17H66_SDK_3.1.1.2-ty/example/Tuya_IOT/simpleBlePeripheral/source/ty_dev_config.h)文件中配置涂鸦平台生成的参数，选择使能的功能

#### 涂鸦平台配置

- 数据点配置



- studio面板配置



#### 硬件





#### 功耗

- ble+wifi模式：平均电流60ma左右
- 纯ble模式：平均电流60ua左右，硬件原理图ble改成dcdc模式可减小功耗，具体可参考官网

#### 存在问题

- 芯片定时器不是很准确，433拷贝时可能不够准确，如果不能控制可以尝试多拷贝几次
- 芯片定时器不是很准确，异步发送时可能出现数据间隔相差较大，需要多尝试

#### 致谢

- [esp-idf-web-rc-switch](https://github.com/nopnop2002/esp-idf-web-rc-switch)
- [rc-switch](https://github.com/sui77/rc-switch)