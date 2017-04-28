set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 23)
set(CMAKE_ANDROID_ARM_MODE ON)
set(CMAKE_ANDROID_ARM_NEON ON)
set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
set(CMAKE_ANDROID_NDK /opt/android-ndk-r14b)
set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION clang)
set(CMAKE_ANDROID_STL_TYPE c++_static)
set(CMAKE_SHARED_LINKER_FLAGS -static-libstdc++)

add_definitions(-DANDROID)
