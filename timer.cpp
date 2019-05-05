#include <iostream>
#include <chrono>
#include <functional>
#include <string>
#include "timer.h"
int main()
{
	using namespace std;
	using namespace std::chrono_literals;

	Timer timer;
	auto f = [](std::string name) {
		cout << "hello timer: " << name << endl;
	};
	
	timer.Sync(1s, std::bind(f, "lambda1 1s"));
	timer.Async(1s, std::bind(f, "lambda2 1s"));
	timer.Start(1s, []() {cout << "hello period timer 1s!" << endl; });

	timer.Sync(0.01min, std::bind(f, "lambda1 0.01min"));
	timer.Async(0.01min, std::bind(f, "lambda2 0.01min"));

	std::this_thread::sleep_for(2s);

	cout << "stop timer 1s"  << endl;
	timer.Stop();
	timer.Start(0.01min, []() {cout << "hello period timer! 0.01min" << endl; });

	std::this_thread::sleep_for(2s);
	timer.Stop();
	cout << "stop timer 0.01s" << endl;
	cin.ignore();
}