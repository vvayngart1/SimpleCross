#pragma once

#include <memory>
#include <string>

#include "Action.h"
#include "Order.h"

/*
	Structure that represents minimumalistic set of
	information about an open order
*/
namespace trading {
	typedef std::shared_ptr<Order> TOrderPtr;

	union Symbol {
		char _str[9];
		std::uint64_t _num;
	};

	struct OrderInfo {
		eAction _action;		
		TOrderPtr _order;
		Symbol _symbol;
	};

	typedef std::shared_ptr<OrderInfo> TOrderInfoPtr;
}
