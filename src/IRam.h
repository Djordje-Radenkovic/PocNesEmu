#pragma once

#include "IBusSlave.h"


template <typename addressWidth, typename dataWidth>
class IRam : public IBusSlave<addressWidth, dataWidth> {
public:
	virtual ~IRam() {}
};
