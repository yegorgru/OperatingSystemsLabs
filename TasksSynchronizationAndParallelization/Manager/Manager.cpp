#include "Manager.h"

#include <iostream>
#include <sstream>

#include <boost/chrono.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace {
    void resetKeyStates() {
        std::vector<char> keys{ 13, 78, 89, 27, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57 };
        for (auto key : keys) {
            GetAsyncKeyState(key) & 0x0001;
        }
    }

    template <typename T>
    T readNumberInput()
    {
        resetKeyStates();
        std::stringstream ss;
        while (!(GetAsyncKeyState(13) & 0x0001)) {
            for (int i = 48; i <= 57; i++) {
                if (GetAsyncKeyState(i) & 0x0001) {
                    char cur = static_cast<char>(i);
                    ss << cur;
                    std::cout << cur;
                }
            }
            if ((GetAsyncKeyState(8) & 0x0001) && ss.str().size() != 0) {
                std::cout << "\b \b";
            }
        }
        std::cout << std::endl;
        T result = 0;
        ss >> result;
        return result;
    }

    enum class ConfirmationResult {
        Yes,
        No,
        Timeout
    };

    ConfirmationResult confirm(const std::string& message, uint32_t seconds)
    {
        resetKeyStates();
        std::cout << message << std::endl;
        auto start = boost::chrono::steady_clock::now();
        while (true) {
            if (GetAsyncKeyState(89) & 0x0001) {
                return ConfirmationResult::Yes;
            }
            else if (GetAsyncKeyState(78) & 0x0001) {
                return ConfirmationResult::No;
            }
            else {
                auto now = boost::chrono::steady_clock::now();
                if (boost::chrono::duration_cast<boost::chrono::seconds>(now - start).count() > seconds) {
                    return ConfirmationResult::Timeout;
                }
            }
        }
    }
}

Manager::Manager()
    : mF("f.exe")
    , mG("g.exe")
    , mFResults{-1, -1}
    , mGResults{ -1, -1 }
{

}

void Manager::run()
{
    while (true) {
        std::cout << "Enter x: " << std::endl;
        auto x = readNumberInput<int>();

        std::cout << "Enter amount of attempts in case of soft fail: " << std::endl;
        auto amountOfAttempts = readNumberInput<uint32_t>();
        amountOfAttempts = amountOfAttempts == 0 ? 1 : amountOfAttempts;

        performSingleComputation(x, amountOfAttempts);

        switch (confirm("Please confirm performing of next computation y(es, perform)/n(o, exit) [n]", 30)) {
        case ConfirmationResult::Yes: {
            continue;
        }
        case ConfirmationResult::No: {
            return;
        }
        case ConfirmationResult::Timeout: {
            std::cout << "Next computation is not confirmed within 30s, closing" << std::endl;
            return;
        }
        }
    }
}

void Manager::performSingleComputation(int x, uint32_t amountOfAttempts)
{
    mFResults = {-1, -1};
    mGResults = { -1, -1 };
    for (uint32_t i = 0; i < amountOfAttempts; i++) {
        std::cout << "Computations have started. Attempt #" << i + 1 << std::endl;
        if (mFResults.first == -1) {
            mF.start();
            mF.write<int>(x);
        }
        if (mGResults.first == -1) {
            mG.start();
            mG.write<int>(x);
        }
        resetKeyStates();
        while (mF.running() || mG.running()) {
            if (mFResults.first == -1 && !mF.running()) {
                getResults(true);
                if (mFResults.first == 2) {
                    reportResults();
                    return;
                }
            }
            if (mGResults.first == -1 && !mG.running()) {
                getResults(false);
                if (mGResults.first == 2) {
                    reportResults();
                    return;
                }
            }
            if (GetAsyncKeyState(27) & 0x0001) {
                switch (confirm("Please confirm that computation should be stopped y(es, stop)/n(ot yet) [n]", 5)) {
                case ConfirmationResult::Yes: {
                    if (!mF.running() && mFResults.first == -1) {
                        getResults(true);
                    }
                    if (!mG.running() && mGResults.first == -1) {
                        getResults(false);
                    }
                    if (mFResults.first == 2 || mGResults.first == 2 || mFResults.first == 0 && mGResults.first == 0) 
                    {
                        std::cout << "overridden by system" << std::endl;
                        reportResults();
                        return;
                    }
                    std::cout << "Computation was canceled." << std::endl;
                    if (mF.running()) {
                        std::cout << "f did not finish. " << std::endl;
                        mF.terminate();
                    }
                    if (mG.running()) {
                        std::cout << "g did not finish. " << std::endl;
                        mG.terminate();
                    }
                    reportResults();
                    return;
                }
                case ConfirmationResult::No: {
                    std::cout << "Action was not confirmed" << std::endl;
                    break;
                }
                case ConfirmationResult::Timeout: {
                    std::cout << "Action is not confirmed within 5s. proceeding..." << std::endl;
                    break;
                }
                }
            }
        }
        if (mFResults.first == -1) {
            getResults(true);
        }
        if (mGResults.first == -1) {
            getResults(false);
        }
        reportResults();
        if (!(mFResults.first == 1 && mGResults.first != 2 || mFResults.first != 2 && mGResults.first == 1)) {
            return;
        }
        mFResults.first = mFResults.first == 1 ? -1 : mFResults.first;
        mGResults.first = mGResults.first == 1 ? -1 : mGResults.first;
    }
    std::cout << "Maximum amount of attempts is reached" << std::endl;
}

void Manager::reportResults() {
    if (mFResults.first == 0 && mGResults.first == 0) {
        std::cout << "Result: " << static_cast<int64_t>(mFResults.second) + mGResults.second << std::endl;
    }
    if (mFResults.first == 1) {
        std::cout << "f function failed, soft" << std::endl;
    }
    if (mGResults.first == 1) {
        std::cout << "g function failed, soft" << std::endl;
    }
    if (mFResults.first == 2) {
        std::cout << "f function failed, hard" << std::endl;
    }
    if (mGResults.first == 2) {
        std::cout << "g function failed, hard" << std::endl;
    }
}

void Manager::getResults(bool f)
{
    if (f) {
        mFResults.first = mF.read<int>();
        if (mFResults.first == 0) {
            mFResults.second = mF.read<int>();
        }
    }
    else {
        mGResults.first = mG.read<int>();
        if (mGResults.first == 0) {
            mGResults.second = mG.read<int>();
        }
    }
}