# 操作系统(CS302) Project —— kernel

生命不息，挖坑不止。

尝试用更现代化的方式实现OSASK类似的简易操作系统（伪）。

必须用32位MBR的grub2生成镜像，否则无法获得正确的显存地址（这个坑就不要去踩了，无解），轻则切换分辨率后无显示，重则导致虚拟机崩溃。

依赖工具：nasm, g++, make, xorriso, mtools, qemu

~~推荐环境：MBR引导启动的32位Ubuntu~~

在64位或EFI引导的系统上需要安装grub-pc才能生成iso镜像。

bgimg.hpp就是张图片而已，编译完成后要占2M的空间，应该会被link到.rodata部分，嫌占空间删掉就得了。

5-1:折腾了大半天，中途好几次差点放弃，终于把中断处理实现了。顺便吐槽一下kernel的中断处理实现方法跟bootloader的区别非常大，OSASK的代码没啥参考价值。

5-3:花了一晚上调试鼠标后来发现vmware里面运行qemu有坑（手动喷血），原因不明。事实上OSASK的鼠标驱动放到kernel里没任何问题。

运行截图：

![](imgs/screenshot.png)
