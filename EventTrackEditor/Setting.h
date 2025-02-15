#pragma once

static const int FPS = 60;
static const float FPSf = static_cast<float>(FPS);
static const float SPF = 1.0f/FPSf;
static const float TICK_SEC = SPF;//1目盛りあたりの秒数
static const int TICK_WDTH = 5;//1目盛りあたりの幅
static const float MAX_TIME = 5.0f;//最大秒数
static const int MAX_TICK_NUM = static_cast<int>(MAX_TIME / TICK_SEC);
static const int HEIGHT_BAR = 24;//バーの高さ
