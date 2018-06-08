# New Space War
使用Surface Dial移动和旋转自己的战斗机，按下发射炮弹击毙敌人。
使用鼠标移动和旋转自己的战斗机，鼠标左键发射炮弹，鼠标右键移动位置。
# Implement
我们在代码实现时引用了Cocos2D-X框架、Windows 10 API、MetaHook库。
Cocos框架用于实现比较基本的UI和游戏流程，Windows 10 API用于实现与Surface Dial的交互，MetaHook作为Cocos和Windows 10 API之间的桥梁。
# 问题1（Window.cpp）：
Cocos2D中无法获得当前窗口的句柄（HWND），而调用Windows 10 API需要它。解决办法是，使用MetaHook为CreateWindowExW函数（WINAPI）创建钩子，在Cocos代码调用到它时获取返回值，即为HWND。
# 问题2（InGameScene）：
Cocos2D中注册碰撞回调函数后无法判断invoker的类型。解决方法，为飞机、敌人、子弹各创建三个类（class），从cocos2d::Sprite继承而来。然后在需要的时候使用C++自带的RTTI（即dynamic_cast）将Node *转换为CPlane *, CEnemy *, CBullet *。
# 问题3（InGameScene）：
让敌人朝向飞机跟踪而来。由于Action用起来比较复杂，我们直接注册了update事件，在其中手动动态地计算敌人的速度（位置相减*v），感觉此处可能会引发效率问题有待优化。
# 问题4（InGameScene）：
子弹飞出屏幕后应该删除，否则会浪费内存。我们把所有子弹对象（CBullet *）存入动态数组（std::vector），然后在update事件判断每一项，如果该项飞出屏幕了就进行删除。此处同样有效率问题有待优化。
# 问题5（Input.cpp）：
使用Windows 10 API等会丧失cocos2d的跨平台特性，因为这些库大量使用winapi。由于Surface Dial只受Windows 10支持，而UWP生态薄弱，故我们暂时只考虑win32平台。
