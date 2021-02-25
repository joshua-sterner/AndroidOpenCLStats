#include <jni.h>
#include <string>
#include <android/log.h>
#include <vector>
#define CL_HPP_TARGET_OPENCL_VERSION 220
#include "CL/opencl.hpp"
#include <libopencl.h>

#define LOG_PLATFORM_INFO(KEY,PLATFORM,ID) logPlatformInfoItem<KEY>(PLATFORM, ID, #KEY);
#define LOG_DEVICE_INFO(KEY,DEVICE,ID,PLATFORM_ID) logDeviceInfoItem<KEY>(DEVICE, ID, PLATFORM_ID, #KEY);
#define LOG_PUSH_BACK_FLAG(VALUE,VALUES,FLAG) if (VALUE & FLAG) {VALUES.push_back(#FLAG);};
#define LOG_PUSH_BACK_EQ(VALUE,VALUES,FLAG) if (VALUE == FLAG) {VALUES.push_back(#FLAG);};

template<typename T=std::string>
void dlog(std::string tag, std::string value) {
    __android_log_write(ANDROID_LOG_DEBUG, tag.c_str(), value.c_str());
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type dlog(std::string tag, T value) {
    __android_log_write(ANDROID_LOG_DEBUG, tag.c_str(), std::to_string(value).c_str());
}

template<typename T>
void dlog(std::string tag, std::vector<T> value) {
    std::string valueStr = "[";
    for (size_t i = 0; i < value.size(); ++i) {
        valueStr += std::to_string(value[i]);
        if (i < value.size() - 1) {
            valueStr += ", ";
        }
    }
    valueStr += "]";
    __android_log_write(ANDROID_LOG_DEBUG, tag.c_str(), valueStr.c_str());
}

template<cl_int NAME>
void logPlatformInfoItem(cl::Platform platform, int platformID, std::string key) {
    std::string tag = std::string("CLInfo.Platform.") + std::to_string(platformID) + "." + key;
    dlog(tag, platform.getInfo<NAME>());
    //__android_log_write(ANDROID_LOG_DEBUG, tag.c_str(), value.c_str());
}

template <cl_int NAME>
auto logDeviceValue(cl::Device device) {
    return device.getInfo<NAME>();
}

std::string logBitfieldValue(std::vector<std::string> types) {
    std::string res = "";
    for (int i = 0; i < types.size(); ++i) {
        res += types[i];
        if (i < types.size() - 1) {
            res += " ";
        }
    }
    return res;
}

template <>
auto logDeviceValue<CL_DEVICE_TYPE>(cl::Device device) {
    std::vector<std::string> types;
    auto value = device.getInfo<CL_DEVICE_TYPE>();
    LOG_PUSH_BACK_FLAG(value, types, CL_DEVICE_TYPE_CPU);
    LOG_PUSH_BACK_FLAG(value, types, CL_DEVICE_TYPE_GPU);
    LOG_PUSH_BACK_FLAG(value, types, CL_DEVICE_TYPE_ACCELERATOR);
    LOG_PUSH_BACK_FLAG(value, types, CL_DEVICE_TYPE_CUSTOM);
    LOG_PUSH_BACK_FLAG(value, types, CL_DEVICE_TYPE_DEFAULT);
    LOG_PUSH_BACK_FLAG(value, types, CL_DEVICE_TYPE_ALL);
    return logBitfieldValue(types);
}

std::string logDeviceValueFPConfig(cl_device_fp_config value) {
    std::vector<std::string> types;
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_DENORM);
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_INF_NAN);
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_ROUND_TO_INF);
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_ROUND_TO_ZERO);
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_ROUND_TO_NEAREST);
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_FMA);
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT);
    LOG_PUSH_BACK_FLAG(value, types, CL_FP_SOFT_FLOAT);
    return logBitfieldValue(types);
}

template <>
auto logDeviceValue<CL_DEVICE_SINGLE_FP_CONFIG>(cl::Device device) {
    auto value = device.getInfo<CL_DEVICE_SINGLE_FP_CONFIG>();
    return logDeviceValueFPConfig(value);
}
template <>
auto logDeviceValue<CL_DEVICE_DOUBLE_FP_CONFIG>(cl::Device device) {
    auto value = device.getInfo<CL_DEVICE_DOUBLE_FP_CONFIG>();
    return logDeviceValueFPConfig(value);
}

std::string logDeviceMemCacheType(cl_device_mem_cache_type value) {
    std::vector<std::string> types;
    LOG_PUSH_BACK_EQ(value, types, CL_NONE);
    LOG_PUSH_BACK_EQ(value, types, CL_READ_ONLY_CACHE);
    LOG_PUSH_BACK_EQ(value, types, CL_READ_WRITE_CACHE);
    return logBitfieldValue(types);
}

template<>
auto logDeviceValue<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE>(cl::Device device) {
    auto value = device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_TYPE>();
    return logDeviceMemCacheType(value);
}

std::string logDeviceLocalMemType(cl_device_local_mem_type value) {
    std::vector<std::string> types;
    LOG_PUSH_BACK_EQ(value, types, CL_LOCAL);
    LOG_PUSH_BACK_EQ(value, types, CL_GLOBAL);
    LOG_PUSH_BACK_EQ(value, types, CL_NONE);
    return logBitfieldValue(types);
}

template<>
auto logDeviceValue<CL_DEVICE_LOCAL_MEM_TYPE>(cl::Device device) {
    auto value = device.getInfo<CL_DEVICE_LOCAL_MEM_TYPE>();
    return logDeviceLocalMemType(value);
}

std::string logDeviceExecutionCapabilities(cl_device_exec_capabilities value) {
    std::vector<std::string> types;
    LOG_PUSH_BACK_FLAG(value, types, CL_EXEC_KERNEL);
    LOG_PUSH_BACK_FLAG(value, types, CL_EXEC_NATIVE_KERNEL);
    return logBitfieldValue(types);
}

template<>
auto logDeviceValue<CL_DEVICE_EXECUTION_CAPABILITIES>(cl::Device device) {
    auto value = device.getInfo<CL_DEVICE_EXECUTION_CAPABILITIES>();
    return logDeviceExecutionCapabilities(value);
}

template<cl_int NAME>
void logDeviceInfoItem(cl::Device device, int deviceID, int platformID, std::string key) {
    std::string tag = std::string("CLInfo.Platform.") + std::to_string(platformID) + ".Device."
            + std::to_string(deviceID) + "." + key;
    dlog(tag, logDeviceValue<NAME>(device));
}

void logDeviceInfo(cl::Device device, int id, int platformID) {
    LOG_DEVICE_INFO(CL_DEVICE_TYPE, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_VENDOR_ID, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_MAX_COMPUTE_UNITS, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_MAX_WORK_ITEM_SIZES, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_MAX_WORK_GROUP_SIZE, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_MAX_CLOCK_FREQUENCY, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_ADDRESS_BITS, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_MAX_MEM_ALLOC_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_IMAGE_SUPPORT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_MAX_READ_IMAGE_ARGS, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_MAX_WRITE_IMAGE_ARGS, device, id, platformID);

    //LOG_DEVICE_INFO(CL_DEVICE_MAX_READ_WRITE_IMAGE_ARGS, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_IL_VERSION, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_IMAGE2D_MAX_WIDTH, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_IMAGE2D_MAX_HEIGHT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_IMAGE3D_MAX_WIDTH, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_IMAGE3D_MAX_HEIGHT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_IMAGE3D_MAX_DEPTH, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_IMAGE_MAX_BUFFER_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_IMAGE_MAX_ARRAY_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_MAX_SAMPLERS, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_IMAGE_PITCH_ALIGNMENT, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_IMAGE_BASE_ADDRESS_ALIGNMENT, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_MAX_PIPE_ARGS, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_PIPE_MAX_PACKET_SIZE, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_MAX_PARAMETER_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_MEM_BASE_ADDR_ALIGN, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_SINGLE_FP_CONFIG, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_DOUBLE_FP_CONFIG, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_GLOBAL_MEM_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_MAX_CONSTANT_ARGS, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_MAX_GLOBAL_VARIABLE_SIZE, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_GLOBAL_VARIABLE_PREFERRED_TOTAL_SIZE, device, id, platformID);

    LOG_DEVICE_INFO(CL_DEVICE_LOCAL_MEM_TYPE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_LOCAL_MEM_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_ERROR_CORRECTION_SUPPORT, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PROFILING_TIMER_RESOLUTION, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_ENDIAN_LITTLE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_AVAILABLE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_COMPILER_AVAILABLE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_LINKER_AVAILABLE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_EXECUTION_CAPABILITIES, device, id, platformID);
    //TODO LOG_DEVICE_INFO(CL_DEVICE_QUEUE_PROPERTIES, device, id, platformID);

    //LOG_DEVICE_INFO(CL_DEVICE_QUEUE_ON_HOST_PROPERTIES, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_QUEUE_ON_DEVICE_PROPERTIES, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_QUEUE_ON_DEVICE_PREFERRED_SIZE, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_QUEUE_ON_DEVICE_MAX_SIZE, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_MAX_ON_DEVICE_QUEUES, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_MAX_ON_DEVICE_EVENTS, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_BUILT_IN_KERNELS, device, id, platformID);
    //FIXME LOG_DEVICE_INFO(CL_DEVICE_PLATFORM, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_NAME, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_VENDOR, device, id, platformID);
    LOG_DEVICE_INFO(CL_DRIVER_VERSION, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PROFILE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_VERSION, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_OPENCL_C_VERSION, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_EXTENSIONS, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PRINTF_BUFFER_SIZE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_INTEROP_USER_SYNC, device, id, platformID); //TODO getting weird results from this...
    //FIXME LOG_DEVICE_INFO(CL_DEVICE_PARENT_DEVICE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_PARTITION_MAX_SUB_DEVICES, device, id, platformID);
    //TODO LOG_DEVICE_INFO(CL_DEVICE_PARTITION_PROPERTIES, device, id, platformID);
    //TODO LOG_DEVICE_INFO(CL_DEVICE_PARTITION_AFFINITY_DOMAIN, device, id, platformID);
    //TODO LOG_DEVICE_INFO(CL_DEVICE_PARTITION_TYPE, device, id, platformID);
    LOG_DEVICE_INFO(CL_DEVICE_REFERENCE_COUNT, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_SVM_CAPABILITIES, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_GLOBAL_ATOMIC_ALIGNMENT, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_PREFERRED_LOCAL_ATOMIC_ALIGNMENT, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_MAX_NUM_SUB_GROUPS, device, id, platformID);
    //LOG_DEVICE_INFO(CL_DEVICE_SUB_GROUP_INDEPENDENT_FORWARD_PROGRESS, device, id, platformID);
}

void logPlatformInfo(cl::Platform platform, int id) {
    LOG_PLATFORM_INFO(CL_PLATFORM_PROFILE, platform, id);
    LOG_PLATFORM_INFO(CL_PLATFORM_VERSION, platform, id);
    LOG_PLATFORM_INFO(CL_PLATFORM_NAME, platform, id);
    LOG_PLATFORM_INFO(CL_PLATFORM_VENDOR, platform, id);
    LOG_PLATFORM_INFO(CL_PLATFORM_EXTENSIONS, platform, id);

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.size() == 0) {
        __android_log_write(ANDROID_LOG_ERROR, "CLInfo.ERROR", "No OpenCL Devices Found.");
    }

    for (int i = 0; i < devices.size(); ++i) {
        logDeviceInfo(devices[i], i, id);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_joshuasterner_openclstats_CLInfo_logInfo(
        JNIEnv* env,
        jobject clInfo /* this */) {

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.size() == 0) {
        __android_log_write(ANDROID_LOG_ERROR, "CLInfo.ERROR", "No OpenCL Platforms Found.");
    }

    for (int i = 0; i < platforms.size(); ++i) {
        logPlatformInfo(platforms[i], i);
    }
}
