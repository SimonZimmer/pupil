#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include <seeleCore/Engine.h>
#include <seeleCore/IMemberParameterSet.h>


class NewProjectAudioProcessor : public juce::AudioProcessor
{
public:
    NewProjectAudioProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAudioProcessorValueTreeState();

    void getLatestAudioBlock(float* data, int numSamples);

private:
    void pushNextAudioBlock(const float* data, int numSamples);

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessor)

    juce::AudioProcessorValueTreeState parameters_;
    hidonash::MemberParameterSetPtr memberParameterSet_;

    int currentProgram_;
    hidonash::AudioProcessorPtr engine_;

    juce::AudioBuffer<float> visualizationBuffer_;
    std::mutex bufferMutex_;
};
