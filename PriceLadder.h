#pragma once

#include <algorithm>
#include <iterator>
#include <map>

#include "ObjectsFactory.h"

namespace trading {
	template <typename TOp>
	class PriceLadderOrderedMap {
	public:
		typedef std::map<Order::Price, TOrderBookPtr, TOp> TPriceLadder;	

	public:
		TOrderBookPtr getBest() const {
			if (isEmpty())
				return _nullBook;

			return _priceLadder.begin()->second;
		}

		void removeBest() {
			if (!isEmpty())
				_priceLadder.erase(_priceLadder.begin());
		}
		
		void add(const TOrderPtr& order) {
			TOrderBookPtr orderBook;
			auto iter = _priceLadder.find(order->_price);
			if (_priceLadder.end() == iter) {
				orderBook = trading::ObjectsFactory::Instance().getNewOrderBook();
				orderBook->init();
				_priceLadder.insert(TPriceLadder::value_type(order->_price, orderBook));
			} else {
				orderBook = iter->second;
			}

			orderBook->addLast(order);
		}

		bool remove(const TOrderPtr& order) {
			auto iter = _priceLadder.find(order->_price);
			if (_priceLadder.end() == iter)
				return false;

			bool status = iter->second->remove(order->_orderId);
			if (iter->second->isEmpty())
				_priceLadder.erase(iter);

			return status;
		}

		bool isEmpty() const {
			return _priceLadder.empty();
		}

		std::vector<TOrderBookPtr> getAll() const {
			std::vector<TOrderBookPtr> prices;
			if (!_priceLadder.empty()) {
				prices.reserve(_priceLadder.size());
				std::transform(_priceLadder.begin(), _priceLadder.end(), std::back_inserter(prices), [](TPriceLadder::const_reference p) { return p.second; });		
			}

			return prices;
		}

		typename TPriceLadder::key_compare getOpCompare() const {
			return _priceLadder.key_comp();
		}

	private:
		TPriceLadder _priceLadder;		
		TOrderBookPtr _nullBook;
	};
}
