#pragma once

#include "Bus.h"

template<typename addressWidth, typename dataWidth>
class IBusMaster {
protected:
	Bus<addressWidth, dataWidth>* m_bus = nullptr;

public:
	virtual void connectBus(Bus<addressWidth, dataWidth>* bus) = 0;
	virtual inline dataWidth read(addressWidth address) = 0;
	virtual inline void write(addressWidth address, dataWidth data) = 0;

	virtual ~IBusMaster() {}
};
