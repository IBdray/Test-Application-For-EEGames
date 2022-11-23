#pragma once

template<typename T>
struct EventBase
{
	virtual T GetData() const = 0;
	virtual ~EventBase() = default;

protected:
	T mData {0};
};

struct NumberEvent : EventBase<int>
{
	NumberEvent();
	virtual int GetData() const override;
};

