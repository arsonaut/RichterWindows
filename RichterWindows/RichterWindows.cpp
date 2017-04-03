#include "stdafx.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter3.h"
#include "Chapter4.h"
#include "Chapter6.h"
#include "SWMRG.h"

void TestChapter1()
{
    std::cout << ">>> Chapter 1" << std::endl;
    const auto& message1 = chapter1::GetFormattedMessage(106);
    std::wcout << message1;
    const auto& message2 = chapter1::GetFormattedMessage(2);
    std::wcout << message2;
}

void TestChapter2()
{
    std::cout << ">>> Chapter 2" << std::endl;
    const auto& wideliteral = L"You are so na\u00EFve \U0001F609";
    const auto& bytestr = chapter2::wstring2string(wideliteral);
    const auto& widestr = chapter2::string2wstring(bytestr);
    if (widestr == wideliteral)
    {
        std::wcout << "String is the same after double conversion" << std::endl;
    }
    else
    {
        std::wcout << "String is NOT the same after double conversion" << std::endl;
    }
}

void TestChapter3()
{
    std::cout << ">>> Chapter 3" << std::endl;
    chapter3::DuplicateHandleForChildren(::GetCurrentThread());
}

void TestChapter4()
{
    std::cout << ">>> Chapter 4" << std::endl;
    const auto exitCode = chapter4::DuplicateHandleCheckExitCode(::GetCurrentThread());
    std::cout << "Children exit code = " << exitCode << std::endl;
}

void TestChapters6_7_8_9()
{
    std::cout << ">>> Chapters 6-9" << std::endl;
    uint64_t prime{1844674407370955};
    if (chapter6::GetNextPrime(prime) && chapter7::PrintNextPrime(prime))
    {
        chapter8::CriticalSectionWrapper cs;
        if (chapter8::PrintSyncedNextPrime(prime, cs.get()))
        {
            bool allowed{true};
            chapter9::EventWrapper event{allowed};
            for (size_t n = 0; n < 10 && chapter9::PrintConditionallyNextPrime(prime, event.get()); ++n)
            {
                allowed = !allowed;
                event.setState(allowed);
            }
        }
    }
}

void TestChapter10()
{
    std::cout << ">>> Chapter 10" << std::endl;
    CSWMRG swmrg;
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 20; ++i)
    {
        threads.emplace_back([&swmrg, i]()
        {
            std::default_random_engine rand{std::random_device{}()};
            std::uniform_int_distribution<int> chooser{0, 99};
            if (chooser(rand) < 60) // 60% chance to become reader
            {
                swmrg.WaitToRead();
                std::this_thread::sleep_for(std::chrono::milliseconds{chooser(rand)});
                if (chooser(rand) < 35) // 35% chance to upgrade to writer
                {
                    swmrg.UpgradeToWrite();
                    std::this_thread::sleep_for(std::chrono::milliseconds{chooser(rand)});
                    std::cout << "Thread #" << i << ", reader upgraded to writer" << std::endl;
                }
                swmrg.Done();
            }
            else
            {
                swmrg.WaitToWrite();
                std::this_thread::sleep_for(std::chrono::milliseconds{chooser(rand)});
                std::cout << "Thread #" << i << " is a writer" << std::endl;
                swmrg.Done();
            }
        });
    }
    for (auto&& thread : threads)
    {
        thread.join();
    }
}

int main()
{
    try
    {
        TestChapter1();
        TestChapter2();
        TestChapter3();
        TestChapter4();
        TestChapters6_7_8_9();
        TestChapter10();
    }
    catch (const std::exception& e)
    {
        std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
        return 1;
    }
    return 0;
}