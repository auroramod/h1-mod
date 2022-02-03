#pragma once
#include <thread>

class debugger
{
public:
	debugger(unsigned long process_id, bool start);
	~debugger();

private:
	volatile bool terminate_ = false;
	std::thread runner_;

	void run(unsigned long process_id) const;
};
