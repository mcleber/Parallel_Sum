#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std::chrono;
using namespace std::chrono_literals;

using ClockT = high_resolution_clock;

int main()
{
	//setlocale(LC_ALL, "pt-BR");

	std::cout << "Initializing ... " << std::flush;
	const int N{ 380'000'000 };

	std::vector<int> Numbers(N, 0);
	std::cout << "Done" << std::endl;

	std::cout << "----------------------------------------" << std::endl;

	// Generate random numbers
	std::cout << "Generating random numbers ... " << std::flush;
	auto StartTime = ClockT::now();
	std::random_device RandomDevice;
	std::uniform_int_distribution<> Distribution(0, 100);
	for (int& X : Numbers)
	{
		X = Distribution(RandomDevice);
	}
	std::cout << "Done" << std::endl;

	auto EndTime = ClockT::now();
	auto Elapsed = EndTime - StartTime;
	std::cout << "Time generating numbers: " << duration_cast<milliseconds>(Elapsed).count() << " ms\n";

	std::cout << "----------------------------------------" << std::endl;

	{
		// Serial scope
		std::cout << "Initializing serial sum" << std::endl;
		StartTime = ClockT::now();
		const int Sum = std::accumulate(Numbers.begin(), Numbers.end(), 0);
		EndTime = ClockT::now();
		Elapsed = EndTime - StartTime;

		std::cout << "Sum : " << Sum << "  |  " << "Time: " << duration_cast<milliseconds>(Elapsed).count() << " ms\n";
	}

	std::cout << "----------------------------------------" << std::endl;

	{
		// Parallel scope
		std::cout << "Initializing parallel sum" << std::endl;
		for (int unsigned NumThreads = 1; NumThreads <= std::thread::hardware_concurrency(); ++NumThreads)
		{
			std::atomic<int> Sum{ 0 };

			auto ThreadFunc = [&](int StartIndex, int EndIndex)
			{
				int LocalSum{ 0 };
				for (int i = StartIndex; i < EndIndex; ++i)
				{
					LocalSum += Numbers[i];
				}
				Sum += LocalSum;
			};

			std::vector<std::thread> Threads;

			StartTime = ClockT::now();

			const int NumbersPerThreads = N / NumThreads;
			for (int unsigned ThreadId = 0; ThreadId < NumThreads; ++ThreadId)
			{
				const int StartIndex = ThreadId * NumbersPerThreads;
				int EndIndex = StartIndex + NumbersPerThreads;

				if (ThreadId == NumThreads - 1)
				{
					EndIndex = std::min(N, EndIndex);
					EndIndex = std::max(N, EndIndex);
				}

				Threads.emplace_back(ThreadFunc, StartIndex, EndIndex);
			}

			for (std::thread& Thread : Threads)
			{
				Thread.join();
			}

			EndTime = ClockT::now();
     			Elapsed = EndTime - StartTime;
			

			std::cout << "Sum : " << Sum << "  |  Threads : " << NumThreads << " - Time: " << duration_cast<milliseconds>(Elapsed).count() << " ms" << std::endl;
		}
		
	}

	std::cout << std::endl;

	system("PAUSE");
	return 0;
}
