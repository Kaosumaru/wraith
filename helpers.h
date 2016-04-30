#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <tuple>

namespace wraith
{
	template<typename T>
	bool done(T&& t)
	{
		return *t == 0;
	}

	template<typename T>
	auto get(T&& t)
	{
		return *t;
	}

	template<typename T>
	void increment(T& t)
	{
		t++;
	}


	struct empty_consumer
	{
		template<typename T>
		void operator() (T&& t)	 const { }
	};

	struct empty_producer
	{
		auto operator() () const
		{
			return std::make_tuple(int{}, [](int &v, auto &c) {}, [](int &v) { return v; });
		}
	};

	struct string_producer
	{
		auto operator() () const
		{
			return std::make_tuple(std::stringstream{},
				[](std::stringstream& v, auto& c) { v << c; },
				[](std::stringstream& v) { return v.str(); });
		}
	};


	template<typename Producer, typename Value>
	void consume(Producer&& p, Value&& v)
	{
		auto &halfproduct = std::get<0>(p);
		auto &consumer = std::get<1>(p);
		consumer(halfproduct, std::forward<Value>(v));
	}

	template<typename Producer>
	auto final_product(Producer&& p)
	{
		auto &halfproduct = std::get<0>(p);
		auto &finalize = std::get<2>(p);
		return finalize(halfproduct);
	}

	struct parsing_error : public std::logic_error
	{
		using logic_error::logic_error;
	};
}