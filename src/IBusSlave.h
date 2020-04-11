#pragma once

template<typename addressWidth, typename dataWidth>
class IBusSlave {
public:
	virtual inline const addressWidth size() = 0;
	virtual dataWidth read(addressWidth address, bool readOnly = false) = 0;
	virtual void write(addressWidth address, dataWidth data) = 0;

	virtual ~IBusSlave() {}
};
