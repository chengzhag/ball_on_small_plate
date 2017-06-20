# BallOnPlate
基于opencv的树莓派小球平衡控制项目，识别并定位小球在平板上的位置，并通过串口发送给stm32f103单片机

## 整体方案

* 玻纤板做平板
    - 如果是舵机球头连杆驱动，需要设计舵机支架的安装、摇臂的转轴支撑
* 舵机驱动
    - 标准舵机：要求可控角度不大，因此可以考虑。球头连杆传动，力矩可变（舵机位置可调）
    - 直驱方式不太现实
* 树莓派opencv摄像头/电阻屏定位小球

## 计算机视觉方案

1. 阈值分割出平板
1. 检测平板轮廓，得到四个角点
1. 进行透视变换，消除平板角度影响
1. 全局寻找极小值，定位小球位置
1. 如果极小值较大，认为小球掉落

### 开发环境
- windows端搭建linux opencv交叉编译环境
- 树莓派搭载opencv3.2和鱼眼镜头
- 调试时采用socket+jpeg发送帧序列到windows server应用，黑白640*480分辨率下可以达到30帧

## 参考
- [基于Socket和OpenCV的实时视频传输（On Windows）](http://blog.csdn.net/pengz0807/article/details/52204134)
- [基于Socket和OpenCV的实时视频传输（On Linux）](http://blog.csdn.net/pengz0807/article/details/52204475)

# OpenCV_raspberry
为树莓派编译opencv，基于visualGDB的项目，不包含opencv源码和编译结果！！

## 简单使用
基于[官方教程](https://visualgdb.com/tutorials/raspberry/opencv/build/)建立opencv3交叉编译工程，所有软件均安装在默认目录

### 树莓派配置
1. 安装基于官方教程的镜像文件配置了opencv开发环境和依赖库的镜像

### windows端配置
1. 安装并破解visualGDB5.2r8（根据前面的经验，5.1版无法识别树莓派系统目录中的某些依赖库）
1. 解压打包好的opencv源码和编译好的动态链接库到D:\Program Files\OpenCV_3_2_0_source（为该工程中配置的opencv源码目录，不可更改）
1. 安装官网提供的[Raspberry / PI的Windows工具链](http://gnutoolchains.com/raspberry/)。下载  4.9.2	2016-09-23-raspbian-jessie (Raspberry Pi 1/2/3/Zero)	raspberry-gcc-4.9.2-r4.exe (738 MB)  并安装
1. 直接打开visual studio工程文件，右键工程名-visualGDB属性-Project settings-Deployment machine，设置目标树莓派的ip和账户、密码
1. 右键工程名-visualGDB属性-CMake project settings-同步sysroot-OK，大概耗时1小时。也可以直接复制拷贝的sysroot文件夹到C:\SysGCC\Raspberry\arm-linux-gnueabihf目录
1. 此时就可以进行编译了，如果报错，可能需要按照[官方教程](https://visualgdb.com/tutorials/raspberry/opencv/build/)第10步在windows端make install一下


## 经验与资料

### 树莓派配置：

1. 安装操作系统
    * Ubuntu MATE: https://ubuntu-mate.org/raspberry-pi/
    * Raspbian: https://www.raspberrypi.org/downloads/

2. 对操作系统进行简单配置
    * 从零开始搭建Raspberry Pi机器视觉编程环境: http://blog.csdn.net/iracer/article/details/51620051

3. 安装opencv相关库
    * 【教程】树莓派安装OS之后的初始配置，以安装OpenCV 3.1.0为例: http://www.jianshu.com/p/7afe8bfa26c0
    * 从零开始搭建Raspberry Pi机器视觉编程环境: http://blog.csdn.net/iracer/article/details/51620051


### vs配置：

1. 安装vs
2. 安装visualGDB最新版
3. 新建linux工程
    * 基础教程：
    * 
        * [Developing a Raspberry PI app with Visual Studio](https://visualgdb.com/tutorials/raspberry/)

    * 采用windows端交叉编译方案：
    * 
        * [交叉编译OpenCV 3为Raspberry Pi 2](https://visualgdb.com/tutorials/raspberry/opencv/build/)
        * [在Raspberry Pi上使用OpenCV库](https://visualgdb.com/tutorials/raspberry/opencv/)


4. 如何通过opencv调用树莓派摄像头：
    * [使用OpenCV与Raspberry Pi 2相机](https://visualgdb.com/tutorials/raspberry/opencv/camera/)
    * [使用Visual Studio的C ++程序的Raspberry Pi 2相机]（https://visualgdb.com/tutorials/raspberry/camera/）

5. 关于树莓派的windows工具链：
    * [Raspberry / PI的Windows工具链](http://gnutoolchains.com/raspberry/)

6. VisualGDB提供的官方兼容镜像：
    * [Raspberry Pi / Raspberry Pi 2 Jessie Image](http://gnutoolchains.com/raspberry/jessie/)

7. 关于其他树莓派库：
    * [使用WiringPi库与Raspberry PI交叉编译器](https://visualgdb.com/tutorials/raspberry/wiringPi/)
    * [为树莓PI创建“闪烁LED”项目](https://visualgdb.com/tutorials/raspberry/LED/)



### vs配置问题
按照[交叉编译OpenCV 3为Raspberry Pi 2](https://visualgdb.com/tutorials/raspberry/opencv/build/)教程配置后，编译失败？编译进行到一半提示"lib... needed by ... not found"（最后解决方法[修复与交叉编译器的路径链接问题](https://sysprogs.com/w/forums/topic/error-compile-with-open-cv-2-4/)

* 已检查下载了pkg-config-lite for windows并解压到了<sysgcc> \ Raspberry \ bin目录
* 已检查通过 sudo apt-get install libgtk2.0-dev安装了gtk2.0
* 已检查synchronize sysroot时包含了/ usr / share / pkgconfig和/ opt / vc目录
* 已为cmake设置环境变量：```PKG_CONFIG_SYSROOT_DIR=C:/SysGCC/Raspberry/arm-linux-gnueabihf/sysroot|PKG_CONFIG_PATH=C:/SysGCC/Raspberry/arm-linux-gnueabihf/sysroot/usr/lib/arm-linux-gnueabihf/pkgconfig;C:/SysGCC/Raspberry/arm-linux-gnueabihf/sysroot/usr/share/pkgconfig```
* 相似问题：
    * 官方文档：https://sysprogs.com/w/fixing-rpath-link-issues-with-cross-compilers/
    * [LD警告：LIB-XYZ，LIB-ABC需要 - CROSS COMP RASP PI"](https://sysprogs.com/w/forums/topic/ld-warning-lib-xyz-needed-by-lib-abc-cross-comp-rasp-pi/)
        * 但是cmake如何添加ldflags？如何更改library_names？

    * 根据[CANNOT COMPILE RASPICAM](https://sysprogs.com/w/forums/topic/error-compile-with-open-cv-2-4/)
        * 编译进行到一半提示lib... needed by ... not found
        * Hi,
Looks like your toolchain sysroot may have the incorrect linker configuration file. Please try updating to VisualGDB 5.2 and then resynchronize the sysroot. This should repair the config file automatically.

        * 下一步升级到visualGDB5.2尝试重新同步树莓派目录


编译好后的二进制可执行文件如何运行？

* chmod添加可执行权限：chmod 777 filename
* 执行：./filename 参数列表

# 例程

## OpenCVDemo
基于上述编译好后的opencv库的演示例程——canny边缘检测

## Raspicam
树莓派的C++摄像头库编译工程，支持opencv

## raspicam-0.1.6
被Raspicam引用，raspicam源码

- 在编译完成后，将生成的动态库libraspicam.so更名为libraspicam.so.0.1，和libraspicam_cv.so一起放入/usr/lib/raspicam文件夹。关于linux的动态库存放位置，看[Linux 里有/lib和/usr/lib，这两个目录下的库文件有什么区别吗？如果没区别为什么又要分开放呢](http://blog.csdn.net/fengyunjh/article/details/6232894)
- 然后按[linux找不到动态链接库 .so文件的解决方法](http://www.cnblogs.com/xudong-bupt/p/3698294.html)更新动态库搜索路径
- 关于linux下文件夹的创建和文件的移动看[Linux文件的复制、删除和移动命令使用说明](http://www.jb51.net/LINUXjishu/34936.html)

## RaspberryCameraTest
测试编译的Raspicam库，不依赖opencv

## OpenCVCameraDemo
基于opencv和Raspicam的边缘检测例程
