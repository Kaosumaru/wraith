// Wraith2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "rules.h"
#include <optional>

namespace wraith
{
	template<typename Stream, typename Rule>
	auto parse(Rule& rule, Stream& stream) -> std::optional<typename Rule::ProductType>
	{
		Rule::ProductType product;
		auto consumer = [&](Rule::ProductType&& p) { product = std::move(p); };
		if (!rule(stream, consumer)) return std::nullopt;

		return product;
	}
}


using namespace wraith;
using namespace wraith::operators;

void CharacterTest()
{
	const char* data = "1";
	auto rule = '1'_c;
	auto res = parse(rule, data);
}

void RepeatTest()
{
	const char* data = "11111";
	auto rule = *'1'_c;
	auto res = parse(rule, data);

}

void FollowTest()
{
	const char* data = "11";
	auto rule = '1'_c >> '1'_c;
	auto res = parse(rule, data);

}


void SimpleTest1()
{
	const char* data = "1,2,3,4,";
	auto rule = *(digit >> ','_l);
	auto res = parse(rule, data);

}


int main()
{
	CharacterTest();
	RepeatTest();
	FollowTest();
	SimpleTest1();




	return 0;
}

