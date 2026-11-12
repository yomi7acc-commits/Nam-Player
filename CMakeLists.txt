# ============================================================
#  NAM Player - ビルド設定（ここは演習の対象外。触らなくてOK）
# ============================================================
cmake_minimum_required(VERSION 3.22)
project(NamPlayer VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# JUCE本体をネットから取ってくる
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.10
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(JUCE)

# NAMの推論ライブラリ（.namファイルを読んで音を変換する部分）
FetchContent_Declare(NAMCore
    GIT_REPOSITORY https://github.com/sdatkinson/NeuralAmpModelerCore.git
    GIT_TAG        d3189ed482ded880c128bf312941266c4fa5ceec
    GIT_SUBMODULES Dependencies/eigen)
FetchContent_Populate(NAMCore)   # ソースだけ取得（NAM側のCMakeは使わない）

file(GLOB NAM_SOURCES
    ${namcore_SOURCE_DIR}/NAM/*.cpp
    ${namcore_SOURCE_DIR}/NAM/*/*.cpp)

juce_add_plugin(NamPlayer
    COMPANY_NAME      "MyName"
    PLUGIN_MANUFACTURER_CODE Mynm
    PLUGIN_CODE       Namp
    FORMATS           Standalone VST3
    PRODUCT_NAME      "NAM Player")

target_sources(NamPlayer PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    ${NAM_SOURCES})

target_include_directories(NamPlayer PRIVATE
    ${namcore_SOURCE_DIR}
    ${namcore_SOURCE_DIR}/NAM
    ${namcore_SOURCE_DIR}/Dependencies/eigen
    ${namcore_SOURCE_DIR}/Dependencies/nlohmann)

target_compile_definitions(NamPlayer PRIVATE
    NAM_SAMPLE_FLOAT=1            # NAMにfloatで計算させる（JUCEのバッファと同じ型）
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(NamPlayer PRIVATE
    juce::juce_audio_utils
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags)
