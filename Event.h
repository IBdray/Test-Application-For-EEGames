#pragma once

template<typename T>
struct Event
{
	virtual T GetData() const = 0;
	virtual ~Event() = default;

protected:
	T mData {0};
};

struct NumberEvent : public Event<int>
{
	NumberEvent();
	virtual int GetData() const override;
};

