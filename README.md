# SRP_GuideTheRoboticArm
## 简介
- SRP任务是制作一个搭载机械臂的轮式机器人，实现货物的运载、抓取和放置。本人负责其中的视觉部分，要实现aprilTag检测确定货物货架号并引导机械臂放置货物。
- aprilTag的检测使用[官方的库](https://github.com/AprilRobotics/apriltag) ，该库的链接方法可见[我的博客](http://t.csdnimg.cn/SBcZI)
- 在检测到货物上贴的aprilTag时要传给下位机货物中心的像素坐标系坐标和在世界坐标系中的绕z轴的转角（pnp算法求解）。
- 在货物的放置阶段，检测货架号（模板匹配）并发送货架号中心在像素坐标系下的x坐标给下位机用于对准。
- 代码使用的是轮询的方式运行，由于当时时间匆忙，aproilTag部分的代码未做二次封装
- 根目录下的**template.png**为模板图片，**test.mp4**为在工位录的测试视频
- 上位机使用的是树莓派4B，编译源码安装的OpenCV，安装过程可见[我的视频](https://www.bilibili.com/video/BV1eM411o72k?vd_source=e67cc43f2e8443b722a5f50ef79db03e)

## 使用方式
- 在工程根目录下：`mkdir build`
- `cd build`
- `cmake ..`
- `make`
- `./srp`
