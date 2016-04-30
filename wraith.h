#pragma once
#include <string>
#include <functional>
#include <memory>

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

	struct parsing_error : public std::logic_error
	{
		using logic_error::logic_error;
	};


	template<typename Range>
	class weak_rule;

	template<typename Range>
	class rule
	{
	public:
		friend class weak_rule<Range>;

		rule() {}

		template<typename T>
		rule(T&& t) :
			_subrule(std::make_shared<Function>(std::forward<T>(t)))
		{}

		bool operator() (Range &range) const
		{
			return (*_subrule)(range);
		}

		template<typename T>
		rule& operator=(T&& t)
		{
			*_subrule = std::forward<T>(t);
			return *this;
		}


	protected:
		using Function = std::function<bool(Range &range)>;
		using FunctionPointer = std::shared_ptr<Function>;

		FunctionPointer _subrule = std::make_shared<Function>();
	};

	template<typename Range>
	class weak_rule
	{
	public:
		weak_rule() {}

		weak_rule(const rule<Range>& t) :
			_subrule(t._subrule.get())
		{}

		bool operator() (Range &range) const
		{
			return (*_subrule)(range);
		}

	protected:
		using Function = std::function<bool(Range &range)>;

		const Function* _subrule;
	};

	template<typename Range>
	auto weaken(const rule<Range>& t)
	{
		return weak_rule<Range>{t};
	}

	template<typename T>
	class lambda_rule
	{
	public:
		lambda_rule(T &t) : _t(t) {}

		template<typename Range>
		bool operator() (Range &range) const
		{
			return _t(range);
		}
	protected:
		T _t;
	};



	template<typename T>
	auto make_lambda_rule(T &&t)
	{
		return lambda_rule<T>{std::forward<T>(t)};
	}


	//matches given character 
	template<typename T>
	auto character(T &&t)
	{
		return make_lambda_rule([=](auto& range)
		{
			if (done(range) || get(range) != t)
				return false;

			increment(range);
			return true;
		});
	}

	template<typename R1, typename R2>
	auto or (R1 &&r1, R2 &&r2)
	{
		return make_lambda_rule([=](auto& range)
		{
			return r1(range) || r2(range);
		});
	}

	template<typename R>
	auto X_or_more(std::size_t x, R &&r)
	{
		return make_lambda_rule([=](auto& range)
		{
			auto temp_range = range;
			std::size_t c = 0;
			while (r(temp_range))
				c++;
			if (c < x)
				return false;

			range = temp_range;
			return true;
		});
	}

	template<typename R1, typename R2>
	auto must_follow(R1 &&r1, R2 &&r2, const char* error)
	{
		return make_lambda_rule([=](auto& range)
		{
			if (!r1(range))
				return false;
			if (!r2(range))
				throw parsing_error{ error };
			return true;
		});
	}

	template<typename R1, typename R2>
	auto follow(R1 &&r1, R2 &&r2)
	{
		return make_lambda_rule([=](auto& range)
		{
			auto temp_range = range;
			if (!r1(temp_range))
				return false;
			if (!r2(temp_range))
				return false;
			range = temp_range;
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

		auto operator""_c(char c)
		{
			return character(c);
		}
	}
}
