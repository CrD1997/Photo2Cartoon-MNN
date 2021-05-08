//
// Created by 63406 on 2021/4/16.
//
#include"photo2cartoon.h"

#include <jni.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "Backend.hpp"
#include "Interpreter.hpp"
#include "MNNDefine.h"
#include "Interpreter.hpp"
#include "Tensor.hpp"


using namespace MNN;
using namespace std;
using namespace cv;


Mat photo2cartoon(Mat image_src, const char* model_name){

//    const char* image_path = "images/test2.jpg";
    const char* model_path = model_name;

    int forward = MNN_FORWARD_CPU;
    int precision  = 1; //0: normal, 1: high, 2: low
    int power      = 1;
    int memory     = 1;
    int threads    = 1;
    int INPUT_SIZE = 256;

    printf("图像读取...\n");
    cv::Mat raw_image = image_src;
    cv::Mat image;

    printf("加载模型及配置...\n");
    std::shared_ptr<Interpreter> net(Interpreter::createFromFile(model_path)); // 1. 创建Interpreter, 通过磁盘文件创建; shared_ptr: 智能指针
    MNN::ScheduleConfig config; // 2. 调度配置
    config.numThread = threads;
    config.type = static_cast<MNNForwardType>(forward);
    MNN::BackendConfig backendConfig; // 3. 后端配置
    backendConfig.precision = (MNN::BackendConfig::PrecisionMode) precision;
    backendConfig.power = (MNN::BackendConfig::PowerMode) power;
    backendConfig.memory = (MNN::BackendConfig::MemoryMode) memory;
    config.backendConfig = &backendConfig;
    printf("创建session...\n");
    auto session = net->createSession(config); // 4. 创建session；auto: 声明变量时，根据初始化表达式自动推断类型
    net->releaseModel();

    clock_t start = clock();
    // 5.输入预处理
    cv::resize(raw_image, image, cv::Size(INPUT_SIZE, INPUT_SIZE));
    cv::cvtColor(image, image, COLOR_BGR2RGB);
    image.convertTo(image, CV_32FC3);
    // image = image / 255.0f;
    // cv::imwrite("images/image.jpg", image);

    // std::vector<int> dims{1, 3, INPUT_SIZE, INPUT_SIZE};
    std::vector<int> dims{1, INPUT_SIZE, INPUT_SIZE, 3};
    auto nchw_Tensor = MNN::Tensor::create<float>(dims, NULL, MNN::Tensor::TENSORFLOW);
    auto nchw_data = nchw_Tensor->host<float>(); // nhwc_data: float*类型的指针
    auto nchw_size = nchw_Tensor->size(); // 占用内存长度
    ::memcpy(nchw_data, image.data, nchw_size); // (dest, src, n): 由src指向地址为起始地址的连续n个字节的数据复制到以dest指向地址为起始地址的空间内
    printf("nhwc_Tensor...\n");
    printf("[");
    for(int i=0;i<nchw_Tensor->shape().size();i++){ // nhwc_Tensor->shape(): vector<int>
        printf("%d ", nchw_Tensor->shape()[i]);
    }
    printf("]\n");

    printf("获取输入...\n");
    std::string input_tensor = "input";
    // 获取输入tensor
    // 拷贝数据, 通过这类拷贝数据的方式，用户只需要关注自己创建的tensor的数据布局，
    // copyFromHostTensor会负责处理数据布局上的转换（如需）和后端间的数据拷贝（如需）。
    auto inputTensor  = net->getSessionInput(session, nullptr);
    inputTensor->copyFromHostTensor(nchw_Tensor); // inputTensor根据nhwc_Tensor调整
    printf("[");
    for(int i=0;i<inputTensor->shape().size();i++){ // nhwc_Tensor->shape(): vector<int>
        printf("%d ", inputTensor->shape()[i]);
    }
    printf("]\n");
    auto inputTensorPtr = inputTensor->host<float>();
    // for(int j=0; j<36; j++){
    //     printf("%f ", *inputTensorPtr);
    //     inputTensorPtr++;
    // }

    printf("运行session...\n");
    net->runSession(session); // 6. 运行session

    // 7. 获取输出
    printf("获取输出...\n");
    std::string output_tensor_name = "output";
    // 获取输出tensor
    auto tensor_scores  = net->getSessionOutput(session, output_tensor_name.c_str());
    auto nhwcTensor = new Tensor(tensor_scores, Tensor::TENSORFLOW);
    tensor_scores->copyToHostTensor(nhwcTensor);
    // post processing steps
    printf("[");
    for(int i=0;i<nhwcTensor->shape().size();i++){ // nhwc_Tensor->shape(): vector<int>
        printf("%d ", nhwcTensor->shape()[i]);
    }
    printf("]\n");
    auto nhwcTensor_data  = nhwcTensor->host<float>();

    cv::Mat result_img = cv::Mat::zeros(256, 256, CV_32FC3);
    printf("...\n");
    ::memcpy(result_img.data, nhwcTensor_data, nhwcTensor->size());
    printf("...\n");
    // for (auto it = result_img.begin<cv::Vec3b>(); it != result_img.end<cv::Vec3b>(); ++it)
    // {
    //     std::cout << float((*it)[0]) << " " << int((*it)[1]) << " " << int((*it)[2]) << std::endl;
    // }
    result_img.convertTo(result_img, CV_8UC3, 255);
    cv::resize(result_img, result_img, cv::Size(raw_image.cols, raw_image.rows));
    // cv::cvtColor(result_img, result_img, COLOR_RGB2BGR);
    // cv::imwrite("images/result.jpg", result_img);
    printf("...\n");

    return result_img; // RGB
}

