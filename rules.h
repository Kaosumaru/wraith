#pragma once
#include <functional>
#include <memory>
#include "helpers.h"
#include "base_rules.h"
#include "rules.h"


namespace wraith
{

	//matches given character 
	template<typename T>
	auto literal(T &&t)
	{
		return make_lambda_rule([=](auto& range, auto& consumer, auto& producer)
		{
			if (done(range) || get(range) != t)
				return false;

			increment(range);
			return true;
		});
	}


	template<typename T>
	auto character(T &&t)
	{
		return make_lambda_rule([=](auto& range, auto& consumer, auto& producer)
		{
			if (done(range))
				return false;

			auto &&c = get(range);
			if (c != t)
				return false;

			auto &&product = producer();
			apply_to_product(product, c);

			consumer(final_product(product));

			increment(range);
			return true;
		}, identity_producer<char>{});
	}

	template<typename R1, typename R2>
	auto or (R1 &&r1, R2 &&r2)
	{
		return make_lambda_rule([=](auto& range, auto& consumer, auto& producer)
		{
			auto &&product = producer();
			auto product_consumer = [&](auto &&v) { apply_to_product(product, v); }; //TODO forwarding

			auto res1 = r1(range, product_consumer);
			if (res1)
			{
				consumer(final_product(product)); //forward final product to consumer
				return true;
			}

			auto res2 = r2(range, product_consumer);
			if (res2)
			{
				consumer(final_product(product)); //forward final product to consumer
				return true;
			}

			return false;
		});
	}

	template<typename R>
	auto X_or_more(std::size_t x, R &&r)
	{
		return make_lambda_rule([=](auto& range, auto& consumer, auto& producer)
		{
			auto &&product = producer();
			auto product_consumer = [&](auto &&v) { apply_to_product(product, v); }; //TODO forwarding

			auto temp_range = range;
			std::size_t c = 0;
			while (r(temp_range, product_consumer))
				c++;
			if (c < x)
				return false;

			range = temp_range;
			consumer(final_product(product)); //forward final product to consumer
			return true;
		});
	}

	template<typename R1, typename R2>
	auto must_follow(R1 &&r1, R2 &&r2, const char* error)
	{
		return make_lambda_rule([=](auto& range, auto& consumer, auto& producer)
		{
			if (!r1(range, consumer))
				return false;
			if (!r2(range, consumer))
				throw parsing_error{ error };
			return true;
		});
	}

	template<typename R1, typename R2>
	auto follow(R1 &&r1, R2 &&r2)
	{
		return make_lambda_rule([=](auto& range, auto& consumer, auto& producer)
		{
			auto &&product = producer();
			auto product_consumer = [&](auto &&v) { apply_to_product(product, v); };

			auto temp_range = range;
			if (!r1(temp_range, product_consumer))
				return false;
			if (!r2(temp_range, product_consumer))
				return false;
			range = temp_range;
			consumer(final_product(product)); //forward final product to consumer
			return true;
		});
	}


	namespace operators
	{
		template<typename T>
		auto operator*(T& r)
		{
			return X_or_more(0, r);
		}

		template<typename T>
		auto operator+(T& r)
		{
			return X_or_more(1, r);
		}

		template<typename T1, typename T2>
		auto operator >> (T1& r1, T2& r2)
		{
			return follow(r1, r2);
		}

		template<typename T1, typename T2>
		auto operator >>= (T1& r1, T2& r2)
		{
			return must_follow(r1, r2, "");
		}

		template<typename T1, typename T2>
		auto operator || (T1& r1, T2& r2)
		{
			return or (r1, r2);
		}

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
