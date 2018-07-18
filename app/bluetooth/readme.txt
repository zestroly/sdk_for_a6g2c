1、程序源码要包含蓝牙相关的头文件，编译要加“-lbluetooth”,编译要指定蓝牙相关头文件及库的路径(server/client目录中的include、lib目录)
2、运行程序之前要先配对蓝牙设备，配对方法请参考用户手册 2.15 蓝牙的使用 。
3、交叉工具为EPC-A6G2C对应的交叉工具链(arm-linux-gnueabihf-)。