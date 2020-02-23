#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <variant>
#include <vector>

namespace Testing
{
    class TestMachine
    {
    public:
        TestMachine();
        ~TestMachine();

        class TestResult
        {
        public:
            class Line
            {
            public:
                double Timestamp;
                std::string Content;
            };
            std::vector<Line> Lines;
            std::string GetAllLines();
            std::string GetAllLinesDuring(double DurationStart, double DurationEnd);
        };

        TestResult Test(std::string Input);
        void RunInCLI();
    protected:
        int ReadInt(std::string Hint);
        long ReadLong(std::string Hint);
        float ReadFloat(std::string Hint);
        double ReadDouble(std::string Hint);
        std::string ReadWord(std::string Hint);
        std::string ReadLine(std::string Hint);

        void WriteLine(std::variant<int, long, float, double, char, std::string>);
        void WriteLine(std::initializer_list<std::variant<int, long, float, double, char, std::string>>);

        /// @brief The single-thread function to be implemented that runs the test machine
        ///
        /// This function should always be blocked by a InternalIO Read somewhere
        /// and provide output only using InternalIO WriteLines.
        virtual void Process() = 0;
    private:
        class TerminateException;

        std::mutex Mutex;
        std::condition_variable ConditionVariable;
        bool WaitingToRead;
        bool IsTesting;

        std::string LastHint;
        std::queue<char> ReadQueue;
        std::queue<std::pair<double, std::string>> WriteQueue;

        std::chrono::time_point<std::chrono::steady_clock> StartTime;

        void Write(std::variant<int, long, float, double, char, std::string>);

        void Start();
        void Terminate();

        void ResetStartTime();
        double GetTime();
    };
}
