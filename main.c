/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: HanYongDPI
 *
 * Created on 2016年5月16日, 下午8:16
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>

/* 定义电子钟三种工作模式*/
enum Mode{Normal,Modify,Stop_watch};
enum Mode Work_Mode=Normal;
int fd;
int Stop_watch_flag=0;//判断是否在跑秒表的键
unsigned int Counter1=0;
unsigned int Counter2=0;
unsigned int Counter_start=0;
unsigned int Counter_stop=0;
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/*LED显示任务线程*/
struct My_time{//定义时间类型结构体
   int hour;
   int minute;
   int second;
};
struct read_val/*各种读到的值*/
{
    int CT1_val;/*秒计数器值*/
    int CT2_val;/*100Hz计数器值*/
    int Key_val;
    int Key1_event;/*按键值*/
    int Key2_event;
    int Key3_event;
};
struct My_time Now_time={
    .hour=0,
    .minute=0,
    .second=0
};
struct My_time Time2show={
    .hour=0,
    .minute=0,
};
struct read_val MyRD_val;
void *LED_thread(void* arg){
    struct My_time *arg_LEDthread;
    arg_LEDthread = ( struct My_time * )arg;
    //while循环轮巡LED管
    while(1){
       // write(fd,arg_LEDthread,sizeof(* arg_LEDthread));
        //printf("This is the LED thread!\n");
        write(fd,&Time2show,sizeof(Time2show));
    }
    return (void *)123;
}
/*键盘扫描任务线程*/
void *Key_thread(void* arg){
//    int ret2;
/*
    ret2 = ioperm(KEY_ADDR,1,1);
    if(ret2 < 0){
        perror("KeyBoard ioperm set error");
    }
*/
    long int i=0;
    while(1){        
        printf("This is the Key reading thread!\n");
        read(fd,&MyRD_val,sizeof(MyRD_val));
        if(MyRD_val.Key1_event=1){
            if(Work_Mode==Normal);  /*正常模式下短按*/
            if(Work_Mode==Modify)   /*改时间模式下短按*/
            Now_time.hour++;
        }
        if(MyRD_val.Key1_event=2){
            if(Work_Mode==Modify)   /*长按改变工作模式*/
                Work_Mode=Normal;
            if(Work_Mode==Normal)
                Work_Mode=Modify;
        }
        if(MyRD_val.Key2_event=1){
            if(Work_Mode==Normal);  /*正常模式下短按*/
            if(Work_Mode==Modify){   /*改时间模式下短按分钟键*/
                Now_time.minute++;
                if(Now_time.minute==59){
                    Now_time.hour++;
                    Now_time.minute=0;                            
                }
            }
            if(Work_Mode==Stop_watch){
                if(Stop_watch_flag==0){
                    Counter_start=Counter2;
                    Stop_watch_flag=1;
                }
            if(Stop_watch_flag==1){
                Stop_watch_flag=0;
                Counter_stop=Counter2;
            }
        }
        if(MyRD_val.Key2_event=2){
            if(Work_Mode==Modify);   /*长按不改变工作模式*/
            if(Work_Mode==Normal)
                Work_Mode=Stop_watch;
        }
    }
    }
    return (void *)123;
}
void *CTrd_thread(void *arg){
    while(1){
        //pthread_mutex_lock(&mutex);
        //printf("This is the Counter reading thread!\n");
        read(fd,&MyRD_val,sizeof(MyRD_val));
        Counter1=MyRD_val.CT1_val;
        Counter2=MyRD_val.CT2_val;
        //pthread_mutex_unlock(&mutex);
    }
    return (void *)123;
}
/*处理键盘，实现各种功能的主程序 */
int main(int argc, char** argv) {
    pthread_t MyLED_thread,MyKey_thread,MyCT_thread;//声明三个线程
    //pthread_mutex_init(&mutex,NULL);
    int thread_ret;

    fd=open("/dev/Mydev",O_RDWR);
    if(fd < 0){
        perror("/dev/Mydev open error");
        //exit (1);
    }
    /*创建LED显示线程*/
    thread_ret = pthread_create(&MyLED_thread,NULL,LED_thread,(void *)&Time2show);
    if(thread_ret!=0){
        printf ("Create LED_pthread error!\n");
        exit (1);
    }
    /*创建键盘扫描线程*/
    thread_ret = pthread_create(&MyKey_thread,NULL,Key_thread,NULL);
    if(thread_ret!=0){
        printf ("Create Key_pthread error!\n");
        exit (1);
    }
    /*创建读取计数值线程*/
    thread_ret = pthread_create(&MyCT_thread,NULL,CTrd_thread,NULL);
    if(thread_ret!=0){
        printf ("Create CTrd_pthread error!\n");
        exit (1);
    }
    /*主程序循环*/
    while(1){
       // printf("This is the main thread!\n");
        if(Work_Mode==Normal){
            Time2show=Now_time;
            if((Counter1%60==0))
                Now_time.minute++;
            if(Now_time.minute==59){
                Now_time.hour++;
                Now_time.minute=0;
            }
        }
        if(Work_Mode==Stop_watch){
            if(Stop_watch_flag==1){
                Time2show.hour=(Counter2-Counter_start)/100;
                Time2show.minute=(Counter2-Counter_start)%100;
            }
            if(Stop_watch_flag==0){
                Time2show.hour=(Counter_stop-Counter_start)/100;
                Time2show.minute=(Counter_stop-Counter_start)%100;
           }
        }
        else
            Time2show=Now_time;          
    }
    pthread_join(MyLED_thread,NULL);
    pthread_join(MyKey_thread,NULL);
    pthread_join(MyCT_thread,NULL);
    close(fd);
    return (EXIT_SUCCESS);
}