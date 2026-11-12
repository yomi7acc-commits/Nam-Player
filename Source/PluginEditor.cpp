#include "PluginEditor.h"

NamPlayerEditor::NamPlayerEditor (NamPlayerProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    statusLabel.setText ("No model loaded", juce::dontSendNotification);
    statusLabel.setJustificationType (juce::Justification::centred);

    addAndMakeVisible (loadButton);
    addAndMakeVisible (statusLabel);

    // ボタンが押されたら、ファイル選択ダイアログを開く
    loadButton.onClick = [this]
    {
        chooser = std::make_unique<juce::FileChooser> ("Select a .nam file", juce::File{}, "*.nam");

        chooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this] (const juce::FileChooser& fc)
                              {
                                  const juce::File file = fc.getResult();
                                  if (file == juce::File{})
                                      return; // キャンセルされた

                                  const bool ok = /* ＿＿ 空欄⑦ ＿＿ */;

                                  statusLabel.setText (ok ? file.getFileName() : "Failed to load",
                                                       juce::dontSendNotification);
                              });
    };

    setSize (300, 120);
}

void NamPlayerEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkslategrey);
}

void NamPlayerEditor::resized()
{
    auto area = getLocalBounds().reduced (10);
    loadButton.setBounds (area.removeFromTop (40));
    statusLabel.setBounds (area);
}
