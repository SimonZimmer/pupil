#include <random>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <AudioBufferMock.h>
#include <DelayProcessorMock.h>
#include <FactoryMock.h>
#include <GainProcessorMock.h>
#include <MemberParameterSetMock.h>
#include <PitchShifterManagerMock.h>

#include <seeleCore/Engine.h>


namespace hidonash
{
    using namespace testing;

    class UnitTest_Engine : public ::testing::Test
    {
    public:
        void SetUp() override
        {
            inputBuffer_ = std::make_unique<core::AudioBuffer>(2, 64);

            std::vector<float> randomValues(2 * 64);

            std::random_device rd;
            std::mt19937 gen(1);
            std::uniform_real_distribution<float> dis(-1.0f, 1.0f);

            for (float& value: randomValues)
                value = dis(gen);

            int index = 0;
            for (int ch = 0; ch < 2; ++ch)
                for (int sa = 0; sa < 64; ++sa)
                    inputBuffer_->setSample(ch, sa, randomValues[index++]);
        }

        void TearDown() override
        {}

        std::unique_ptr<core::AudioBuffer> inputBuffer_;
    };

    TEST_F(UnitTest_Engine, process)
    {
        auto memberParameterSetMock = MemberParameterSetMock();
        EXPECT_CALL(memberParameterSetMock, getSanctity).WillRepeatedly(Return(0.5f));
        EXPECT_CALL(memberParameterSetMock, getSummonState).WillRepeatedly(Return(true));
        EXPECT_CALL(memberParameterSetMock, getGain).WillRepeatedly(Return(6.0f));
        EXPECT_CALL(memberParameterSetMock, getDistance).WillRepeatedly(Return(0.0f));
        auto engine = Engine(memberParameterSetMock, 44100., 1024, 2);

        for (auto n = 1; n < 10; ++n)
            engine.process(*inputBuffer_);

        EXPECT_FLOAT_EQ(inputBuffer_->getSample(0, 0), 0.000154527879f);
        EXPECT_FLOAT_EQ(inputBuffer_->getSample(1, 0), 0.000154527879f);
        EXPECT_FLOAT_EQ(inputBuffer_->getSample(0, 32), -0.000111830588f);
        EXPECT_FLOAT_EQ(inputBuffer_->getSample(1, 32), -0.000111830588f);
        EXPECT_FLOAT_EQ(inputBuffer_->getSample(0, 63), -0.000168549144f);
        EXPECT_FLOAT_EQ(inputBuffer_->getSample(1, 63), -0.000168549144f);
    }
}
