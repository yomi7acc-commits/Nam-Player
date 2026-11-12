#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include "NAM/dsp.h"

// ============================================================
//  音の処理をする本体。DAWやStandaloneアプリから呼ばれる。
// ============================================================
class NamPlayerProcessor : public juce::AudioProcessor
{
public:
    NamPlayerProcessor();

    // 再生が始まる直前に1回呼ばれる（サンプルレートなどが決まるタイミング）
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}

    // 音の小さなかたまり（ブロック）ごとに何度も呼ばれる
    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;

    // .namファイルを読み込む。成功したら true を返す
    bool loadModel (const juce::File& file);

    // ---- 以下はJUCEが要求する決まり文句（演習の対象外） ----
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "NAM Player"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override {}
    void setStateInformation (const void*, int) override {}

private:
    // 読み込んだアンプモデル（まだ無いときは空っぽ = nullptr）
    /* ＿＿ 空欄① ＿＿ */

    // モデルの差し替え中に音声処理が触らないようにする鍵
    juce::SpinLock modelLock;

    // NAMの出力を一旦受け取る置き場
    std::vector<float> outputBuffer;

    double currentSampleRate = 48000.0;
    int currentBlockSize = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamPlayerProcessor)
};
