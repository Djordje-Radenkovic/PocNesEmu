#pragma once

template<typename addressWidth, typename dataWidth>
class IBusSlave {
public:
	virtual addressWidth size() = 0;
	virtual dataWidth read(addressWidth address) = 0;
	virtual void write(addressWidth address, dataWidth data) = 0;

	virtual ~IBusSlave() = 0 {}
};
