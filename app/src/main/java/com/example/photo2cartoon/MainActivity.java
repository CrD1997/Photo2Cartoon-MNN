package com.example.photo2cartoon;


import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.MediaStore;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.io.FileInputStream;
import java.lang.reflect.Array;
import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    private ImageView imageView;
    private TextView textView;
    private ArrayList<String> classNames;
    String classificationModelPath;

    // 加载生成的动态链接库, Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    // 声明JNI函数，对应native-lib.cpp里定义的函数
    native String photo2cartoonJNI(Object bitmap, String str);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d("Photo2Cartoon", "onCreating...");

        // 获取摄像头、文件读取权限
        if (!hasPermission()) {
            requestPermission();
        }

        // !!!!!加载模型!!!!!
        classificationModelPath = getCacheDir().getAbsolutePath() + File.separator + "photo2cartoon.mnn";
        Utils.copyFileFromAsset(MainActivity.this, "photo2cartoon.mnn", classificationModelPath);

        // 获取UI控件
        Button selectImgBtn = findViewById(R.id.select_img_btn);
        imageView = findViewById(R.id.image_view);
        textView = findViewById(R.id.result_text);
        selectImgBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // 打开相册
                Intent intent = new Intent(Intent.ACTION_PICK);
                intent.setType("image/*");
                startActivityForResult(intent, 1);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        String image_path;
        if (resultCode == Activity.RESULT_OK) {
            if (requestCode == 1) {
                if (data == null) {
                    Log.w("onActivityResult", "user photo data is null");
                    return;
                }
                Uri image_uri = data.getData();
                image_path = getPathFromURI(MainActivity.this, image_uri);
                try {
                    // 获取图片
                    FileInputStream fis = new FileInputStream(image_path);
                    Bitmap bmp = BitmapFactory.decodeStream(fis);
                    imageView.setImageBitmap(bmp);
                    // !!!!!执行预测!!!!!
                    long start = System.currentTimeMillis();
                    Log.d("Photo2Cartoon", "Predicting...");
                    String result = photo2cartoonJNI(bmp, classificationModelPath);
                    imageView.setImageBitmap(bmp);
                    Log.d("Photo2Cartoon", "Predicted...");
                    long end = System.currentTimeMillis();
                    String show_text = "预测结果标签：" + result +
                            "\n时间：" + (end - start) + "ms";
                    textView.setText(show_text);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    }

    // 根据相册的Uri获取图片的路径
    public static String getPathFromURI(Context context, Uri uri) {
        String result;
        Cursor cursor = context.getContentResolver().query(uri, null, null, null, null);
        if (cursor == null) {
            result = uri.getPath();
        } else {
            cursor.moveToFirst();
            int idx = cursor.getColumnIndex(MediaStore.Images.ImageColumns.DATA);
            result = cursor.getString(idx);
            cursor.close();
        }
        return result;
    }

    // check had permission
    private boolean hasPermission() {
        Log.d("Photo2Cartoon", "hasPermission...");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            return checkSelfPermission(Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED &&
                    checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED &&
                    checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED;
        } else {
            return true;
        }
    }

    // request permission
    private void requestPermission() {
        Log.d("Photo2Cartoon", "requestPermission...");
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.CAMERA,
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        }
    }
}