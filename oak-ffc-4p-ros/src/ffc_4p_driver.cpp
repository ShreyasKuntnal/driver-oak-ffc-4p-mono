#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <list>
#include <vector>
#include <linux/videodev2.h>
#include <memory.h>
#include <unistd.h>
#include <time.h>

#include "ffc_4p_driver.h"

#include <ros/ros.h>
#include <std_msgs/Int32.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <image_transport/image_transport.h>
#include <depthai/utility/Clock.hpp>
#include <chrono>
#include <csignal>

#define SECOND 1000000
#define IMAGE_WIDTH 1280
#define FPS_BIAS 0

namespace OAKCAM{
FFC4PDriver::FFC4PDriver(std::shared_ptr<ros::NodeHandle>& nh){
<<<<<<< HEAD
	if(nh ==nullptr){
		ROS_ERROR("Init with a invalid Nodehandler");
		return;
	}
	cv::setNumThreads(1);
	this->ros_node_ = nh;
  ROS_INFO("FFC 4P Device Detecting\n");
  auto deviceInfoVec = dai::Device::getAllAvailableDevices();
  const auto usbSpeed = dai::UsbSpeed::SUPER_PLUS;
  auto openVinoVersion = dai::OpenVINO::Version::VERSION_2021_4;
	if(deviceInfoVec.size() != 1 ){
		ROS_ERROR("Multiple devices or No device detected\n");
		this->device_is_detected_ = 0;
		return;
	}
	this->device_ = std::make_shared<dai::Device>(openVinoVersion, deviceInfoVec.front(), usbSpeed);
	if(device_ == nullptr){
		ROS_ERROR("device init failed\n");
		return;
	}
	//print device infomation
	std::cout << "===Connected to " << deviceInfoVec.front().getMxId() << std::endl;
	auto mxId = this->device_->getMxId();
	auto cameras = this->device_->getConnectedCameras();
	auto usbSpeed_dev = this->device_->getUsbSpeed();
	auto eepromData = this->device_->readCalibration2().getEepromData();
	std::cout << "   >>> MXID:" << mxId << std::endl;
	std::cout << "   >>> Num of cameras:" << cameras.size() << std::endl;
	std::cout << "   >>> USB speed:" << usbSpeed_dev << std::endl;
	if(eepromData.boardName != "") {
			std::cout << "   >>> Board name:" << eepromData.boardName << std::endl;
	}
	if(eepromData.productName != "") {
			std::cout << "   >>> Product name:" << eepromData.productName << std::endl;
	}
	this->device_is_detected_ = 1;
	ROS_INFO("FFC 4P Device detected!\n");
	this->GetParameters(*nh);
}

FFC4PDriver::~FFC4PDriver(){
	this->device_->close();
=======
    if(nh == nullptr){
        ROS_ERROR("Init with an invalid NodeHandle");
        return;
    }
    cv::setNumThreads(1);
    this->ros_node_ = nh;
    ROS_INFO("FFC 4P Device Detecting\n");
    auto deviceInfoVec = dai::Device::getAllAvailableDevices();
    const auto usbSpeed = dai::UsbSpeed::SUPER_PLUS;
    auto openVinoVersion = dai::OpenVINO::Version::VERSION_2021_4;
    if(deviceInfoVec.size() != 1 ){
        ROS_ERROR("Multiple devices or No device detected\n");
        this->device_is_detected_ = 0;
        return;
    }
    this->device_ = std::make_shared<dai::Device>(openVinoVersion, deviceInfoVec.front(), usbSpeed);
    if(device_ == nullptr){
        ROS_ERROR("Device init failed\n");
        return;
    }
    // Print device information
    std::cout << "=== Connected to " << deviceInfoVec.front().getMxId() << std::endl;
    auto mxId = this->device_->getMxId();
    auto cameras = this->device_->getConnectedCameras();
    auto usbSpeed_dev = this->device_->getUsbSpeed();
    auto eepromData = this->device_->readCalibration2().getEepromData();
    std::cout << "   >>> MXID: " << mxId << std::endl;
    std::cout << "   >>> Num of cameras: " << cameras.size() << std::endl;
    std::cout << "   >>> USB speed: " << usbSpeed_dev << std::endl;
    if(eepromData.boardName != "") {
        std::cout << "   >>> Board name: " << eepromData.boardName << std::endl;
    }
    if(eepromData.productName != "") {
        std::cout << "   >>> Product name: " << eepromData.productName << std::endl;
    }
    this->num_cameras = cameras.size();
    this->device_is_detected_ = 1;
    ROS_INFO("FFC 4P Device detected!\n");

    // Set is_connected flag for each camera
    for(auto& camera : this->CameraList){
        camera.is_connected = std::find(cameras.begin(), cameras.end(), camera.socket) != cameras.end();
    }

    this->GetParameters(*nh);
}

FFC4PDriver::~FFC4PDriver(){
    this->device_->close();
>>>>>>> Added Mono Cam
}

//TODO parameter did not get in
void FFC4PDriver::GetParameters(ros::NodeHandle& nh){
<<<<<<< HEAD
	nh.getParam("show_img",this->module_config_.show_img);
	nh.getParam("fps",this->module_config_.fps);
	nh.getParam("resolution",this->module_config_.resolution);
	nh.getParam("auto_expose",this->module_config_.auto_expose);
	nh.getParam("expose_time_us",this->module_config_.expose_time_us);
	nh.getParam("iso",this->module_config_.iso);
	nh.getParam("image_info",this->module_config_.show_img_info);
	nh.getParam("auto_awb", this->module_config_.auto_awb);
	nh.getParam("awb_value", this->module_config_.awb_value);
	nh.getParam("ros_defined_freq", this->module_config_.ros_defined_freq);
	nh.getParam("calibration_mode", this->module_config_.calibration_mode);
	nh.getParam("compresse_assemble_image", this->module_config_.compresse_assemble_image);
	nh.getParam("enable_upside_down", this->module_config_.enable_upside_down);
	switch (this->module_config_.resolution){
		case 720:{
			this->resolution_ = dai::ColorCameraProperties::SensorResolution::THE_720_P;
			break;
		}
		case 800:{
			this->resolution_ = dai::ColorCameraProperties::SensorResolution::THE_800_P;
			break;
		}
		default:{
			ROS_WARN("Unsupport resolution%d, setting to default 720p",this->module_config_.resolution);
			this->resolution_ = dai::ColorCameraProperties::SensorResolution::THE_720_P;
			break;
		}
	}
	// ROS_INFO("Parameter Setting List");
	// ROS_INFO("Start with Image viewer: %d",this->module_config_.show_img);
	// ROS_INFO("FPS: %d",this->module_config_.show_img);
	// ROS_INFO("Resolution: %d", this->module_config_.resolution);
	// ROS_INFO("")
	// ROS_INFO("Image info: %d", this->module_config_.show_img_info)

}

int32_t FFC4PDriver::InitPipeline(){
	this->pipeline_ = std::make_shared<dai::Pipeline>();
	if(this->pipeline_ ==nullptr){
		ROS_ERROR("pipline init failed\n");
		return  -1;
	}
	this->pipeline_->setXLinkChunkSize(0);
	// std::list<std::shared_ptr<dai::node::ColorCamera>> rgb_cam_list;
	for(int i = 0 ; i< this->CameraList.size(); i ++){
		auto rgb_cam = this->pipeline_->create<dai::node::ColorCamera>();

		rgb_cam->setResolution(this->resolution_);
		rgb_cam->setInterleaved(false);
		rgb_cam->setFps(this->module_config_.fps+FPS_BIAS);
		if(this->module_config_.auto_expose){
			rgb_cam->initialControl.setAutoExposureEnable();
		} else {
			rgb_cam->initialControl.setManualExposure(this->module_config_.expose_time_us,this->module_config_.iso);
		}

		if(!this->module_config_.auto_awb){
			rgb_cam->initialControl.setManualWhiteBalance(this->module_config_.awb_value);
		}


		if(CameraList[i].is_master){
			printf("set %s as master camera\n",CameraList[i].stream_name.c_str());
			rgb_cam->initialControl.setFrameSyncMode(dai::CameraControl::FrameSyncMode::OUTPUT);
		} else {
			printf("set %s as slave camera\n",CameraList[i].stream_name.c_str());
			rgb_cam->initialControl.setFrameSyncMode(dai::CameraControl::FrameSyncMode::INPUT);	
		}
		rgb_cam->setBoardSocket(CameraList[i].socket);

		auto xout_rgb = this->pipeline_->create<dai::node::XLinkOut>();
		if(xout_rgb ==nullptr){
			ROS_ERROR("xout link falied\n");
		}
    xout_rgb->setStreamName(CameraList[i].stream_name);
		ROS_INFO("Set stream name:%s\n",CameraList[i].stream_name.c_str());
		
    rgb_cam->video.link(xout_rgb->input);
		// rgb_cam_list.push_back(rgb_cam);
	}
	this->pipeline_is_init_ = 1;
	if(this->device_ != nullptr && this->device_is_detected_ ){
		this->device_->startPipeline(*this->pipeline_);
		return 0;
	}
	ROS_ERROR("Device is not init\n");
	return -2;
=======
    nh.getParam("show_img",this->module_config_.show_img);
    nh.getParam("fps",this->module_config_.fps);
    nh.getParam("resolution",this->module_config_.resolution);
    nh.getParam("auto_expose",this->module_config_.auto_expose);
    nh.getParam("expose_time_us",this->module_config_.expose_time_us);
    nh.getParam("iso",this->module_config_.iso);
    nh.getParam("image_info",this->module_config_.show_img_info);
    nh.getParam("auto_awb", this->module_config_.auto_awb);
    nh.getParam("awb_value", this->module_config_.awb_value);
    nh.getParam("ros_defined_freq", this->module_config_.ros_defined_freq);
    nh.getParam("calibration_mode", this->module_config_.calibration_mode);
    nh.getParam("compresse_assemble_image", this->module_config_.compresse_assemble_image);
    nh.getParam("enable_upside_down", this->module_config_.enable_upside_down);
    nh.getParam("use_rgb", this->module_config_.use_rgb); // New parameter
    nh.getParam("multi_window", this->module_config_.multi_window); // New parameter

    switch (this->module_config_.resolution){
        case 400:{
            if(!this->module_config_.use_rgb){
            this->mono_resolution_ = dai::MonoCameraProperties::SensorResolution::THE_400_P;
            }else{
                ROS_WARN("Unsupport resolution %d for Color Camera, setting to default 720p", this->module_config_.resolution);
                this->color_resolution_ = dai::ColorCameraProperties::SensorResolution::THE_720_P;
            }

            break;
        }
        case 720:{
            if(!this->module_config_.use_rgb){
                this->mono_resolution_ = dai::MonoCameraProperties::SensorResolution::THE_720_P;
            } else {
                this->color_resolution_ = dai::ColorCameraProperties::SensorResolution::THE_720_P;
            }
            break;
        }
        case 800:{
            if(!this->module_config_.use_rgb){
                this->mono_resolution_ = dai::MonoCameraProperties::SensorResolution::THE_800_P;
            } else {
                this->color_resolution_ = dai::ColorCameraProperties::SensorResolution::THE_800_P;
            }
            break;
        }
        case 1080:{
            if(this->module_config_.use_rgb){
                this->color_resolution_ = dai::ColorCameraProperties::SensorResolution::THE_1080_P;
            } else {
                ROS_WARN("Unsupport resolution %d for mono camera, setting to default 720p", this->module_config_.resolution);
                this->mono_resolution_ = dai::MonoCameraProperties::SensorResolution::THE_720_P;
            }
            break;
        }
        default:{
            ROS_WARN("Unsupport resolution %d, setting to default 720p", this->module_config_.resolution);
            if(this->module_config_.use_rgb){
                this->color_resolution_ = dai::ColorCameraProperties::SensorResolution::THE_720_P;
            } else {
                this->mono_resolution_ = dai::MonoCameraProperties::SensorResolution::THE_720_P;
            }
            break;
        }
    }
}

int32_t FFC4PDriver::InitPipeline(){
    this->pipeline_ = std::make_shared<dai::Pipeline>();
    if(this->pipeline_ == nullptr){
        ROS_ERROR("Pipeline init failed\n");
        return -1;
    }
    this->pipeline_->setXLinkChunkSize(0);

    for(int i = 0; i < this->CameraList.size(); i++){
        if (this->module_config_.use_rgb) {
            auto color_cam = this->pipeline_->create<dai::node::ColorCamera>();
            color_cam->setResolution(this->color_resolution_);
            color_cam->setFps(this->module_config_.fps + FPS_BIAS);
            color_cam->setBoardSocket(CameraList[i].socket);
            color_cam->initialControl.setAutoExposureEnable();

            auto xout_color = this->pipeline_->create<dai::node::XLinkOut>();
            if(xout_color == nullptr){
                ROS_ERROR("XLinkOut link failed\n");
                return -1;
            }
            xout_color->setStreamName(CameraList[i].stream_name);
            ROS_INFO("Set stream name: %s\n", CameraList[i].stream_name.c_str());
            color_cam->video.link(xout_color->input);
        } else {
            auto mono_cam = this->pipeline_->create<dai::node::MonoCamera>();
            mono_cam->setResolution(this->mono_resolution_);
            mono_cam->setFps(this->module_config_.fps + FPS_BIAS);
            if(this->module_config_.auto_expose){
                mono_cam->initialControl.setAutoExposureEnable();
            } else {
                mono_cam->initialControl.setManualExposure(this->module_config_.expose_time_us, this->module_config_.iso);
            }

            if(CameraList[i].is_master){
                printf("Set %s as master camera\n", CameraList[i].stream_name.c_str());
                mono_cam->initialControl.setFrameSyncMode(dai::CameraControl::FrameSyncMode::OUTPUT);
            } else {
                printf("Set %s as slave camera\n", CameraList[i].stream_name.c_str());
                mono_cam->initialControl.setFrameSyncMode(dai::CameraControl::FrameSyncMode::INPUT);    
            }
            mono_cam->setBoardSocket(CameraList[i].socket);

            auto xout_mono = this->pipeline_->create<dai::node::XLinkOut>();
            if(xout_mono == nullptr){
                ROS_ERROR("XLinkOut link failed\n");
                return -1;
            }
            xout_mono->setStreamName(CameraList[i].stream_name);
            ROS_INFO("Set stream name: %s\n", CameraList[i].stream_name.c_str());
            mono_cam->out.link(xout_mono->input);
        }
    }

    this->pipeline_is_init_ = 1;
    if(this->device_ != nullptr && this->device_is_detected_){
        this->device_->startPipeline(*this->pipeline_);
        return 0;
    }
    ROS_ERROR("Device is not init\n");
    return -2;
>>>>>>> Added Mono Cam
}

int32_t FFC4PDriver::SetAllCameraSychron(){
	if(this->device_is_detected_ == 0 || this->pipeline_ ==nullptr){
		ROS_ERROR("Device is not detected or pipeline is not initiated\n");
		return -1;
	}
	dai::Device::Config pipeline_config = this->pipeline_->getDeviceConfig();
	pipeline_config.board.gpio[6] = dai::BoardConfig::GPIO(dai::BoardConfig::GPIO::Direction::OUTPUT, 
		dai::BoardConfig::GPIO::Level::HIGH);
	return 0;
}

int32_t FFC4PDriver::SetVedioOutputQueue(){
	for(int i = 0; i < this->CameraList.size(); i++){
		auto rgb_queue = this->device_->getOutputQueue(this->CameraList[i].stream_name, 1, false);
		if(rgb_queue == nullptr){
			ROS_ERROR("Get video queue failed\n");
			return -1 ;
		} else {
			ROS_INFO("Get Out put queue %s success\n",this->CameraList[i].stream_name.c_str());
		}
		this->image_queue_.push_back(ImageNode(rgb_queue,this->CameraList[i].stream_name));
		ROS_INFO("queue back push %s success\n",this->CameraList[i].stream_name.c_str());
	}
	return 0;
}

void FFC4PDriver::StartVideoStream(){
	for(auto& i : this->image_queue_){
		std::stringstream topic;
		if(this->module_config_.calibration_mode){
			topic << "/oak_ffc_4p/image_" << i.topic <<"/compressed";
			i.ros_publisher = this->ros_node_->advertise<sensor_msgs::CompressedImage>(topic.str(),1);
		} else {
			topic << "/oak_ffc_4p/image_" << i.topic;
			i.ros_publisher = this->ros_node_->advertise<sensor_msgs::Image>(topic.str(),1);
		}
		
		ROS_INFO("Image topic %s publisher created",i.topic.c_str());
	}
	ROS_DEBUG("ros publisher established");

	this->expose_time_publisher_ = this->ros_node_->advertise<std_msgs::Int32>("/oak_ffc_4p/expose_time_us",1);
	if(this->module_config_.compresse_assemble_image){
		this->assemble_image_publisher_ = this->ros_node_->advertise<sensor_msgs::CompressedImage>("/oak_ffc_4p/assemble_image/compressed",1);
	} else {
		this->assemble_image_publisher_ = this->ros_node_->advertise<sensor_msgs::Image>("/oak_ffc_4p/assemble_image",1);
	}
	if(this->module_config_.ros_defined_freq){
		printf("Use timer\n");
		// this->thread_timer_  = this->ros_node_->createTimer(ros::Duration(1/this->module_config_.fps*2),&FFC4PDriver::RosGrabImgThread, this);
		this->ros_rate_ptr_ = std::make_unique<ros::Rate>(this->module_config_.fps);
		this->grab_thread_ = std::thread(&FFC4PDriver::RosGrabImgThread,this);
	} else{
		printf("Use std thread\n");
		this->grab_thread_ = std::thread(&FFC4PDriver::StdGrabImgThread,this);
	}
	ROS_INFO("Start streaming\n");
	return;
}

void FFC4PDriver::RosGrabImgThread(){
	while(this->ros_node_->ok() && this->is_run_){
		GrabImg();
		this->ros_rate_ptr_->sleep();
	}
	ROS_INFO("Stop grab tread\n");
}

void FFC4PDriver::StdGrabImgThread(){
	
	while(this->is_run_){
		GrabImg();
		usleep(SECOND/(2.0f*this->module_config_.fps));
	}
	ROS_INFO("Stop grab tread\n");
}
<<<<<<< HEAD
<<<<<<< HEAD

void FFC4PDriver::GrabImg(){
	static cv_bridge::CvImage cv_img, assemble_cv_img;
	static std_msgs::Int32 expose_time_msg;
	static cv::Mat assemble_cv_mat = cv::Mat::zeros(720,5120,CV_8UC3);
	auto host_ros_now_time = ros::Time::now();
	
	assemble_cv_img.header.stamp = host_ros_now_time;
	assemble_cv_img.header.frame_id = "depth ai";
	assemble_cv_img.encoding = "bgr8";
	assemble_cv_img.image = assemble_cv_mat;

	cv_img.header.stamp = host_ros_now_time;
	cv_img.header.frame_id = "depth ai";
	cv_img.encoding = "bgr8";

	expose_time_msg.data = this->module_config_.expose_time_us;

	auto host_time_now = dai::Clock::now();
	int colow_position = 0;
	int image_conter=0;

	for(auto && queue_node : this->image_queue_){
		auto video_frame = queue_node.data_output_q->tryGet<dai::ImgFrame>();
		if(video_frame != nullptr){
			queue_node.image = video_frame->getCvFrame();
			queue_node.cap_time_stamp =  video_frame->getTimestamp();
			image_conter++;
		} else {
			ROS_WARN("Get %s frame failed\n",queue_node.topic.c_str());
			return ;
		}
	}
	//calibration mode publish four compressed image and raw assemble

	if(image_conter == 4){//all cameras get images
		if(this->module_config_.enable_upside_down){
			for(auto && queue_node : this->image_queue_){
				cv::flip(queue_node.image,queue_node.image,-1);
			}
		}
		if(this->module_config_.calibration_mode){
			for(auto && queue_node : this->image_queue_){
				cv_img.image = queue_node.image;
				queue_node.ros_publisher.publish(cv_img.toCompressedImageMsg());
			}
		} else {
			for(auto && queue_node : this->image_queue_){
				queue_node.image.copyTo(assemble_cv_img.image(cv::Rect(colow_position,0,1280,720)));
				colow_position += IMAGE_WIDTH;
			}
			if(this->module_config_.compresse_assemble_image){
				assemble_image_publisher_.publish(assemble_cv_img.toCompressedImageMsg());
			} else {
				assemble_image_publisher_.publish(assemble_cv_img.toImageMsg());
			}

		}
	} else {
		// for (auto && queue_node : this->image_queue_){
		// 	//TODO: might be here
		// 	queue_node.image = cv::Mat::zeros(720,1280,CV_8UC3);
		// }
		printf("[quadcam WARNING]Image not ready clear buffers\n");
	}
	this->expose_time_publisher_.publish(expose_time_msg);
	if(this->module_config_.show_img){
		for(auto & image_node : image_queue_){
			this->ShowImg(image_node,host_time_now);
		}
	}
=======
// void FFC4PDriver::GrabImg(){
// 	static cv_bridge::CvImage cv_img, assemble_cv_img;
// 	static std_msgs::Int32 expose_time_msg;
// 	static cv::Mat assemble_cv_mat = cv::Mat::zeros(720,5120,CV_8UC3);
// 	auto host_ros_now_time = ros::Time::now();
	
// 	assemble_cv_img.header.stamp = host_ros_now_time;
// 	assemble_cv_img.header.frame_id = "depth ai";
// 	assemble_cv_img.encoding = "bgr8";
// 	assemble_cv_img.image = assemble_cv_mat;

// 	cv_img.header.stamp = host_ros_now_time;
// 	cv_img.header.frame_id = "depth ai";
// 	cv_img.encoding = "bgr8";

// 	expose_time_msg.data = this->module_config_.expose_time_us;

// 	auto host_time_now = dai::Clock::now();
// 	int colow_position = 0;
// 	int image_conter=0;

// 	for(auto && queue_node : this->image_queue_){
// 		auto video_frame = queue_node.data_output_q->tryGet<dai::ImgFrame>();
// 		if(video_frame != nullptr){
// 			queue_node.image = video_frame->getCvFrame();
// 			queue_node.cap_time_stamp =  video_frame->getTimestamp();
// 			image_conter++;
// 		} else {
// 			ROS_WARN("Get %s frame failed\n",queue_node.topic.c_str());
// 			return ;
// 		}
// 	}
// 	//calibration mode publish four compressed image and raw assemble

// 	if(image_conter == 4){//all cameras get images
// 		if(this->module_config_.enable_upside_down){
// 			for(auto && queue_node : this->image_queue_){
// 				cv::flip(queue_node.image,queue_node.image,-1);
// 			}
// 		}
// 		if(this->module_config_.calibration_mode){
// 			for(auto && queue_node : this->image_queue_){
// 				cv_img.image = queue_node.image;
// 				queue_node.ros_publisher.publish(cv_img.toCompressedImageMsg());
// 			}
// 		} else {
// 			for(auto && queue_node : this->image_queue_){
// 				queue_node.image.copyTo(assemble_cv_img.image(cv::Rect(colow_position,0,1280,720)));
// 				colow_position += IMAGE_WIDTH;
// 			}
// 			if(this->module_config_.compresse_assemble_image){
// 				assemble_image_publisher_.publish(assemble_cv_img.toCompressedImageMsg());
// 			} else {
// 				assemble_image_publisher_.publish(assemble_cv_img.toImageMsg());
// 			}

// 		}
// 	} else {
// 		// for (auto && queue_node : this->image_queue_){
// 		// 	//TODO: might be here
// 		// 	queue_node.image = cv::Mat::zeros(720,1280,CV_8UC3);
// 		// }
// 		printf("[quadcam WARNING]Image not ready clear buffers\n");
// 	}
// 	this->expose_time_publisher_.publish(expose_time_msg);
// 	if(this->module_config_.show_img){
// 		for(auto & image_node : image_queue_){
// 			this->ShowImg(image_node,host_time_now);
// 		}
// 	}
// }
=======
>>>>>>> New Changes

void FFC4PDriver::GrabImg(){
    static cv_bridge::CvImage cv_img, assemble_cv_img;
    static std_msgs::Int32 expose_time_msg;
    static cv::Mat assemble_cv_mat;
    auto host_ros_now_time = ros::Time::now();

    int image_height = 720; // Default height
    int image_width = IMAGE_WIDTH; // Default width

    // Adjust the height and width based on the resolution
    switch (this->module_config_.resolution) {
        case 400:
            image_height = 400;
            image_width = 640; // Assuming 640x400 resolution
            break;
        case 720:
            image_height = 720;
            image_width = 1280; // Assuming 1280x720 resolution
            break;
        case 800:
            image_height = 800;
            image_width = 1280; // Assuming 1280x800 resolution
            break;
        case 1080:
            image_height = 1080;
            image_width = 1920; // Assuming 1920x1080 resolution
            break;
        default:
            ROS_WARN("Unsupported resolution %d, setting to default 720p", this->module_config_.resolution);
            image_height = 720;
            image_width = 1280;
            break;
    }

    int total_width = image_width * this->CameraList.size();
    if (this->module_config_.use_rgb) {
        assemble_cv_mat = cv::Mat::zeros(image_height, total_width, CV_8UC3);
        assemble_cv_img.encoding = "bgr8";
        cv_img.encoding = "bgr8";
    } else {
        assemble_cv_mat = cv::Mat::zeros(image_height, total_width, CV_8UC1);
        assemble_cv_img.encoding = "mono8";
        cv_img.encoding = "mono8";
    }
    assemble_cv_img.header.stamp = host_ros_now_time;
    assemble_cv_img.header.frame_id = "depth ai";
    assemble_cv_img.image = assemble_cv_mat; // Ensure the image is properly resized

    cv_img.header.stamp = host_ros_now_time;
    cv_img.header.frame_id = "depth ai";

    expose_time_msg.data = this->module_config_.expose_time_us;

    auto host_time_now = dai::Clock::now();
    int colow_position = 0;
    int image_counter = 0;

    for(auto & queue_node : this->image_queue_){
        auto video_frame = queue_node.data_output_q->tryGet<dai::ImgFrame>();
        if(video_frame != nullptr){
            queue_node.image = video_frame->getCvFrame();
            queue_node.cap_time_stamp = video_frame->getTimestamp();
            image_counter++;
        } else {
            // Show warning only if the camera is connected
            auto it = std::find_if(this->CameraList.begin(), this->CameraList.end(), [&](const FFCCameraConfig& config) {
                return config.stream_name == queue_node.topic;
            });
            if(it != this->CameraList.end() && it->is_connected){
                ROS_WARN("Get %s frame failed\n", queue_node.topic.c_str());
            }
        }
    }

    if(image_counter > 0) {
        if(this->module_config_.enable_upside_down){
            for(auto && queue_node : this->image_queue_){
                cv::flip(queue_node.image, queue_node.image, -1);
            }
        }
        if(this->module_config_.calibration_mode) {
            for(auto & queue_node : this->image_queue_){
                if(!queue_node.image.empty()){
                    cv_img.image = queue_node.image;
                    queue_node.ros_publisher.publish(cv_img.toCompressedImageMsg());
                } else {
                    // ROS_WARN("Image for %s is empty, skipping publish", queue_node.topic.c_str());
                }
            }
        } else {
            int rows, cols;
            std::vector<int> camera_order;

            if (this->num_cameras == 4) {
                rows = 2;
                cols = 2;
                camera_order = {0, 3, 1, 2};
            } else if (this->num_cameras == 3) {
                rows = 2;
                cols = 2;
                if (!this->CameraList[0].is_connected) {
                    camera_order = {1, 2, 3};
                } else if (!this->CameraList[1].is_connected) {
                    camera_order = {0, 2, 3};
                } else if (!this->CameraList[2].is_connected) {
                    camera_order = {0, 1, 3};
                } else {
                    camera_order = {0, 1, 2};
                }
            } else if (this->num_cameras == 2) {
                rows = 1;
                cols = 2;
                if (!this->CameraList[0].is_connected && !this->CameraList[3].is_connected) {
                    camera_order = {1, 2};
                } else if (!this->CameraList[1].is_connected && !this->CameraList[2].is_connected) {
                    camera_order = {0, 3};
                } else if (!this->CameraList[0].is_connected && !this->CameraList[1].is_connected) {
                    camera_order = {2, 3};
                } else if (!this->CameraList[1].is_connected && !this->CameraList[3].is_connected) {
                    camera_order = {0, 2};
                } else if (!this->CameraList[0].is_connected && !this->CameraList[2].is_connected) {
                    camera_order = {1, 3};
                } else if (!this->CameraList[2].is_connected && !this->CameraList[3].is_connected) {
                    camera_order = {0, 1};
                } else {
                    // camera_order = {0, 1}; // Default case if all cameras are connected
                }
            } else {
                rows = 1;
                cols = 1;
                if (this->CameraList[0].is_connected) {
                    camera_order = {0};
                } else if (this->CameraList[1].is_connected) {
                    camera_order = {1};
                } else if (this->CameraList[2].is_connected) {
                    camera_order = {2};
                } else if (this->CameraList[3].is_connected) {
                    camera_order = {3};
                } else {
                    // ROS_WARN("No cameras are connected");
                }
            }

            int width = image_width;
            int height = image_height;

            // Create a display image with enough space for the required rows and columns
            cv::Mat disp_image = cv::Mat::zeros(height * rows, width * cols, assemble_cv_img.image.type());

            auto it = this->image_queue_.begin();
            for (int i = 0; i < camera_order.size(); ++i) {
                int row = i / cols;
                int col = i % cols;
                std::advance(it, camera_order[i]);
                auto &node = *it;
                if (!node.image.empty()) {
                    node.image.copyTo(disp_image(cv::Rect(col * width, row * height, width, height)));
                }
                it = this->image_queue_.begin(); // Reset iterator to the beginning
            }

            assemble_cv_img.image = disp_image;

            if(this->module_config_.compresse_assemble_image){
                assemble_image_publisher_.publish(assemble_cv_img.toCompressedImageMsg());
            } else {
                assemble_image_publisher_.publish(assemble_cv_img.toImageMsg());
            }
        }
    }

    this->expose_time_publisher_.publish(expose_time_msg);
    if(this->module_config_.show_img){
        for(auto & image_node : image_queue_){
            this->ShowImg(image_node, host_time_now);
        }
    }
>>>>>>> Added Mono Cam
}

//TODO fps counter
void FFC4PDriver::ShowImg(ImageNode & image_node, std::chrono::_V2::steady_clock::time_point& time_now){
<<<<<<< HEAD
	if(image_node.image.empty()){
		// ROS_ERROR("Image empty\n");
		return;
	} else {
		if(!this->module_config_.show_img_info){
				// ROS_DEBUG("Show pure image\n");
				cv::imshow(image_node.topic.c_str(),image_node.image);
				cv::waitKey(1);
			} else {
				// printf("Show info image\n");
				double clearness = Clearness(image_node.image);
				uint32_t latency_us = std::chrono::duration_cast<std::chrono::microseconds>(time_now - image_node.cap_time_stamp).count();
				std::stringstream info;
				info << image_node.topic << "clearness: = " << clearness <<"    image_delay ms:=" << (latency_us/1000);
				cv::putText(image_node.image, info.str(), cv::Point(10, 30), 
					cv::FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(255, 255, 0));
				cv::imshow(image_node.topic,image_node.image);
				cv::waitKey(1);
			}
	}
	return;
}

double Clearness(cv::Mat &img){
  //Clearness for focus
	if(img.empty()){
		// printf("img is empty");
		return 0.0f;
	} else {
		cv::Mat gray, imgSobel;
		cv::Rect2d roi(img.cols/3, img.rows/3, img.cols/3, img.rows/3);
		cv::rectangle(img, roi, cv::Scalar(255, 0, 0), 1);
		cv::cvtColor(img(roi), gray, cv::COLOR_BGR2GRAY);
		cv::Sobel(gray, imgSobel, CV_16U, 1, 1);
		return cv::mean(imgSobel)[0];
	}
=======
    if(image_node.image.empty()){
        // ROS_ERROR("Image empty\n");
        return;
    } else if (!this->module_config_.multi_window && !this->module_config_.calibration_mode) {
        if (this->module_config_.show_img_info) {
            double clearness = Clearness(image_node.image);
            uint32_t latency_us = std::chrono::duration_cast<std::chrono::microseconds>(time_now - image_node.cap_time_stamp).count();
            static std::map<std::string, std::chrono::_V2::steady_clock::time_point> last_time_map;
            static std::map<std::string, double> fps_map;

            auto now = std::chrono::steady_clock::now();
            if (last_time_map.find(image_node.topic) != last_time_map.end()) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time_map[image_node.topic]).count();
                fps_map[image_node.topic] = 1000.0 / duration;
            }
            last_time_map[image_node.topic] = now;

            std::string camera_id = "Camera ID: " + image_node.topic;
            std::string clearness_str = "Clearness: " + std::to_string(clearness);
            std::string delay_str = "Image Delay (ms): " + std::to_string(latency_us / 1000);
            std::string resolution_str = "Resolution: " + std::to_string(image_node.image.cols) + "x" + std::to_string(image_node.image.rows);
            std::string fps_str = "FPS: " + std::to_string(fps_map[image_node.topic]);

            cv::Scalar textColor = cv::Scalar(0,0,0);
            int thickness = 2;
            int font = cv::FONT_HERSHEY_PLAIN;
            double font_scale = 2.0;
            int line_height = 30;
            cv::putText(image_node.image, camera_id, cv::Point(10, line_height * 1), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, clearness_str, cv::Point(10, line_height * 2), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, delay_str, cv::Point(10, line_height * 3), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, resolution_str, cv::Point(10, line_height * 4), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, fps_str, cv::Point(10, line_height * 5), font, font_scale, textColor, thickness);
        }

        cv::Mat display_image;
        int num_cameras_ = this->num_cameras;
        int rows, cols;
        std::vector<int> camera_order;

        if (num_cameras_ == 4) {
            rows = 2;
            cols = 2;
            camera_order = {0, 3, 1, 2};
        } else if (num_cameras_ == 3) {
            rows = 2;
            cols = 2;
            if (!this->CameraList[0].is_connected) {
                camera_order = {1, 2, 3};
            } else if (!this->CameraList[1].is_connected) {
                camera_order = {0, 2, 3};
            } else if (!this->CameraList[2].is_connected) {
                camera_order = {0, 1, 3};
            } else {
                camera_order = {0, 1, 2};
            }
        } else if (num_cameras_ == 2) {
            rows = 1;
            cols = 2;
            if (!this->CameraList[0].is_connected && !this->CameraList[3].is_connected) {
                camera_order = {1, 2};
            } else if (!this->CameraList[1].is_connected && !this->CameraList[2].is_connected) {
                camera_order = {0, 3};
            } else if (!this->CameraList[0].is_connected && !this->CameraList[1].is_connected) {
                camera_order = {2, 3};
            } else if (!this->CameraList[1].is_connected && !this->CameraList[3].is_connected) {
                camera_order = {0, 2};
            } else if (!this->CameraList[0].is_connected && !this->CameraList[2].is_connected) {
                camera_order = {1, 3};
            } else if (!this->CameraList[2].is_connected && !this->CameraList[3].is_connected) {
                camera_order = {0, 1};
            } else {
                // camera_order = {0, 1}; // Default case if all cameras are connected
            }
        } else {
            rows = 1;
            cols = 1;
            if (this->CameraList[0].is_connected) {
                camera_order = {0};
            } else if (this->CameraList[1].is_connected) {
                camera_order = {1};
            } else if (this->CameraList[2].is_connected) {
                camera_order = {2};
            } else if (this->CameraList[3].is_connected) {
                camera_order = {3};
            } else {
                // ROS_WARN("No cameras are connected");
            }
        }
        int width = image_node.image.cols;
        int height = image_node.image.rows;

        // Create a display image with enough space for the required rows and columns
        display_image = cv::Mat::zeros(height * rows, width * cols, image_node.image.type());

        auto it = this->image_queue_.begin();
        for (int i = 0; i < camera_order.size(); ++i) {
            int row = i / cols;
            int col = i % cols;
            std::advance(it, camera_order[i]);
            auto &node = *it;
            if (!node.image.empty()) {
                node.image.copyTo(display_image(cv::Rect(col * width, row * height, width, height)));
            }
            it = this->image_queue_.begin(); // Reset iterator to the beginning
        }
        // Resize display image if it exceeds 1080p
        if (display_image.rows > 1080 || display_image.cols > 1920) {
            if (!display_image.empty()) {
                // ROS_WARN("Resizing display image to 1080p");
                cv::resize(display_image, display_image, cv::Size(1080, 720));
            }
        }
        
        cv::imshow("FFC4PDriver", display_image);
        int key = cv::waitKey(1);
        if (key == 27 || key == 'q' || key == 'Q') { // Check if 'Esc' or 'q' or 'Q' key is pressed
            this->is_run_ = false;
            cv::destroyAllWindows();
            ros::shutdown(); // Shutdown ROS application
            return;
        }
    } else {
        if (!this->module_config_.show_img_info) {
            // ROS_DEBUG("Show pure image\n");
            cv::imshow(image_node.topic.c_str(), image_node.image);
            int key = cv::waitKey(1);
            if (key == 27 || key == 'q' || key == 'Q') { // Check if 'Esc' or 'q' or 'Q' key is pressed
                this->is_run_ = false;
                cv::destroyAllWindows();
                ros::shutdown(); // Shutdown ROS application
                return;
            }
        } else {
            // printf("Show info image\n");
            double clearness = Clearness(image_node.image);
            uint32_t latency_us = std::chrono::duration_cast<std::chrono::microseconds>(time_now - image_node.cap_time_stamp).count();
            static std::map<std::string, std::chrono::_V2::steady_clock::time_point> last_time_map;
            static std::map<std::string, double> fps_map;

            auto now = std::chrono::steady_clock::now();
            if (last_time_map.find(image_node.topic) != last_time_map.end()) {
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time_map[image_node.topic]).count();
                fps_map[image_node.topic] = 1000.0 / duration;
            }
            last_time_map[image_node.topic] = now;

            std::string camera_id = "Camera ID: " + image_node.topic;
            std::string clearness_str = "Clearness: " + std::to_string(clearness);
            std::string delay_str = "Image Delay (ms): " + std::to_string(latency_us / 1000);
            std::string resolution_str = "Resolution: " + std::to_string(image_node.image.cols) + "x" + std::to_string(image_node.image.rows);
            std::string fps_str = "FPS: " + std::to_string(fps_map[image_node.topic]);

            cv::Scalar textColor = cv::Scalar(0,0,0);
            int thickness = 2;
            int font = cv::FONT_HERSHEY_PLAIN;
            double font_scale = 2.0;
            int line_height = 30;
            cv::putText(image_node.image, camera_id, cv::Point(10, line_height * 1), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, clearness_str, cv::Point(10, line_height * 2), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, delay_str, cv::Point(10, line_height * 3), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, resolution_str, cv::Point(10, line_height * 4), font, font_scale, textColor, thickness);
            cv::putText(image_node.image, fps_str, cv::Point(10, line_height * 5), font, font_scale, textColor, thickness);
            cv::imshow(image_node.topic, image_node.image);
            int key = cv::waitKey(1);
            if (key == 27 || key == 'q' || key == 'Q') { // Check if 'Esc' or 'q' or 'Q' key is pressed
                this->is_run_ = false;
                cv::destroyAllWindows();
                ros::shutdown(); // Shutdown ROS application
                return;
            }
        }
    }
    return;
}

double Clearness(cv::Mat &img){
    if(img.empty()){
		// printf("img is empty");
        return 0.0f;
    } else {
        cv::Mat gray, imgSobel;
        cv::Rect2d roi(img.cols / 3, img.rows / 3, img.cols / 3, img.rows / 3);
        cv::rectangle(img, roi, cv::Scalar(0,0,0), 2);
        if (img.channels() == 3) {
            cv::cvtColor(img(roi), gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = img(roi);
        }
        cv::Sobel(gray, imgSobel, CV_16U, 1, 1);
        return cv::mean(imgSobel)[0];
    }
>>>>>>> Added Mono Cam
}

}