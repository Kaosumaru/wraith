#pragma once
#include <functional>
#include <memory>
#include "helpers.h"
#include "rule.h"
#include <vector>

namespace wraith
{

	//matches given character 
	template<typename T>
	auto literal(T&& t)
	{
		return make_rule<empty>([=](auto& range, auto& consumer)
		{
			if (done(range) || get(range) != t)
				return false;

			increment(range);
			return true;
		});
	}

	template<typename T>
	auto character(T&& t)
	{
		using Product = std::remove_reference_t<T>;
		return make_rule<Product>([=](auto& range, auto& consumer)
		{
			if (done(range))
				return false;

			auto &&c = get(range);
			if (c != t)
				return false;

			consumer(std::move(c));
			increment(range);
			return true;
		});
	}

	template<std::size_t r1, std::size_t r2>
	auto character()
	{
		using Product = char;
		return make_rule<Product>([=](auto& range, auto& consumer)
		{
			static_assert(r1 <= r2);
			if (done(range))
				return false;

			auto &&c = get(range);
			if (c < r1 || c > r2)
				return false;

			consumer(std::move(c));
			increment(range);
			return true;
		});
	}


	template<typename R1, typename R2>
	auto follow(R1 r1, R2 r2)
	{
		using Product1 = typename R1::ProductType;
		using Product2 = typename R2::ProductType;
		using Product = detail::TwoTypes<Product1, Product2>;
		return make_rule<Product>([=](auto& range, auto& consumer)
		{

			std::pair<Product1, Product2> result;
			auto subconsumer1 = [&](auto&& a) { result.first = std::move(a); };
			auto subconsumer2 = [&](auto&& a) { result.second = std::move(a); };

			auto temp_range = range;
			if (!r1(temp_range, subconsumer1))
				return false;
			if (!r2(temp_range, subconsumer2))
				return false;

			detail::ConsumeNonEmpty(consumer, result);

			range = temp_range;
			return true;
		});
	}

	template<typename R1, typename R2>
	auto must_follow(R1 r1, R2 r2)
	{
		using Product1 = typename R1::ProductType;
		using Product2 = typename R2::ProductType;
		using Product = detail::TwoTypes<Product1, Product2>;
		return make_rule<Product>([=](auto& range, auto& consumer)
		{

			std::pair<Product1, Product2> result;
			auto subconsumer1 = [&](auto&& a) { result.first = std::move(a); };
			auto subconsumer2 = [&](auto&& a) { result.second = std::move(a); };

			auto temp_range = range;
			if (!r1(temp_range, subconsumer1))
				return false;
			if (!r2(temp_range, subconsumer2))
				throw parsing_error {};

			detail::ConsumeNonEmpty(consumer, result);

			range = temp_range;
			return true;
		});
	}

	template<typename R1, typename R2>
	auto or(R1&& r1, R2&& r2)
	{
		using Product1 = typename R1::ProductType;
		using Product2 = typename R2::ProductType;
		static_assert(Product1 == Product2);
		return make_rule<Product1>([=](auto& range, auto& consumer)
		{
			return r1(range, consumer) || r2(range, consumer);
		});
	}

	template<typename R>
	auto X_or_more(std::size_t x, R r)
	{
		using Subproduct = char;
		using Product = std::vector<Subproduct>;
		return make_rule<Product>([r,x](auto& range, auto& consumer)
		{
			Product result;
			auto product_consumer = [&](Subproduct&& s) { result.emplace_back(std::move(s)); };

			auto temp_range = range;
			std::size_t c = 0;
			while (r(temp_range, product_consumer))
				c++;
			if (c < x)	return false;

			range = temp_range;
			consumer(std::move(result));
			return true;
		});
	}


	namespace operators
	{
		template<typename P, typename T>
		auto operator*(Rule<P, T>& r)
		{
			return X_or_more(0, r);
		}

		template<typename P, typename T>
		auto operator+(Rule<P, T>& r)
		{
			return X_or_more(1, r);
		}

		template<typename P1, typename T1, typename P2, typename T2>
		auto operator >> (Rule<P1, T1>& r1, Rule<P2, T2>& r2)
		{
			return follow(r1, r2);
		}
		
		template<typename T1, typename T2>
		auto operator >>= (T1& r1, T2& r2)
		{
			return must_follow(r1, r2);
		}
		
		template<typename T1, typename T2>
		auto operator || (T1& r1, T2& r2)
		{
			return or (r1, r2);
		}

		auto digit = character<'0', '9'>();

		auto operator""_l(char c)
		{
			return literal(c);
		}

		auto operator""_c(char c)
		{
			return character(c);
		}
	}

}
