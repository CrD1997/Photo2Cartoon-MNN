#include <jni.h>
#include <string>

#include <android/bitmap.h>
#include <opencv2/opencv.hpp>
#include "../../androidTest/get_result.h"
#include "photo2cartoon.h"

using namespace cv;

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_photo2cartoon_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_photo2cartoon_MainActivity_getEdge (JNIEnv *env, jobject obj, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels;

    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        Mat gray;
        cvtColor(temp, gray, COLOR_RGBA2GRAY);
        Canny(gray, gray, 45, 75);
        cvtColor(gray, temp, COLOR_GRAY2RGBA);

    } else {
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        Mat gray;
        cvtColor(temp, gray, COLOR_RGB2GRAY);
        Canny(gray, gray, 45, 75);
        cvtColor(gray, temp, COLOR_GRAY2RGB);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_photo2cartoon_MainActivity_getGray (JNIEnv *env, jobject obj, jobject bitmap) {
    AndroidBitmapInfo info;
    void *pixels;

    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(info.height, info.width, CV_8UC4, pixels);

        Mat gray;
        cvtColor(temp, gray, COLOR_RGBA2GRAY);
        Mat result(info.height, info.width, CV_8UC4, pixels);
        cvtColor(gray, result, COLOR_GRAY2RGBA);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_photo2cartoon_MainActivity_mnistJNI (JNIEnv *env, jobject obj, jobject bitmap, jstring jstr) {

    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        Mat temp2 = temp.clone();
        //将jstring类型转换成C++里的const char*类型
        const char *path = env->GetStringUTFChars(jstr, 0);

        Mat RGB;
        //先将图像格式由BGRA转换成RGB，不然识别结果不对
        cvtColor(temp2, RGB, COLOR_RGBA2RGB);
        //调用之前定义好的mnist()方法，识别文字图像
        int result = mnist(RGB, path);
        //将图像转回RGBA格式，Android端才可以显示
        Mat show(info.height, info.width, CV_8UC4, pixels);
        cvtColor(RGB, temp, COLOR_RGB2RGBA);
        //将int类型的识别结果转成jstring类型，并返回
        string re_reco = to_string(result);
        const char* ss = re_reco.c_str();
        char cap[12];
        strcpy(cap, ss);
        return (env)->NewStringUTF(cap);

    } else {
    Mat temp(info.height, info.width, CV_8UC2, pixels);

    }
    AndroidBitmap_unlockPixels(env, bitmap);
}


extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_photo2cartoon_MainActivity_photo2cartoonJNI(JNIEnv *env, jobject obj, jobject bitmap, jstring jstr){
    AndroidBitmapInfo info;
    void *pixels;
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 || info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        Mat temp2 = temp.clone();
        //将jstring类型转换成C++里的const char*类型
        const char *path = env->GetStringUTFChars(jstr, 0);

        Mat BGR;
        //先将图像格式由RGBA转换成RGB，不然识别结果不对
        cvtColor(temp2, BGR, COLOR_RGBA2BGR);
        //调用之前定义好的mnist()方法，识别文字图像
        Mat result_img = photo2cartoon(BGR, path);

        //将图像转回RGBA格式，Android端才可以显示
        Mat show(info.height, info.width, CV_8UC4, pixels);
        cvtColor(result_img, temp, COLOR_RGB2RGBA);
        return (env)->NewStringUTF("photo2cartoon");
    } else {
        Mat temp(info.height, info.width, CV_8UC2, pixels);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
}
