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
long int systimer=0;
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
/*LED显示任务线程*/
struct My_time{//定义时间类型结构体
   unsigned int hour;
   unsigned int minute;
   unsigned int second;
    int flag;
};
struct read_val/*各种读到的值*/
{
    int CT1_val;/*秒计数器值*/
    int CT2_val;/*100Hz计数器值*/
    int Key_val;
    unsigned long int systimer;

};
struct My_time Now_time={
    .hour=0,
    .minute=0,
    .second=0,
    .flag=0
};
struct My_time Time2show={
    .hour=0,
    .minute=0,
    .second=0
};
struct My_time MD_time={
    .hour=0,
    .minute=0,
    .second=0,
    .flag=0
};
struct read_val MyRD_val={
	.CT1_val=0,
	.CT2_val=0,
	.Key_val=0,
	.systimer=0,
};
void  delay(int i){
	int j=0;	
	for(j=0;j<1000*i;j++);
}
void *LED_thread(void* arg){
    struct My_time *arg_LEDthread;
    arg_LEDthread = ( struct My_time * )arg;
    //while循环轮巡LED管
    while(1){
       // write(fd,arg_LEDthread,sizeof(* arg_LEDthread));
        //printf("This is the LED thread!\n");
        write(fd,&Time2show,sizeof(Time2show));
	delay(100);
    }
    return (void *)123;
}
/*键盘扫描任务线程*/
void *Key_thread(void* arg){
    long int i=0;
    while(1){        
        read(fd,&MyRD_val,sizeof(MyRD_val));
        if(((MyRD_val.Key_val)&0x01)==0){
            read(fd,&MyRD_val,sizeof(MyRD_val));
            while(((MyRD_val.Key_val)&0x01)==0){
                i++;
                read(fd,&MyRD_val,sizeof(MyRD_val));  
            }
            if((i>10)&&(i<200)){
                printf("Key1 short pressed!\n");
                i=0;
                if(Work_Mode==Normal);  /*Õý³£Ä£ÊœÏÂ¶Ì°Ž*/
                if(Work_Mode==Modify)   /*žÄÊ±ŒäÄ£ÊœÏÂ¶Ì°Ž*/
                    MD_time.hour++;
            }
            if(i>200){
                printf("Key1 long pressed!\n");
                i=0;
		switch(Work_Mode){
		case Normal:
			Work_Mode=Modify;
			MD_time=Now_time;
			break;
		case Modify:
			Work_Mode=Normal;
			Now_time.flag=0;
			write(fd,&Now_time,sizeof(Now_time));
  			 Now_time.flag=1;
			break;
		case Stop_watch:
			Work_Mode=Normal;
			break;
		default:
			Work_Mode=Normal;
			break;
		}
            }
        }
       
        if(((MyRD_val.Key_val)&0x02)==0){
            read(fd,&MyRD_val,sizeof(MyRD_val));
            while(((MyRD_val.Key_val)&0x02)==0){
                i++;
                read(fd,&MyRD_val,sizeof(MyRD_val));            
            }
            if((i>10)&&(i<200)){
                printf("Key2 short pressed!\n");
                i=0;
                    if(Work_Mode==Normal);  /*Õý³£Ä£ÊœÏÂ¶Ì°Ž*/
                    if(Work_Mode==Modify){   /*žÄÊ±ŒäÄ£ÊœÏÂ¶Ì°Ž·ÖÖÓŒü*/
                        MD_time.minute++;
                        if(MD_time.minute==60){
                            MD_time.hour++;
                            MD_time.minute=0;                            
                        }
		}
                    if(Work_Mode==Stop_watch){
			printf("%d\n",Work_Mode);
			switch(Stop_watch_flag){
				case 0:
                               		 Counter_start=MyRD_val.systimer;
                                	 Stop_watch_flag=1;	
					 break;	
				case 1:
                               		 Counter_stop=MyRD_val.systimer;
                                	 Stop_watch_flag=0;	
					 break;	
			}
			printf("%d\n",Stop_watch_flag);                                               
                    }
            }
            if(i>200){
                printf("Key2 long pressed!\n");
                i=0;
                        //printf("The Key was long pressed!\n");
                if(Work_Mode==Modify)
		    Work_Mode=Stop_watch;   /*³€°Ž²»žÄ±ä¹€×÷Ä£Êœ*/
                if(Work_Mode==Normal)
                    Work_Mode=Stop_watch;
            }
        }
        if(((MyRD_val.Key_val)&0x04)==0){
            read(fd,&MyRD_val,sizeof(MyRD_val));
            while(((MyRD_val.Key_val)&0x04)==0){
                i++;
                read(fd,&MyRD_val,sizeof(MyRD_val));            
            }
            if((i>10)&&(i<200)){
                printf("Key3 short pressed!\n");
                i=0;
                    if(Work_Mode==Normal);  /*Õý³£Ä£ÊœÏÂ¶Ì°Ž*/
                    if(Work_Mode==Modify){   /*žÄÊ±ŒäÄ£ÊœÏÂ¶Ì°Ž·ÖÖÓŒü*/
                        Now_time.minute++;
                        if(Now_time.minute==60){
                            Now_time.hour++;
                            Now_time.minute=0;                            
                        }
                    }
                    if(Work_Mode==Stop_watch){
                        if(Stop_watch_flag==0)
                            Counter_start=Counter_stop;
                    }
            }
            if(i>200){
                printf("Key3 long pressed!\n");
                i=0;
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
	if(Counter2==255){
		systimer=systimer+255;
	}
        //pthread_mutex_unlock(&mutex);
    }
    return (void *)123;
}
/*main线程处理键盘，实现各种功能调度*/
int main(int argc, char** argv) {
    pthread_t MyLED_thread,MyKey_thread,MyCT_thread;//声明三个线程
    //pthread_mutex_init(&mutex,NULL);
    int thread_ret;

    fd=open("/dev/Mydev",O_RDWR);
    if(fd < 0){
        perror("/dev/Mydev open error");
        //exit (1);
    }
    write(fd,&Now_time,sizeof(Now_time));
   Now_time.flag=1;
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
        //printf("%d\n",Stop_watch_flag);
	Now_time.hour=Now_time.hour%24;
        if(Work_Mode==Normal){
            Time2show=Now_time;
	Now_time.minute=(MD_time.minute+(MyRD_val.systimer/10)%60)%60;
	Now_time.hour=MD_time.hour+((MyRD_val.systimer/10)/60);
        }
	if(Work_Mode==Modify){
		 Time2show=MD_time; 
		//printf("The now time is    %d:%d\n",Time2show.hour,Time2show.minute);	
	}
        if(Work_Mode==Stop_watch){
            if(Stop_watch_flag==1){
                Time2show.hour=(MyRD_val.systimer-Counter_start)/100;
                Time2show.minute=(MyRD_val.systimer-Counter_start)%100;
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
    return (EXIT_SUCCESS);
}