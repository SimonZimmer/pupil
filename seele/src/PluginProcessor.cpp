#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_core/juce_core.h"
#include "seeleCore/MemberParameterSet.h"

#include <seeleCore/Config.h>
#include <seeleCore/Factory.h>

#include <core/AudioBuffer.h>

NewProjectAudioProcessor::NewProjectAudioProcessor()
: AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true))
, parameters_(*this, nullptr, "Parameters", createParameters())
, memberParameterSet_(std::make_unique<hidonash::MemberParameterSet>(parameters_))
, currentProgram_(1)
{
    //TODO: add actual presets
    programs_.emplace_back("program one");
    programs_.emplace_back("program two");
    programs_.emplace_back("program three");
}

const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
    return false;
}

bool NewProjectAudioProcessor::producesMidi() const
{
    return false;
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
    return false;
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return programs_.size();
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return currentProgram_;
}

void NewProjectAudioProcessor::setCurrentProgram(int index)
{
    currentProgram_ = index;
}

const juce::String NewProjectAudioProcessor::getProgramName(int index)
{
    return juce::String(programs_.at(index));
}

void NewProjectAudioProcessor::changeProgramName(int index, const juce::String& newName)
{}

void NewProjectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    engine_ =
        hidonash::Factory().createEngine(*memberParameterSet_, sampleRate, samplesPerBlock, getTotalNumInputChannels());

    visualizationBuffer_.setSize(getTotalNumInputChannels(), samplesPerBlock);
}

void NewProjectAudioProcessor::releaseResources()
{}

bool NewProjectAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return !(layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet());
}

void NewProjectAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto&& inputBuffer =
        hidonash::core::AudioBuffer(buffer.getArrayOfWritePointers(), static_cast<int>(buffer.getNumChannels()),
                                    static_cast<int>(buffer.getNumSamples()));

    engine_->process(inputBuffer);

    buffer.copyFrom(0, 0, inputBuffer.getDataPointer(), inputBuffer.getNumSamples());
    buffer.copyFrom(1, 0, inputBuffer.getDataPointer(), inputBuffer.getNumSamples());

    const std::lock_guard<std::mutex> lock(bufferMutex_);
    pushNextAudioBlock(buffer.getReadPointer(0), buffer.getNumSamples());
}

bool NewProjectAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor(*this);
}

void NewProjectAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{}

void NewProjectAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}

void NewProjectAudioProcessor::pushNextAudioBlock(const float* data, int numSamples)
{
    auto* bufferToWrite = visualizationBuffer_.getWritePointer(0);
    std::memcpy(bufferToWrite, data, numSamples * sizeof(float));
}

juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameters()
{
    auto parameters = std::vector<std::unique_ptr<juce::RangedAudioParameter>>();

    for (auto n = 0; n < hidonash::config::constants::numMembers; ++n)
    {
        parameters.emplace_back(std::make_unique<juce::AudioParameterFloat>(
            hidonash::config::parameters::sanctityPrefix + std::to_string(n),
            "Seele " + std::to_string(n + 1) + " Sanctity", hidonash::config::parameters::minPitchFactor,
            hidonash::config::parameters::maxPitchFactor, hidonash::config::parameters::defaultPitchFactor));
        parameters.emplace_back(std::make_unique<juce::AudioParameterBool>(
            hidonash::config::parameters::summonStatePrefix + std::to_string(n),
            "Seele " + std::to_string(n + 1) + " Summoned", true));
        parameters.emplace_back(std::make_unique<juce::AudioParameterFloat>(
            hidonash::config::parameters::distancePrefix + std::to_string(n),
            "Seele " + std::to_string(n + 1) + " Distance", hidonash::config::parameters::minDistanceInSeconds,
            hidonash::config::parameters::maxDistanceInSeconds,
            hidonash::config::parameters::defaultDistanceInSeconds));
        parameters.emplace_back(std::make_unique<juce::AudioParameterFloat>(
            hidonash::config::parameters::levelPrefix + std::to_string(n), "Seele " + std::to_string(n + 1) + " Level",
            hidonash::config::parameters::minLevelDb, hidonash::config::parameters::maxLevelDb,
            hidonash::config::parameters::defaultLevelDb));
    }

    return {parameters.begin(), parameters.end()};
}

juce::AudioProcessorValueTreeState& NewProjectAudioProcessor::getAudioProcessorValueTreeState()
{
    return parameters_;
}

void NewProjectAudioProcessor::getLatestAudioBlock(float* data, int numSamples)
{
    const std::lock_guard<std::mutex> lock(bufferMutex_);

    auto* bufferToRead = visualizationBuffer_.getReadPointer(0);
    std::memcpy(data, bufferToRead, numSamples * sizeof(float));
}
