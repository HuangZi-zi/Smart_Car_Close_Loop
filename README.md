# Smart_Car_Close_Loop
利用码盘测速实现闭环控制

# 0814
将超声波舵机移动到PA7，取消中间红外寻迹传感器，将右侧红外寻迹传感器移动到PA0

修改了对应的舵机和寻迹函数，完成编译但未测试

# 0815
完成旧功能的调试，红外寻迹、舵机、电机、超声波测距工作正常

编写了测速函数，完成编译但未测试

# 0816
完成M法测速程序的编写，每秒测速4次，单位为脉冲次数(Hz)