摄像头实验（例程）
===========================================================================================
实验目的：1. 了解OV2640摄像头模组的工作原理
          2. 了解STM32使用DCMI驱动OV2640的方法
          3. 掌握OV2640 RGB565 模式的应用方法
===========================================================================================
实验准备：
一个eeLab板卡，一个MiniUSB线
一个DC 12V电源，一个ARM仿真器
===========================================================================================
实验步骤：
1）将ARM仿真器的USB端连接电脑，另一端连接节点的调试接口。
2）在IAR软件菜单栏选择Project->Download and debug将程序下载到eeLab板卡并进入到调试状态。
3) 用USB线把节点与PC连接起来,PC的串口助手设置，设置对应的端口号，
   波特率为115200bps（与程序相对应）,8位数据位,无奇偶校验,1位停止位，去掉十六进制显示。
4）在IAR软件菜单栏选择Debug->Go全速运行程序，观察实验现象。
5）退出调试状态：在IAR软件菜单栏选择Debug->Stop Debugging停止程序运行。
6）此时程序已经固化到节点板的Flash中，重启或重新上电，程序将直接运行。

注意：
1）进入Debug状态若出错，请检查电脑的设备管理器是否正确识别仿真器设备，如果未成功
   识别则安装jlink驱动后重试。
2）如果以上方法均无效请更换仿真器再试。
3）若LCD屏显示错误或不显示将节点断电在重新上电
===========================================================================================
实验现象：
程序启动后，如果提示："OV2640初始化成功"，可以通过任意按键进入摄像头界面，界面中可以通过KEY1
\KEY2\KEY3\KEY4按键触发不同的功能。
===========================================================================================

