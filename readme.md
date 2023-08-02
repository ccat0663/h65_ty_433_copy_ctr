#### 简介

- 使用17h65 ble芯片和涂鸦wbr3模块配合完成的433远程控制器，支持ble+wifi或者纯ble使用，具体工程可配置
- 433模块采用蜂鸟接收发射模块
- 外壳采用我上一个[开源项目](https://oshwhub.com/ccat0663/ty_433_wifi_ble)留下来的外壳

#### 开源连接

- 

#### 代码工程配置

- 在[ty_dev_config.h](https://github.com/ccat0663/h65_ty_433_copy_ctr/blob/master/software/ST17H66_SDK_3.1.1.2-ty/example/Tuya_IOT/simpleBlePeripheral/source/ty_dev_config.h)文件中配置涂鸦平台生成的参数，选择使能的功能

#### 涂鸦平台配置

- 数据点配置

![数据点配置](https://github.com/ccat0663/h65_ty_433_copy_ctr/blob/master/file/%E6%B6%82%E9%B8%A6%E6%95%B0%E6%8D%AE%E7%82%B9%E9%85%8D%E7%BD%AE.png)

- studio面板配置

![面板配置](https://github.com/ccat0663/h65_ty_433_copy_ctr/blob/master/file/%E6%B6%82%E9%B8%A6studio%E9%9D%A2%E6%9D%BF.png)

#### 硬件

![实物图](https://github.com/ccat0663/h65_ty_433_copy_ctr/blob/master/file/%E5%AE%9E%E7%89%A9%E5%9B%BE1.png)

![3d图](https://github.com/ccat0663/h65_ty_433_copy_ctr/blob/master/file/3d%E5%9B%BE.png)

![外壳](https://github.com/ccat0663/h65_ty_433_copy_ctr/blob/master/file/%E5%A4%96%E5%A3%B3.png)

#### 功耗

- ble+wifi模式：平均电流60ma左右
- 纯ble模式：平均电流60ua左右，硬件原理图ble改成dcdc模式可减小功耗，具体可参考官网

#### 存在问题

- 芯片定时器不是很准确，433拷贝时可能不够准确，如果不能控制可以尝试多拷贝几次
- 芯片定时器不是很准确，异步发送时可能出现数据间隔相差较大，需要多尝试

#### 致谢

- [esp-idf-web-rc-switch](https://github.com/nopnop2002/esp-idf-web-rc-switch)
- [rc-switch](https://github.com/sui77/rc-switch)