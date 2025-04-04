实训项目二
===========================================================================================
实验目的：1. 掌握集成燃气灶控制系统需求分析。
          2. 完成火焰级别显示，燃气检测控制程序编写。
===========================================================================================
实验准备：
一个eeLab板卡，一个MiniUSB线
一个DC 12V电源
===========================================================================================
实验步骤：
1）将ARM仿真器的USB端连接电脑，另一端连接节点的调试接口。
2）在IAR软件菜单栏选择Project->Download and debug将程序下载到嵌入式原型机板卡ZI-ARMEmbed并进入到调试状态。
3）在IAR软件菜单栏选择Debug->Go全速运行程序，观察实验现象。
4）退出调试状态：在IAR软件菜单栏选择Debug->Stop Debugging停止程序运行。
5）此时程序已经固化到节点板的Flash中，重启或重新上电，程序将直接运行。

注意：
1）进入Debug状态若出错，请检查电脑的设备管理器是否正确识别仿真器设备，如果未成功
   识别则安装jlink驱动后重试。
2）如果以上方法均无效请更换仿真器再试。
3）若LCD屏显示错误或不显示将节点断电在重新上电
===========================================================================================
项目模型为智能集成灶，集成灶能够通过K4按键触发电
子灶台开关，燃气阀打开，通过按钮控制火候大小火焰分为4级（小火、中火、高火、
大火），当灶台打开但没有检测到有火焰同时燃气泄漏时产生报警信号，同时燃气阀
关闭。LCD屏幕上显示集成灶开关状态，火候大小，火焰检测、燃气监测和厂家logo。     
===========================================================================================
