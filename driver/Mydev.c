/*======================================================================
                an example of char device drivers  
   
======================================================================*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/wait.h>

#define DEVICE_NAME "Mydev"



#define MYDEV_MAJOR 201    /*预设的Mydev的主设备号为201*/
#define MYDEV_SIZE 5       /*设备数，两个计数器、LED管、按键*/
#define COUNTER1 0x3321     /*秒计数器1的地址*/
#define COUNTER2 0x3320    /*百分之一秒计数器2的地址*/
#define KEY1_ADDR 0x3310     /*按键的地址*/
#define KEY2_ADDR 0x3311 
#define KEY3_ADDR 0x3312 
#define LED_ADDR 0x3300     /*LED数码管地址*/
#define LED_SELECT 0x3301   /*LED数码管显示哪个亮的地址*/
#define IRQ_MYDEV 7

int dr,dw;
int flag=0;
const unsigned char Number[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
const unsigned char LED_Select[]={0x07,0x0b,0x0d,0x0e};
static int Mydev_MAJOR = MYDEV_MAJOR;
/*Mydev设备结构体*/
struct Mydev
{                                                        
  struct cdev cdev; /*cdev结构体*/                       
  long count;  /*记录设备目前被多少设备打开*/
};
struct read_val/*各种读到的值*/
{
    int CT1_val;/*秒计数器值*/
    int CT2_val;/*100Hz计数器值*/
    int Key_val;
    unsigned long int systimer;

};
struct My_time{//定义时间类型结构体
   unsigned int hour;
   unsigned int minute;
   unsigned int second;
    int flag;
};
struct My_time Now_time;
struct read_val MyRD_val;
struct Mydev  Mydev_devp_real; /*设备结构体指针*/
struct Mydev *Mydev_devp; /*设备结构体指针*/

static void Mydev_irq_handle(int irq,void *dev_id,struct pt_regs *regs)
{
    MyRD_val.systimer++;
    outb(0x20,0x20);
}

/*文件打开函数*/
int Mydev_open(struct inode *inode, struct file *filp)
{ 

  /*将设备结构体指针赋值给文件私有数据指针*/
 
  filp->private_data = &Mydev_devp_real;
  Mydev_devp_real.count++;/*增加设备打开次数*/

  request_region(0x3300,0x2f,DEVICE_NAME);
  request_irq(IRQ_MYDEV,Mydev_irq_handle,0,DEVICE_NAME,NULL);

  return 0;
}
/*文件释放函数*/
int Mydev_release(struct inode *inode, struct file *filp)
{
  free_irq(IRQ_MYDEV,NULL);
  release_region(0x3300,0x2f);
  struct Mydev *devp = filp->private_data;/*获得设备结构体指针*/
  devp->count--;/*减少设备打开次数*/
  return 0;
}




/*读函数*/
static ssize_t Mydev_read(struct file *filp, char __user *buf, size_t size,
  loff_t *ppos)
{
  unsigned int count = size;/*记录需要读取的字节数*/
  int ret = 0;/*返回值*/
  long int i=0;
  	//local_irq_save(flags);
	//outb(0x00,0x21);  
	//local_irq_restore(flags);
     
       enable_irq(IRQ_MYDEV);
  MyRD_val.CT1_val=inb(COUNTER1);
  MyRD_val.CT2_val=inb(COUNTER2);
  MyRD_val.Key_val=inb(0x3310);
  
 // dr=inb(0x3302);          
       // disable_irq(IRQ_MYDEV);
    //	flag=0;
       /*内核空间->用户空间交换数据*/
  copy_to_user(buf, &MyRD_val, count);
  
  return ret;
}

/*写函数*/
static ssize_t Mydev_write(struct file *filp, const char __user *buf,
  size_t size, loff_t *ppos)
{
  int hour_ones,hour_tens,minute_ones,minute_tens;//时间的个位数和十位数
  int ret = 0;  /*返回值*/
  unsigned int count =  size;/*记录需要写入的字节数*/

  /*用户空间->内核空间*/
 copy_from_user(&Now_time, buf, count);
     if(Now_time.flag==0)
		MyRD_val.systimer=0;
    hour_ones=Now_time.hour%10;
    hour_tens=Now_time.hour/10;
    minute_ones=Now_time.minute/10;
    minute_tens=Now_time.minute%10;
    outb(~Number[hour_tens],LED_ADDR);
    outb(LED_Select[0],LED_SELECT);
    udelay(1000);
    outb(~Number[hour_ones],LED_ADDR);
    outb(LED_Select[1],LED_SELECT);
    udelay(1000);
    outb(~Number[minute_tens],LED_ADDR);
    outb(LED_Select[2],LED_SELECT);
    udelay(1000);
    outb(~Number[minute_ones],LED_ADDR);
    outb(LED_Select[3],LED_SELECT);
  return ret;
}

/*定位寻址函数*/
static loff_t Mydev_llseek(struct file *filp, loff_t offset, int orig)
{
  
  loff_t ret = 0;  /*返回值*/
  switch (orig)
  {
      case SEEK_SET:
          if(offset<0){
            ret = - EINVAL;
            break;
          }
          if((unsigned int)offset>MYDEV_SIZE){
            ret = - EINVAL;
            break;          
          }
          filp->f_pos=(unsigned int)offset;
          ret = filp->f_pos;
          break;
      default:
          ret = -EINVAL;
          break;  
  }
  return ret;
}
/*文件操作结构体*/
static const struct file_operations Mydev_fops =
{
  .owner = THIS_MODULE,
  .read = Mydev_read,/*读设备函数*/
  .write = Mydev_write,/*写设备函数*/
  .open = Mydev_open,/*打开设备函数*/
  .release = Mydev_release,/*释放设备函数*/
  .llseek=Mydev_llseek,/*定位寻址函数*/
};

/*初始化并注册cdev*/
static void Mydev_setup_cdev(struct Mydev *dev, int minor)
{
  int err;
  dev_t devno;
  devno = MKDEV(Mydev_MAJOR, minor);/*构造设备号*/
  cdev_init(&dev->cdev, &Mydev_fops);/*初始化cdev设备*/
  dev->cdev.owner = THIS_MODULE;/*使驱动程序属于该模块*/
  dev->cdev.ops = &Mydev_fops;/*cdev连接file_operations指针*/
  err = cdev_add(&dev->cdev, devno, 1);/*将cdev注册到系统中*/
  if (err)
    //printk(KERN_NOTICE "Error in cdev_add()\n");
    printk(KERN_ALERT "Error in cdev_add()\n");
}
/*设备驱动模块加载函数*/
int Mydev_init(void)
{
  int result;
  dev_t devno = MKDEV(Mydev_MAJOR, 0); /*构建设备号*/

  /* 申请设备号*/
 
     register_chrdev_region(devno, 1, "Mydev");
  
    
  
  Mydev_devp = &Mydev_devp_real;
  memset(Mydev_devp, 0, sizeof(struct Mydev));/*将内存清零*/
  /*初始化并且添加cdev结构体*/
  Mydev_setup_cdev(Mydev_devp, 0);
  printk(KERN_ALERT "insmod over\n");
  return 0;

  
}

/*模块卸载函数*/
void Mydev_exit(void)
{
  cdev_del(&Mydev_devp->cdev);   /*注销cdev*/
  unregister_chrdev_region(MKDEV(Mydev_MAJOR, 0), 1); /*释放设备号*/
  printk(KERN_ALERT "rmmod over\n");
}

MODULE_AUTHOR("Yong Han");
MODULE_LICENSE("Dual BSD/GPL");

module_param(Mydev_MAJOR, int, S_IRUGO);

module_init(Mydev_init);
module_exit(Mydev_exit);
