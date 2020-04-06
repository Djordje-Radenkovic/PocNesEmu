#pragma once

#include "IBus.h"

template<typename addressWidth, typename dataWidth>
class IBusMaster {
public:
	virtual void connectBus(std::shared_ptr<IBus<addressWidth, dataWidth>> bus) = 0;

	virtual ~IBusMaster() {}

protected:
	std::shared_ptr<IBus<addressWidth, dataWidth>> m_bus = nullptr;
	virtual inline dataWidth readFrom(addressWidth address) = 0;
	virtual inline void writeTo(addressWidth address, dataWidth data) = 0;
};
