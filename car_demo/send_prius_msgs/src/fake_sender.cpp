#include <ros/ros.h>
#include <prius_msgs/Control.h>
#include <thread>
#include <algorithm>

prius_msgs::Control shared_msg;

void publishData(int hz, ros::NodeHandle* nh_ptr){
    prius_msgs::Control msg;
    ros::Rate loop_rate(hz);
    ros::Publisher pub = nh_ptr->advertise<prius_msgs::Control>("prius", 10);

    while(true){
        msg.throttle = std::fabs(shared_msg.throttle);
        msg.brake = shared_msg.brake;
        msg.steer = shared_msg.steer;
        msg.shift_gears = (shared_msg.throttle >= 0) ? 
            prius_msgs::Control::FORWARD : prius_msgs::Control::REVERSE;
        pub.publish(msg);
        loop_rate.sleep();
    }
}

int main(int argc, char *argv[]){
    ros::init(argc, argv, "fake_sender");
    ros::NodeHandle nh;

    int hz;
    nh.param("prius/fake_control/frequency", hz, 20);
    std::thread(publishData, hz, &nh).detach();

    double throttle_gain, brake_gain, steer_gain;
    nh.param("prius/fake_control/throttle_gain", throttle_gain, 0.1);
    nh.param("prius/fake_control/brake_gain", brake_gain, 1.0);
    nh.param("prius/fake_control/steer_gain", steer_gain, 0.2);

    ros::Rate loop_rate(hz);//pub thread fre and input thread fre would be differ in the future... 

    char ch;
    while(true){
        ch = getchar();
        ROS_INFO("get key : %c", ch);
        switch(ch){
        case 'w':
            shared_msg.brake = 0;
            shared_msg.steer = 0; 
            shared_msg.throttle += throttle_gain;
            shared_msg.throttle = std::min(shared_msg.throttle, 1.0);
            break;
        case 's':
            shared_msg.brake = 0;
            shared_msg.steer = 0; 
            shared_msg.throttle -= throttle_gain;
            shared_msg.throttle = std::max(shared_msg.throttle, -1.0);
            break;
        case 'd':
            shared_msg.steer -= steer_gain;
            shared_msg.steer = std::max(shared_msg.steer, -1.0);
            break;
        case 'a':
            shared_msg.steer += steer_gain;
            shared_msg.steer = std::min(shared_msg.steer, 1.0);
            break;
        case ' ':
            shared_msg.throttle = 0;
            shared_msg.steer = 0;
            shared_msg.brake += brake_gain;
            shared_msg.brake = std::min(shared_msg.brake, 1.0);
            break;
        default:
            break;
        }
        loop_rate.sleep();
    }
}