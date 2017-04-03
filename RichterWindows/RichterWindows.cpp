#include "stdafx.h"
#include "Chapter1.h"
#include "Chapter2.h"
#include "Chapter6.h"
#include "SWMRG.h"

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
        const auto& message1 = chapter1::GetFormattedMessage(106);
        const auto& message2 = chapter1::GetFormattedMessage(2);
        const auto& bytestr = chapter2::wstring2string(L"You are so na\u00EFve \U0001F609");
        const auto& widestr = chapter2::string2wstring(bytestr);

        TestChapter10();

        uint64_t prime{18446744073709551437};
        if (chapter6::GetNextPrime(prime))
        {
            while (chapter7::PrintNextPrime(prime));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
        return 1;
    }
    return 0;
}