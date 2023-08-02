# tuya ble sdk

The tuya ble sdk encapsulates the communication protocol with Tuya Smart mobile App and implements a event scheduling abilities. The device using tuya ble sdk does not need to care about the specific communication protocol implementation details. It can be interconnected with Tuya Smart App by calling the API and call back provided by the tuya ble sdk.

This topic gives details of the components, porting instruction, SDK configuration, API description, and usage of tuya ble sdk.

## Overview of tuya ble sdk

### Framework

The following figure shows the Application framework based on tuya ble sdk:

![clip_image002.png](https://airtake-public-data.oss-cn-hangzhou.aliyuncs.com/goat/20200310/4de0b2ed979d4b5593ee00f2793ca99b.png)

- platform: the chip platform. The chip and protocol stack are maintained by the chip company.

- Port: the abstract interfaces needed by the  tuya ble sdk. You must implement them according to the chip-specific platform.

- tuya ble sdk: encapsulates the communication protocol of Tuya BLE and provides the service interface to develop Tuya BLE devices.

- Application: your application, built by using tuya ble sdk.

- Tuya sdk API: to implement BLE related management, communication, and so forth. The calls of API are based on asynchronous messages, and the result of API will be notified to the Application of device by message or call back.

- sdk config: by setting the macro in the configuration file, you can configure tuya ble sdk to different modes, for example, the general network configuration mode applicable to multi-protocol devices, Bluetooth singlepoint devices mode, ECDH key based encryption method, whether to use OS, and so forth.

- Main process function: as the engine of tuya ble sdk, to which the Application will call all the time. If the platform architecture has an OS, The tuya ble sdk will automatically create a task to run the main process based on the OS related interface provided by the port layer. If the platform does not have an OS, the device Application needs to be called circularly.

- Message or Call back: SDK sends the data of status, data, and others to device Application through call back function registered by device Application or messages.

### OS compatibility

The tuya ble sdk can run on OS based chip platform other than Linux. If an OS is used, the API requests are based on asynchronous messages. When tuya ble sdk is initialized, the SDK automatically creates a task based on '`tuya_ble_config.h` file to process the message events of the SDK, and creates a message queue to receive the responses of the Application API. The results of the API are notified to the Application of the device in the form of message, so your Application needs to create a message queue and call `tuya_ble_callback_queue_register()` after calling `tuya_ble_sdk_init()` or `tuya_ble_sdk_init_async()` to register the message queue to the SDK.

In the chip platform that has an OS, you can also configure the tuya ble sdk to process messages using the task provided by Application instead of tasks within the tuya ble sdk. By doing so, the Application must implement the outbound message interface at the port layer. 

### Event queue

The earlier event takes precedence to leave (FIFO). Event queue caches the messages sent by the Application and platform layer, the event can be API calls, data response from BLE devices, and so forth. The main process function module circularly queries the message queue and takes it out for processing.

### Directories

| **Directory**               | **Description**                                              |
| --------------------------- | ------------------------------------------------------------ |
| app                         | Stores Applications that managed by the tuya ble sdk, such as Tuya test and production module, general connection modules and so forth. |
| doc                         | Help file.                                                   |
| extern\_components          | External components, for example, the extension for security-specific algorithm. |
| port                        | The abstract interfaces which must be implemented by Applications. |
| sdk                         | The core code of the tuya ble sdk.                           |
| tuya\_ble\_config.h         | The configuration file for tuya ble sdk. However, your Application needs to create another configuration files on demand. |
| tuya\_ble\_sdk\_version.h   | The version file.                                            |
| README.md                   | A brief introduction of the tuya ble sdk.                    |
| tuya\_ble\_sdk\_version.txt | Explains what are updated for each version in Chinese.       |
| CHANGELOG.md                | Explains what are updated for each version in English.       |

## The concepts of tuya ble service

The tuya ble sdk does not provide the interfaces for initializing service. Your Application needs to implement the service characteristics defined in the following table before you initializing the SDK. Other than the services required by the tuya ble sdk, you can also define other services if needed. The initial format of broadcast data must be implemented according to the following table, otherwise the tuya ble sdk cannot work.

| **Service UUID** | **Characteristic UUID**              | **Properties**                | **Security Permissions** |
| ---------------- | ------------------------------------ | ----------------------------- | ------------------------ |
| FD50             | 00000002-0000-1001-8001-00805F9B07D0 | Notify                        | None.                    |
|                  | 00000001-0000-1001-8001-00805F9B07D0 | Write,write without response. | None.                    |


### The MTU

For a better compatibility, The maximum MTU size cannot exceed 247.

### Broadcast data format

The following picture illustrates the broadcast packet format of BLE.

![](https://images.tuyacn.com/fe-static/docs/img/a48b425f-19e7-40c1-986e-11f082416b49.png)

When the SDK is started, the SDK will actively update the advertising content.

## How to port and configure tuya ble sdk

As the following picture shows, the interfaces defined in the file `tuya_ble_port.h` and `tuya_ble_port_peripheral.h` must be ported and implemented according to the chip-specific platform. Note that if the platform used in the Application does not have an OS, the OS related interfaces do not need to be implemented. The `tuya_ble_port.c` and `tuya_ble_port_peripheral.c` are the weak implementation of the interfaces defined for the `tuya_ble_port.h` and `tuya_ble_port_peripheral.h`.

![image.png](https://airtake-public-data-1254153901.cos.ap-shanghai.myqcloud.com/goat/20200606/feac2f6cf3c847a7a0b12077b435fac3.png)

You cannot implement platform-specific interfaces in the preceding `.c` files, please create a new one, for example `tuya_ble_port_nrf52832.c`. If the file name contains the keyword `tuya`, it is the platform implementation file that Tuya Smart has adapted and transplanted, you can refer to it if needed.