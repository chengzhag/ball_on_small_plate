# ebox_stm32f103RCT6_VS
注意！！更换为stm32f103C8T6配置好的ebox，visual studio项目，方便以后使用
如果要设置为其他型号：
1. 更换startup文件
1. 更改编译器define
1. 更改debug目标型号

vs工程在ebox_stm32f103RCT6_VS/ebox_stm32f103RCT6_VS/ebox_stm32f103RCT6_VS文件夹下

***工程使用stm32md的startup文件，更改型号时注意更换该文件***

## 包含驱动
### 逻辑器件
- 74HC595：移位寄存器
### 储存
- E2PROM
    - AT24C02
- Flash
    - AT45DB
    - w25x16
- RAM
    - FM25V
- SD/mmc
    - mmc_sd
### 输入设备
- 按键
    - Button
    - key_dm
- 矩阵键盘
    - TM1638：带键盘扫描接口的LED驱动控制专用电路
### 输出设备
- 显示屏
    - FT800
    - lcd_1.8
    - LCD1602
    - Nokia5110
    - OLED
    - OLED_ssd1306
- LED
    - colorled
    - LED
    - led_segment：数码管，使用74hc595
    - TM1638：带键盘扫描接口的LED驱动控制专用电路
    - ws2812：内置IC串行RGBLED
- OSD
    - Max7456：单通道、单色随屏显示（OSD）发生器，内置EEPROM
- GPIO
    - parallel_gpio：8位并行输出，可以组合任意端口
- uart_vcan：配合山外多功能调试助手的串口发送类
### 传感器
- 湿度传感器
    - dht11
- 温度传感器
    - ds18b20
- 尘埃传感器
    - GP2Y1051
- 红外遥控
    - ir_decoder
    - ir_encoder
- 陀螺仪
    - MPU6050
    - L3G4200D
- 电子罗盘
    - lsm303dhlc
- ADC
    - LTC1446：12位逐次逼近式工作的A/D转换器
- 距离传感器
    - ultrasonic_wave：超声波
    - ultrasonic_wave_uart：超声波，基于串口，带有中断绑定功能
### 传输
- 2.4G无线
    - NRF24L01
- 数传
    - si4432：低于1GHz频段的无线数传芯片
- 以太网
    - w5500：全硬件TCP/IP嵌入式以太网控制器
- 串口
    - uart_string：具备字符串buffer的uart类，基于Uart
    - uart_num：用于在两片单片机之间通过串口传输数据
### RTC（实时时钟）
- ds3231：i2c
### 正交编码器
- encoder
- encoder_exti：旋转正交编码器。基于外部中断
- encoder_timer：用官方库配置定时器实现的正交编码器，AB相上下沿触发
- encoder_motor：用encoder_timer实现的直流有刷编码器电，三种控制方式，PID位置、速度控制和无控制
### 电机
- tb6612fng：有刷电机双向驱动
- servo：舵机
- AccuratePwm:Pwm：继承自ebox Pwm的精确占空比Pwm类，覆盖原```set_frq```函数，直接对寄存器进行操作，并且在更改预分频系数前暂存计数器的值，解决了ebox_pwm在改变频率时相位变化的问题！
### 控制
- greg::PID：绝对式PID
- sky::PID：积分分离式绝对PID

## utility(有用的东西)

### 信号处理
- signal_stream：用circle buffer实现的试试信号处理类，仅做信号存储，通过继承或实例化实现信号处理函数

### 存储
- ringbuf：队列，先声明buf数组，传入buf指针使用
- heap：实现malloc函数
- list：链表
- WString：字符串类

### 时间
- calendar：日历

### 显示
- color_convert：hsv、rgb等色彩空间转换

### 通信
- crc：crc循环校验



## 如何配置开发环境？

> 以下所有程序均安装在默认位置

### 安装Jlink驱动

### 安装keil5并破解

1. 安装stm32的pack和CMSIS

### 安装visual studio 2015

1. 选择自定义
1. 包含visual c++

### 安装VassistX

1. 破解目录C:\Users\用户名\AppData\Local\Microsoft\VisualStudio\14.0
1. 复制dll文件覆盖

### 安装VisualGDB

1. 安装visualGDB-5.1r6，Quick Install
2. 打开一次visualstudio
3. 双击reg添加注册表
4. 复制activation.key到C:\Users\用户名\AppData\Local\VisualGDB
5. 复制VisualGDB.exe文件到C:\Program Files (x86)\Sysprogs\VisualGDB

### 打开VS，弹出VisualGDB初始设置

1. 默认打开clang intellisense
2. 提示检测到visual assist X，选择用

### 打开工程编译成功，但有波浪线？intellisense没有正常工作？

1. 最后发现全新安装的keil5在C:\Keil_v5\ARM下缺少CMSIS文件夹，该文件夹下的文件在工程中被包含了
1. CMSIS文件夹怎么获取？下载安装Cortex Microcontroller Software Interface Standard（CMSIS）包，（最新为5.0.1版），并安装
1. keil本身自带CMSIS，但是在C:\Keil_v5\ARM\Pack\ARM\CMSIS\<VERSION>\CMSIS\Include文件夹下，因此在工程的include设置中添加此路径可以解决这个问题
1. keil的CMSIS路径带有版本号，不同版本其路径不同，为了适应不同的开发环境，建议将CMSIS文件夹统一复制到C:\Keil_v5\ARM文件夹下，这样就能杜绝后患

## 如何将工程搬移到visualGDB上？
> 参考[官方教程](https://visualgdb.com/tutorials/arm/keil/)

### 导入keil工程目录
1. 文件扩展名必须小写
1. 导入工程文件夹时去掉原工程没有导入的c/cpp文件，避免include未设置在包含路径内的h文件

### 设置编译器、链接器flag
1. COMMONFLAGS去掉```--split_sections```，否则会报错
1. 改为```--list "$(BINARYDIR)\ebox.map"```，将输出内容保存到Debug文件夹
1. 去掉```--scatter ".\Objects\ebox.sct"```，否则会报错
1. LDFLAGS需要添加```--ro-base```、```--rw-base```等flag，这些设置在keil的项目设置的linker选项卡下

### 设置包含目录
1. 项目的visualGDB propoties的makefile settings中也需要设置include，否则intellisense无法正常
1. 在需要链接dll文件的情况下，需要再makefile settings中设置additional linker inputs

### 设置debug settings
1. 配置j-link时注意手动选择设备类型
1. 下载报错：```Cannot resolve the address of _estack. Skipping stack pointer validity check.```，可以在debug settings中取消勾选validate stack pointer when starting debugging选项解决。

### 其他
1. 全局变量初始化为-1导致程序运行失败，要按照教程后修改Makefile，将```ifeq ($(TARGETTYPE),APP)...endif```替换为
```
ifeq ($(TARGETTYPE),APP)
ROM_SECTION_NAME := ER_RO
$(BINARYDIR)/$(TARGETNAME): $(all_objs) $(EXTERNAL_LIBS)
    $(LD) -o $(@:.elf=.axf) $(LDFLAGS) $(START_GROUP) $(all_objs) \
      $(LIBRARY_LDFLAGS) $(END_GROUP)
    $(FROMELF) --bin --output $(@:.elf=.bin) $(@:.elf=.axf)
    $(OBJCOPY) --remove-section $(ROM_SECTION_NAME) \
        --add-section $(ROM_SECTION_NAME)=$(@:.elf=.bin) \
        --set-section-flags \
        $(ROM_SECTION_NAME)=CONTENTS,ALLOC,LOAD,READONLY,CODE \
        --change-section-address $(ROM_SECTION_NAME)=0x08000000 \
        $(@:.elf=.axf) $@
endif
```
1. makefile,.mak文件中使用```\```进行换行时，后面不能有包括空格在内的任何字符

## visualGDB使用教程

> - [官方教程目录](https://visualgdb.com/w/tutorials/category/tutorials/)
> - [官网功能介绍](https://visualgdb.com/?features=embedded)

- [开发STM32官方教程](https://visualgdb.com/tutorials/arm/stm32/)
- [如何建立模板？](https://visualgdb.com/tutorials/arm/templates/)
- [优化FreeRTOS的实时性](https://visualgdb.com/tutorials/profiler/realtime/freertos/)
- [freeRTOS进程同步](https://visualgdb.com/tutorials/freertos/mutexes/)
- 性能分析工具[embedded-profiler](https://visualgdb.com/w/tutorials/category/tutorials/profiler/embedded-profiler/)
    - [Precise embedded profiling with VisualGDB](https://visualgdb.com/tutorials/profiler/realtime/usb/)
    - [Adding support for more platforms to the Embedded Profiler](https://visualgdb.com/tutorials/profiler/embedded/sampling/driver/)
    - [Profiling a basic STM32 application with Visual Studio](https://visualgdb.com/tutorials/profiler/embedded/sampling/)
- 外围设备[Peripherals](https://visualgdb.com/w/tutorials/category/tutorials/embedded/stm32_periph/)
