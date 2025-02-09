#include <gtest/gtest.h>

#include <seeleCore/Synthesis.h>

#include <AnalysisMock.h>


namespace hidonash
{
    using namespace testing;

    class UnitTest_Synthesis : public ::testing::Test
    {
    public:
        void SetUp() override
        {
            analysisMock_ = std::make_unique<NiceMock<AnalysisMock>>();
            analysisMockPtr_ = analysisMock_.get();
        }

        void TearDown() override
        {
            analysisMock_ = nullptr;
        }

        std::unique_ptr<AnalysisMock> analysisMock_;
        AnalysisMock* analysisMockPtr_;
    };

    TEST_F(UnitTest_Synthesis, process)
    {
        auto&& pitchShifter = Synthesis(44100, std::move(analysisMock_));
        auto&& fftWorkspace = std::array<juce::dsp::Complex<float>, 2 * config::constants::analysisSize>();

        {
            InSequence sequence;
            EXPECT_CALL(*analysisMockPtr_, perform(_)).Times(1);
        }

        pitchShifter.perform(fftWorkspace.data(), 0.5f);
    }
}
