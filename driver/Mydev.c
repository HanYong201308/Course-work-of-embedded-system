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



#define MYDEV_MAJOR 201    /*Ԥ���Mydev�����豸��Ϊ201*/
#define MYDEV_SIZE 5       /*�豸����������������LED�ܡ�����*/
#define COUNTER1 0x3321     /*�������1�ĵ�ַ*/
#define COUNTER2 0x3320    /*�ٷ�֮һ�������2�ĵ�ַ*/
#define KEY1_ADDR 0x3310     /*�����ĵ�ַ*/
#define KEY2_ADDR 0x3311 
#define KEY3_ADDR 0x3312 
#define LED_ADDR 0x3300     /*LED����ܵ�ַ*/
#define LED_SELECT 0x3301   /*LED�������ʾ�ĸ����ĵ�ַ*/
#define IRQ_MYDEV 7

int dr,dw;
int flag=0;
const unsigned char Number[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
const unsigned char LED_Select[]={0x07,0x0b,0x0d,0x0e};
static int Mydev_MAJOR = MYDEV_MAJOR;
/*Mydev�豸�ṹ��*/
struct Mydev
{                                                        
  struct cdev cdev; /*cdev�ṹ��*/                       
  long count;  /*��¼�豸Ŀǰ�������豸��*/
};
struct read_val/*���ֶ�����ֵ*/
{
    int CT1_val;/*�������ֵ*/
    int CT2_val;/*100Hz������ֵ*/
    int Key_val;
    unsigned long int systimer;

};
struct My_time{//����ʱ�����ͽṹ��
   unsigned int hour;
   unsigned int minute;
   unsigned int second;
    int flag;
};
struct My_time Now_time;
struct read_val MyRD_val;
struct Mydev  Mydev_devp_real; /*�豸�ṹ��ָ��*/
struct Mydev *Mydev_devp; /*�豸�ṹ��ָ��*/

static void Mydev_irq_handle(int irq,void *dev_id,struct pt_regs *regs)
{
    MyRD_val.systimer++;
    outb(0x20,0x20);
}

/*�ļ��򿪺���*/
int Mydev_open(struct inode *inode, struct file *filp)
{ 

  /*���豸�ṹ��ָ�븳ֵ���ļ�˽������ָ��*/
 
  filp->private_data = &Mydev_devp_real;
  Mydev_devp_real.count++;/*�����豸�򿪴���*/

  request_region(0x3300,0x2f,DEVICE_NAME);
  request_irq(IRQ_MYDEV,Mydev_irq_handle,0,DEVICE_NAME,NULL);

  return 0;
}
/*�ļ��ͷź���*/
int Mydev_release(struct inode *inode, struct file *filp)
{
  free_irq(IRQ_MYDEV,NULL);
  release_region(0x3300,0x2f);
  struct Mydev *devp = filp->private_data;/*����豸�ṹ��ָ��*/
  devp->count--;/*�����豸�򿪴���*/
  return 0;
}




/*������*/
static ssize_t Mydev_read(struct file *filp, char __user *buf, size_t size,
  loff_t *ppos)
{
  unsigned int count = size;/*��¼��Ҫ��ȡ���ֽ���*/
  int ret = 0;/*����ֵ*/
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
       /*�ں˿ռ�->�û��ռ佻������*/
  copy_to_user(buf, &MyRD_val, count);
  
  return ret;
}

/*д����*/
static ssize_t Mydev_write(struct file *filp, const char __user *buf,
  size_t size, loff_t *ppos)
{
  int hour_ones,hour_tens,minute_ones,minute_tens;//ʱ��ĸ�λ����ʮλ��
  int ret = 0;  /*����ֵ*/
  unsigned int count =  size;/*��¼��Ҫд����ֽ���*/

  /*�û��ռ�->�ں˿ռ�*/
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

/*��λѰַ����*/
static loff_t Mydev_llseek(struct file *filp, loff_t offset, int orig)
{
  
  loff_t ret = 0;  /*����ֵ*/
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
/*�ļ������ṹ��*/
static const struct file_operations Mydev_fops =
{
  .owner = THIS_MODULE,
  .read = Mydev_read,/*���豸����*/
  .write = Mydev_write,/*д�豸����*/
  .open = Mydev_open,/*���豸����*/
  .release = Mydev_release,/*�ͷ��豸����*/
  .llseek=Mydev_llseek,/*��λѰַ����*/
};

/*��ʼ����ע��cdev*/
static void Mydev_setup_cdev(struct Mydev *dev, int minor)
{
  int err;
  dev_t devno;
  devno = MKDEV(Mydev_MAJOR, minor);/*�����豸��*/
  cdev_init(&dev->cdev, &Mydev_fops);/*��ʼ��cdev�豸*/
  dev->cdev.owner = THIS_MODULE;/*ʹ�����������ڸ�ģ��*/
  dev->cdev.ops = &Mydev_fops;/*cdev����file_operationsָ��*/
  err = cdev_add(&dev->cdev, devno, 1);/*��cdevע�ᵽϵͳ��*/
  if (err)
    //printk(KERN_NOTICE "Error in cdev_add()\n");
    printk(KERN_ALERT "Error in cdev_add()\n");
}
/*�豸����ģ����غ���*/
int Mydev_init(void)
{
  int result;
  dev_t devno = MKDEV(Mydev_MAJOR, 0); /*�����豸��*/

  /* �����豸��*/
 
     register_chrdev_region(devno, 1, "Mydev");
  
    
  
  Mydev_devp = &Mydev_devp_real;
  memset(Mydev_devp, 0, sizeof(struct Mydev));/*���ڴ�����*/
  /*��ʼ���������cdev�ṹ��*/
  Mydev_setup_cdev(Mydev_devp, 0);
  printk(KERN_ALERT "insmod over\n");
  return 0;

  
}

/*ģ��ж�غ���*/
void Mydev_exit(void)
{
  cdev_del(&Mydev_devp->cdev);   /*ע��cdev*/
  unregister_chrdev_region(MKDEV(Mydev_MAJOR, 0), 1); /*�ͷ��豸��*/
  printk(KERN_ALERT "rmmod over\n");
}

MODULE_AUTHOR("Yong Han");
MODULE_LICENSE("Dual BSD/GPL");

module_param(Mydev_MAJOR, int, S_IRUGO);

module_init(Mydev_init);
module_exit(Mydev_exit);
