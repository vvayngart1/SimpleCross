#pragma once

#include "Defs.h"

/*
	Structure that represents minimumalistic set of information
	about an open order. Used in OrderBook.h family of classes
*/
namespace trading {
	enum class eOrderSide {
		kBuy = 'B',
		kSell = 'S',
		kUnknown = 0
	};

	struct Order {	
		Order() {
			clear();
		}

		void clear() {
			NULLIFY(this);
		}

		typedef std::uint32_t Id;
		typedef std::uint64_t Price;
		typedef std::uint16_t Size;
		
		Id _orderId;
		Price _price;
		Size _qty;
		Size _cumQty;
		eOrderSide _side;

		Size getOpenQty() const {
			return _qty - _cumQty;
		}
	};
}