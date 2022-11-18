#include "Event.h"
#include "RandomGenerator.h"


NumberEvent::NumberEvent()
{
	mData = RandomGenerator::GenerateNumber(-1000, 1000);
}

int NumberEvent::GetData() const
{
	return mData;
}
