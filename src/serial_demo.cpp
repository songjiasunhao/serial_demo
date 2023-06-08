#include <ros/ros.h>
#include <serial/serial.h>
#include <iostream>
#include <serial_demo/distances.h>
 
int main(int argc, char** argv)
{
    ros::init(argc, argv, "serial_port");
    //创建句柄（虽然后面没用到这个句柄，但如果不创建，运行时进程会出错）
    ros::NodeHandle n;
    ros::Publisher distance_pub = n.advertise<serial_demo::distances>("distances",10);
    //创建一个serial对象
    serial::Serial sp;
    //创建timeout
    serial::Timeout to = serial::Timeout::simpleTimeout(100);
    //设置要打开的串口名称
    sp.setPort("/dev/ttyUSB0");
    //设置串口通信的波特率
    sp.setBaudrate(9600);
    //串口设置timeout
    sp.setTimeout(to);
 
    try
    {
        //打开串口
        sp.open();
    }
    catch(serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port.");
        return -1;
    }
    
    //判断串口是否打开成功
    if(sp.isOpen())
    {
        ROS_INFO_STREAM("/dev/ttyUSB0 is opened.");
    }
    else
    {
        return -1;
    }
    
   // ros::Rate loop_rate(500);
    while(ros::ok())
    {
        //获取缓冲区内的字节数
        size_t n = sp.available();
        serial_demo::distances msg;
        if(n!=0)
        {
            uint8_t buffer[1024];
            //读出数据
            n = sp.read(buffer, n);
            
/*             for(int i=0; i<n; i++)
            {
                //16进制的方式打印到屏幕
              std::cout <<std::hex << (buffer[i] & 0xff) << " ";
              
            } */

        if(n==10)
        {
            int one = (buffer[1] & 0xff)*256 +(buffer[2] & 0xff);
            int two = (buffer[3] & 0xff)*256 +(buffer[4] & 0xff);
            int three = (buffer[5] & 0xff)*256 +(buffer[6] & 0xff);
            int four = (buffer[7] & 0xff)*256 +(buffer[8] & 0xff);
           std::cout<<"第一个距离为"<<one<<std::endl;
            std::cout<<"第二个距离为"<<two<<std::endl;
            std::cout<<"第三个距离为"<<three<<std::endl;
            std::cout<<"第四个距离为"<<four<<std::endl;
            if ((one>250&&one<14000) ||(two>250&&two<4000) || (three>250&&three<4000) || (four>250&&four<4000))
            {
                std:: cout<<"报警"<<std::endl;
                 printf("\a");
            }
            
            
            msg.distance[0]= one;
            msg.distance[1]= two;
            msg.distance[2]= three;
            msg.distance[3]= four;

           // distance_pub.publish(msg);

        }
            //把数据发送回去
          //  sp.write(buffer, n);
          distance_pub.publish(msg);
        ros::spinOnce();
        }
        //loop_rate.sleep();
    }
    
    //关闭串口
    sp.close();
 
    return 0;
}
