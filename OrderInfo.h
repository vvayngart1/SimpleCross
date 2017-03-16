#pragma once

#include <memory>
#include <string>

#include "Action.h"
#include "Order.h"

/*
	Structure that represents extended set of
	information about an open order.  
	As an afterthought,	it's an overkill, Order and OrderInfo structures
	probably should be combined, my original thinking was that Order
	structure would be accessed much more frequently than OrderInfo, so
	tried to keep Order to a minimum size to potentially decrease number
	of cache misses on Order containers.  Ran out of time to clean up :)
*/
namespace trading {
	typedef std::shared_ptr<Order> TOrderPtr;

	/*
		Union to represent symbol as both string and a number
	*/
	union Symbol {
		char _str[9];
		std::uint64_t _num;
	};

	struct OrderInfo {
		OrderInfo() {
			clear();
		}

		void clear() {
			_action = eAction::kUnknown;
			_order.reset();
			NULLIFY(_symbol._str);
		}

		eAction _action;		
		TOrderPtr _order;
		Symbol _symbol;
	};

	typedef std::shared_ptr<OrderInfo> TOrderInfoPtr;
}
