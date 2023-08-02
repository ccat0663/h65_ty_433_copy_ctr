
版本 | 改动 | 时间 | 责任人（花名） | 备注
--- | --- | --- | --- | ---
1.0.0 | 初始 | 2020/11/23 | 逻辑 |



## 1. 简介

ty_key_press - 按键  

支持按下防抖，松开防抖，防抖参数可设定  
支持单击，多次长按，释放，很容易扩展成双击，三击……  


## 2. 实现介绍

![](https://images.tuyacn.com/fe-static/docs/img/e169c2fa-9a0a-4597-b4e3-13346c1f7a62.png)

按下按键后，会启动一个周期为t的定时器。  
如果按键不松开，会依次到达T1=t*count1, T2=t\*count2, ……, T3=t\*count3(release)时间点  
如果按键松开，可能会触发release0, release1, release2, ……  

press0：可能是抖动，抛弃不用  
release0：一般为了响应及时，count1设置较小（例如50ms），所以很难触发release0  
press1：按下按键累计时间到达count1设置的时间  
release1：松开按键的时间处于count1和count2之间  
press2：按下按键累计时间到达count2设置的时间  
release2：松开按键的时间处于count2和count3之间  
release：按下按键累计时间到达count3设置的时间，由于count3是最后一个count，所以即使按键没有释放，也认为按键释放，即按键超时释放  

### 使用步骤
（1）ty_key_press_init()  
（2）启动一个定时器（建议10ms）  
（3）ty_key_press_timeout_handler()放到定时器处理函数中  
（4）实现ty_key_press_get_pin_level()  
（5）仿照Demo在“关闭定时器”处关闭定时器  
（6）在各种state中加上自定义处理功能即可  


## 3. 依赖
### 3.1 HAL依赖情况

### 3.2 组件依赖

详见包含的头文件

### 3.3 虚函数

```
uint32_t ty_key_press_get_pin_level(uint32_t pin);
```
用户实现以上虚函数即可正常使用该组件  

## 4. 主要结构体和函数介绍
### 4.1 结构体

```
typedef struct {
    uint32_t count;
    uint32_t check_idx;
    uint32_t check_record;
    uint32_t state;
} ty_key_press_inter_param_t;

typedef struct {
    uint32_t pin; //按键对应的引脚
    ty_key_press_level_t valid_level; //有效电平
    uint8_t  count_len; //按键状态数
    uint32_t count_array[10]; //按键状态时间枚举
    ty_key_press_handler_t handler; //按键结果处理
    ty_key_press_inter_param_t inter; //内部变量，不用赋值
} ty_key_press_param_t;
```

### 4.2 函数


## 5. FAQ
