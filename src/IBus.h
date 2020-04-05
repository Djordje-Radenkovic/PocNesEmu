#pragma once

#include <limits>

#include "IBusSlave.h"


template <typename addressWidth, typename dataWidth>
class IBus {
protected:
	static const size_t maxAddress = std::numeric_limits<addressWidth>::max();

public:
	virtual void addSlave(IBusSlave<addressWidth, dataWidth>* slave, addressWidth startAddress, addressWidth endAddress = 0) = 0;
	virtual IBusSlave<addressWidth, dataWidth>* getSlaveWithAddress(addressWidth address) = 0;
	virtual bool write(addressWidth address, dataWidth data, bool log = false) = 0;
	virtual dataWidth read(addressWidth address, bool log = false, bool readOnly = false) = 0;
};
