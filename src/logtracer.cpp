#include "logtracer.h"

std::mutex jumper::jumper_inner::g_inner_mutex;

jumper::LogLevel jumper::LogTracer::s_level = LV_INFO;
std::ofstream jumper::LogTracer::s_ofs;
