#pragma once
#include <functional>
#include <memory>
#include <utility>

namespace wraith
{
	template<typename Product, typename T>
	class Rule
	{
	public:
		Rule(const T& t) : _t(t){}

		Rule(const T&& t) : _t(std::move(t)){}
		Rule(const Rule& rule) : _t(rule._t) {}
		Rule(Rule&& rule) : _t(std::move(rule._t)) {}

		template<typename Stream, typename Consumer>
		bool operator() (Stream& stream, Consumer& consumer) const
		{
			return _t(stream, consumer);
		}

		using ProductType = Product;
	protected:
		T _t;
	};

	struct empty {};

	template<typename Product, typename T>
	auto make_rule(T&& t)
	{
		return Rule<Product, T>(std::forward<T>(t));
	}

	struct parsing_error : public std::logic_error
	{
		using logic_error::logic_error;
	};
}