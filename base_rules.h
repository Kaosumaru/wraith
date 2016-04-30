#pragma once
#include <functional>
#include <memory>
#include "helpers.h"



namespace wraith
{


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

		//NYI
		template<typename Consumer>
		bool operator() (Range &range, Consumer& consumer) const
		{
			return (*_subrule)(range);
		}

		//NYI
		template< typename Consumer, typename Producer>
		bool operator() (Range &range, Consumer& consumer, Producer& producer) const
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

		//NYI
		template<typename Consumer>
		bool operator() (Range &range, Consumer& consumer) const
		{
			return (*_subrule)(range);
		}

		//NYI
		template< typename Consumer, typename Producer>
		bool operator() (Range &range, Consumer& consumer, Producer& producer) const
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
			return _t(range, empty_consumer{}, empty_producer{});
		}

		template<typename Range, typename Consumer>
		bool operator() (Range &range, Consumer& consumer) const
		{
			return _t(range, consumer, empty_producer{});
		}

		template<typename Range, typename Consumer, typename Producer>
		bool operator() (Range &range, Consumer& consumer, Producer& producer) const
		{
			return _t(range, consumer, producer);
		}
	protected:
		T _t;
	};

	template<typename T>
	auto make_lambda_rule(T &&t)
	{
		return lambda_rule<T>{std::forward<T>(t)};
	}


}
