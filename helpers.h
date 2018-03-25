#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <tuple>
#include <vector>

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

	template<typename T>
	struct identity_producer
	{
		auto operator() () const
		{
			return std::make_tuple(T{}, [](T &v, auto &c) { v = c; }, [](T &v) ->T& { return v; });
		}
	};

	struct string_producer
	{
		using Type = std::stringstream;
		auto operator() () const
		{
			return std::make_tuple(Type{},
				[](Type& v, auto&& c) { v << c; },
				[](Type& v) { return v.str(); });
		}
	};

	template<typename T>
	struct vector_producer
	{
		using Type = std::vector<T>;
		auto operator() () const
		{
			return std::make_tuple(Type{},
				[](Type& v, auto& c) { v.push_back(c); },
				[](Type& v) { return v; });
		}
	};


	template<typename Producer, typename Value>
	void apply_to_product(Producer&& p, Value&& v)
	{
		auto &halfproduct = std::get<0>(p);
		auto &consumer = std::get<1>(p);
		consumer(halfproduct, std::forward<Value>(v));
	}

	template<typename Producer>
	auto final_product(Producer&& p) //TODO forwarding return type
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