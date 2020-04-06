#pragma once

template<typename addressWidth, typename dataWidth>
class IBusSlave {
public:
	virtual void setAddressRange(addressWidth startAddress,
		addressWidth endAddress) = 0;
	virtual void setAddressRange(addressWidth startAddress) = 0;
	virtual inline const addressWidth startAddress() = 0;
	virtual inline const addressWidth endAddress() = 0;
	virtual inline const addressWidth size() = 0;
	virtual dataWidth read(addressWidth address) = 0;
	virtual void write(addressWidth address, dataWidth data) = 0;

	virtual ~IBusSlave() {}
};
