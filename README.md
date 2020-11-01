### 1. 简介 

基于EVM + HML + LVGL的 MVVM框架

### 2. 目录结构

| 文件      |    说明|
| :-------- | --------:|
| **bin**| 二进制目录 |
| **bin/x86_64-linux-gnu/evue**| evue二进制程序 |
| **bin/x86_64-window-mingw/evue.exe**| evue二进制程序堆栈信息配置 |
| **frameworks**| js app框架 |
| **include**|   evm头文件  |
| **lib**|  静态库目录 |
| **lib/x86_64-linux-gnu/libhml.a**|   linux下hml静态库 |
| **lib/x86_64-window-mingw/libhml.a**|  windows下hml静态库|
| **lvgl**| lvgl 7.6.1 版本源码|
| **modules**| 库目录 |
| **modules/ecma**| ecma标准库 |
| **modules/hml**| 基于lvgl的hml封装 |
| **src**| 工程源码目录 |
| **test**| 测试代码目录 |


### 3. 如何测试

+ 测试 Div

```

cd test/Div
./evue.exe

```

