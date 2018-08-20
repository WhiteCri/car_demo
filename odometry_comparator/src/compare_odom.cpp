#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <gazebo_msgs/ModelStates.h>
#include <gazebo_msgs/ModelState.h>
#include <std_msgs/Float64.h>
#include <deque>
#include <cmath>
#include <odometry_comparator/gap_msgs.h>

class OdomComparator{
    typedef std::pair<ros::Time, gazebo_msgs::ModelState> ModelStateStamped;
public:
    OdomComparator(ros::NodeHandle& nh) : nh_ptr(&nh) {
        pub = nh_ptr->advertise<odometry_comparator::gap_msgs>("prius/gap", 10);
    }

    void modelDeque_CB(const gazebo_msgs::ModelStatesConstPtr& ptr){
        auto targetIter = std::find(ptr->name.rbegin(), ptr->name.rend(), "prius");
        int idx = ptr->name.size() - (targetIter - ptr->name.rbegin());

        ModelStateStamped model;
        model.first = ros::Time::now();
        model.second.model_name = "prius";
        model.second.pose = ptr->pose[idx];

        ModelDeque.push_back(model);
    }

    void ndt_pose_CB(const geometry_msgs::PoseStampedConstPtr& ptr){        
        double targetTime = ptr->header.stamp.toSec();
        double time_error = 1.0; //just positive value
        auto it = ModelDeque.begin();
        while(time_error > 0) {
            time_error = targetTime - it++->first.toSec(); // find closest data
            if(it == ModelDeque.end()){
                ROS_WARN("some fucking situation happened. Call me : sjrnfu12@naver.com");
                exit(-1);
            }
        }
        it--;

        double dist;
        dist = std::sqrt(
                std::pow(it->second.pose.position.x - ptr->pose.position.x, 2) +
                std::pow(it->second.pose.position.y - ptr->pose.position.y, 2)
            ); 
        ROS_INFO("gazebo prius pose : %lf %lf", it->second.pose.position.x, it->second.pose.position.y);
        odometry_comparator::gap_msgs gap_msg;
        gap_msg.stamp = ros::Time::now();
        gap_msg.dist = dist;
        pub.publish(gap_msg);

        ModelDeque.erase(ModelDeque.begin(), it+1);
    }
private:
    ros::NodeHandle *nh_ptr;
    ros::Publisher pub;
    std::deque<ModelStateStamped> ModelDeque;
    
};

int main(int argc, char *argv[]){
    ros::init(argc, argv, "compare_odom");
    ros::NodeHandle nh;

    OdomComparator cp(nh);
    ros::Subscriber sub1 = nh.subscribe("/gazebo/model_states", 1, &OdomComparator::modelDeque_CB, &cp);
    ros::Subscriber sub2 = nh.subscribe("/ndt_pose", 1, &OdomComparator::ndt_pose_CB, &cp);

    ros::spin();
}