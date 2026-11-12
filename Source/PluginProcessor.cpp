#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "NAM/get_dsp.h"

NamPlayerProcessor::NamPlayerProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
{
}

// ------------------------------------------------------------
//  .namファイルの読み込み（ボタンを押したときに呼ばれる）
// ------------------------------------------------------------
bool NamPlayerProcessor::loadModel (const juce::File& file)
{
    // JUCEのファイルを、NAMが受け取れる形のパスに変換
    const std::filesystem::path path (file.getFullPathName().toWideCharPointer());

    std::unique_ptr<nam::DSP> newModel;
    try
    {
        newModel = /* ＿＿ 空欄② ＿＿ */;
    }
    catch (const std::exception&)
    {
        return false; // 壊れたファイルなど
    }

    if (newModel == nullptr)
        return false;

    // 今の再生条件に合わせて準備させる
    /* ＿＿ 空欄③ ＿＿ */

    // 鍵をかけてから、古いモデルと新しいモデルを入れ替える
    {
        const juce::SpinLock::ScopedLockType lock (modelLock);
        std::swap (model, newModel);
    }
    return true; // ここで newModel(=古いモデル) が自動で片付けられる
}

void NamPlayerProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize = samplesPerBlock;
    outputBuffer.resize ((size_t) samplesPerBlock);

    const juce::SpinLock::ScopedLockType lock (modelLock);
    if (model != nullptr)
        model->Reset (currentSampleRate, currentBlockSize);
}

// ------------------------------------------------------------
//  音声処理の本体
// ------------------------------------------------------------
void NamPlayerProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();

    // 鍵が取れない（差し替え中）か、モデルが無いなら、何もせず素通し
    const juce::SpinLock::ScopedTryLockType lock (modelLock);
    if (! lock.isLocked() || model == nullptr || numSamples > (int) outputBuffer.size())
        return;

    // ギターは1本 = 左チャンネル(0番)だけを入力として使う
    float* input = /* ＿＿ 空欄④ ＿＿ */;
    float* output = outputBuffer.data();

    // NAMに変換してもらう（input → output）
    /* ＿＿ 空欄⑤ ＿＿ */

    // 変換結果を、左右すべてのチャンネルに書き戻す
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        /* ＿＿ 空欄⑥ ＿＿ */
}

juce::AudioProcessorEditor* NamPlayerProcessor::createEditor()
{
    return new NamPlayerEditor (*this);
}

// JUCEがプラグインを作るときに呼ぶ入口（決まり文句）
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NamPlayerProcessor();
}
