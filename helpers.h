#pragma once
#include <string>
#include <sstream>
#include <utility>
#include <tuple>
#include "rule.h"

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

	namespace detail
	{
		template<typename T1, typename T2>
		struct TwoTypes_h
		{
			using type = std::pair<T1, T2>;
		};

		template<>
		struct TwoTypes_h<empty, empty>
		{
			using type = empty;
		};

		template<typename T>
		struct TwoTypes_h<T, empty>
		{
			using type = T;
		};

		template<typename T>
		struct TwoTypes_h<empty, T>
		{
			using type = T;
		};

		template<typename T1, typename T2>
		using TwoTypes = typename TwoTypes_h<T1, T2>::type;

		template<typename Consumer, typename Product1, typename Product2>
		void ConsumeNonEmpty(Consumer& consumer, std::pair<Product1, Product2>& result)
		{
			if constexpr (std::is_same_v<Product1, empty> && std::is_same_v<Product2, empty>) {

			}
			else if constexpr (!std::is_same_v<Product1, empty> && !std::is_same_v<Product2, empty>) {
				consumer(std::move(result));
			}
			else if constexpr (!std::is_same_v<Product1, empty>) {
				consumer(std::move(result.first));
			}
			else if constexpr (!std::is_same_v<Product2, empty>) {
				consumer(std::move(result.second));
			}
		}
	}

}