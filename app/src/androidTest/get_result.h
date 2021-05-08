//
// Created by 63406 on 2021/4/13.
//
#include "../../../../../../Users/63406/AppData/Local/Android/Sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/jni.h"
#include "../../../../../../Users/63406/AppData/Local/Android/Sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/string"
#include "../../../../../../Users/63406/AppData/Local/Android/Sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/iostream"
#include "../../../../../../Users/63406/AppData/Local/Android/Sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/stdio.h"
#include "../../../../../../Users/63406/AppData/Local/Android/Sdk/ndk/21.0.6113669/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/c++/v1/math.h"

#include "../../../../opencv-4.5.2-android-sdk/OpenCV-android-sdk/sdk/native/jni/include/opencv2/opencv.hpp"
#include "../../../../opencv-4.5.2-android-sdk/OpenCV-android-sdk/sdk/native/jni/include/opencv2/core/core.hpp"
#include "../../../../opencv-4.5.2-android-sdk/OpenCV-android-sdk/sdk/native/jni/include/opencv2/highgui/highgui.hpp"
#include "../../../../opencv-4.5.2-android-sdk/OpenCV-android-sdk/sdk/native/jni/include/opencv2/imgcodecs/imgcodecs.hpp"

#include "../../../../MNN/MNN/source/core/Backend.hpp"
#include "../../../../MNN/MNN/include/MNN/Interpreter.hpp"
#include "../../../../MNN/MNN/include/MNN/MNNDefine.h"
#include "../../../../MNN/MNN/include/MNN/Interpreter.hpp"
#include "../../../../MNN/MNN/include/MNN/Tensor.hpp"

using namespace MNN;
using namespace std;
using namespace cv;

int mnist(Mat image_src, const char* model_name);