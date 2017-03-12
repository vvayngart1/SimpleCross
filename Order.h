#pragma once

/*
	Structure that represents minimumalistic set of information
	about an open order. Used in OrderBook.h family of classes
*/
namespace trading {
	enum class eOrderSide {
		kBuy = 'B',
		kSell = 'S'
	};

	struct Order {		
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