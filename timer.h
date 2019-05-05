#pragma once
#ifndef __TIMER_H__  
#define __TIMER_H__
#include<functional>
#include<chrono>
#include<thread>
#include<atomic>
#include<memory>
#include<mutex>
#include<condition_variable>

template<class T,template <class...> class TMP> constexpr bool is_specialization_v = false;
template<template <class...> class TMP,class... T> constexpr bool is_specialization_v<TMP<T...>, TMP> = true;
template<class T> constexpr bool is_duration_v = is_specialization_v<T, std::chrono::duration>;

class Timer
{
public:
	Timer() :expired_(true) {}
	~Timer(){
		Stop();
	}

	template<typename durt, typename = std::enable_if_t<is_duration_v<durt>>>
	void Start(durt interval, std::function<void()> task){
		expired_ = false;
		std::thread([&, interval, task]() {
			while (!expired_){
				std::unique_lock<std::mutex> locker(mutex_);
				if (expired_cond_.wait_for(locker, std::chrono::duration_cast<std::chrono::milliseconds>(interval), [&]() {return expired_ == true; })) break; else task();
			}
		}).detach();
	}

	void Stop()
	{
		expired_ = true;
		expired_cond_.notify_one();
	}

	template<typename durt, typename callable, class... arguments, typename = std::enable_if_t<is_duration_v<durt>> >
	void Sync(durt &&after, callable&& f, arguments&&... args){

		std::function<typename std::result_of<callable(arguments...)>::type()> \
			task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

		std::this_thread::sleep_for(std::forward<durt>(after));
		task();
	}

	template<typename durt, typename callable, class... arguments, typename = std::enable_if_t<is_duration_v<durt>>>
	void Async(durt after, callable&& f, arguments&&... args){

		std::async(std::launch::async, [&]() {
			std::this_thread::sleep_for(after);
			f(args...);
		});
	}
private:
	std::atomic<bool> expired_;
	std::mutex mutex_;
	std::condition_variable expired_cond_;
};
#endif //!__TIMER_H__