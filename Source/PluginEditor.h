#pragma once

#include "PluginProcessor.h"

// ============================================================
//  画面。ボタン1つと、文字表示1つだけ。
// ============================================================
class NamPlayerEditor : public juce::AudioProcessorEditor
{
public:
    explicit NamPlayerEditor (NamPlayerProcessor&);

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    NamPlayerProcessor& processor;

    juce::TextButton loadButton { "Load .nam" };
    juce::Label statusLabel;
    std::unique_ptr<juce::FileChooser> chooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NamPlayerEditor)
};
