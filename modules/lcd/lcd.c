/*************************************************************************
	> File Name: lcd.c
	> Author: 
	> Mail: 
	> Created Time: Tue 14 Nov 2017 03:30:47 AM PST
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/fb.h> //fb_fix_screeninfo,fb_var_screeninfo
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
void put_pixel(int x, int y, unsigned int c);
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;
unsigned char *lcd_addr;
int main()
{
int fd;
unsigned int lcd_size;
int x, y;
//1. 打开/dev/fbX 设备
fd = open("/dev/fb0", O_RDWR);
if(fd < 0) {
perror("open");
exit(1);
}
//2. 使用 ioctl 获得运行平台的 framebuffer 设备的信息
ioctl(fd, FBIOGET_FSCREENINFO, &finfo); //获得屏幕固定信息
ioctl(fd, FBIOGET_VSCREENINFO, &vinfo); //获得屏幕可变信息
printf("vinfo.xres:%d,vinfo.yres:%d,bpp:%d\r\n",
vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
//3. 使用前面获得的 framebuffer 信息计算 LCD 显存的大小
lcd_size = finfo.line_length * vinfo.yres_virtual;
//4. 调用 mmap 函数把内核空间中开辟的显存映射到进程空间中
lcd_addr = mmap(NULL, lcd_size, PROT_READ | PROT_WRITE, PROT_READ, fd, 0);
if(lcd_addr == (void *) - 1) {
perror("mmap");
exit(1);
}
//5. 使用 mmap 得到的地址操作显存空间的内容，从而改变 LCD 屏的显示?
// (x， y)座标偏移量：
//pos = line_length*y + x*(bits_per_pixel/8)
//内存地址：
//lcd_addr + pos深圳信盈达科技有限公司—陈志发 Framebuffer 设备应用程序编程 Linux 笔记 2016-11-08

memset(lcd_addr, 0, lcd_size); //清显存缓冲数据为黑色
for(y = 10; y < 1024; y++)
for(x = 10; x < 1024; x++) {
#if 0
unsigned int pos;
pos = finfo.line_length * y + x * (vinfo.bits_per_pixel / 8);
//24BPP ,32BPP
*(int *)(lcd_addr + pos) = 0xff0000; //红色
#endif
put_pixel(x, y, 0xff0000);//绿色
}
//6. 当不使用 framebuffer 设备时，使用 munmap 取消映射。
munmap(lcd_addr, lcd_size);
return 0;
}
//在(x,y)点显示一个颜色为 c 的点
void put_pixel(int x, int y, unsigned int c)
{
unsigned int pos;
pos = finfo.line_length * y + x * (vinfo.bits_per_pixel / 8);
//24BPP ,32BPP
*(int *)(lcd_addr + pos) = c; //红色
}
