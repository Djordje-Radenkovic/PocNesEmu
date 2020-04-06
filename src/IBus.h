#pragma once

#include <limits>
#include <fstream>

#include "IBusSlave.h"


template <typename addressWidth, typename dataWidth>
class IBus {
public:
	virtual void addSlave(std::shared_ptr<IBusSlave<addressWidth, dataWidth>> slave,
		addressWidth startAddress, addressWidth endAddress) = 0;

	virtual void addSlave(std::shared_ptr<IBusSlave<addressWidth, dataWidth>> slave,
		addressWidth startAddress) = 0;

	virtual void getSlaveWithAddress(addressWidth address) = 0;

	virtual bool write(addressWidth address, dataWidth data) = 0;
	virtual dataWidth read(addressWidth address) = 0;

	virtual void dump_memory(const char* filePath,
		addressWidth startAddress = 0, addressWidth endAddresss = maxAddress) = 0;

protected:
	static const size_t maxAddress = std::numeric_limits<addressWidth>::max();
};
