#pragma once

#include <deque>
#include <list>
#include <vector>
#include <algorithm>

#include "Defs.h"
#include "OrderInfo.h"

/*
	Classes implemented in this file represent storage of TOrderPtr objects
	on the same price level, with oldest order being return first by getFirst()
	method.

	Three implementations in this file were insprired by ideas in the following 2 links:
		http://baptiste-wicht.com/posts/2012/12/cpp-benchmark-vector-list-deque.html
		https://www.codeproject.com/Articles/5425/An-In-Depth-Study-of-the-STL-Deque-Container
	
	The implementations are:
	1. Orders stored in std::vector<TOrderPtr>
	2. Orders stored in std::deque<TOrderPtr>
	3. Orders stored in std::list<TOrderPtr>

	Which implementation used can be controlled at compile time by defining __OrderBookVector__,
	__OrderBookDeque__ or __OrderBookList__ preprocessor directives in Defs.h file
*/

namespace trading {
	template <typename TContainerType>
	class OrderBookBase {
	public:
		TOrderPtr getFirst() const {
			if (isEmpty())
				return TOrderPtr();

			return _orders.front();
		}

		void addLast(const TOrderPtr& order) {
			_orders.push_back(order);
		}		

		bool remove(Order::Id orderId) {
			auto iter = std::remove_if(_orders.begin(), _orders.end(), [orderId](const TOrderPtr& order) { return orderId == order->_orderId; });
			if (_orders.end() == iter)
				return false;

			_orders.erase(iter);
			return true;
		}

		bool isEmpty() const {
			return _orders.empty();
		}

		TContainerType getAll() const {
			return _orders;
		}

	protected:
		typedef TContainerType TOrders;	
		TOrders _orders;		
	};
#if defined(__OrderBookDeque__)
	class OrderBookDeque : public OrderBookBase<std::deque<TOrderPtr> > {
	public:
		void init() {
			if (!_orders.empty())
				_orders.clear();			
		}

	public:
		void removeFirst() {			
			if (!isEmpty())
				_orders.pop_front();
		}
	};

	typedef OrderBookDeque TOrderBook;	
	typedef std::deque<TOrderPtr> TOrderBookContainer;
#elif defined(__OrderBookList__)
	class OrderBookList : public OrderBookBase<std::list<TOrderPtr> > {
	public:
		void init() {
			if (!_orders.empty())
				_orders.clear();
		}

	public:
		void removeFirst() {
			if (!isEmpty())
				_orders.pop_front();
		}
	};

	typedef OrderBookList TOrderBook;
	typedef std::list<TOrderPtr> TOrderBookContainer;
#elif defined(__OrderBookVector__)
	class OrderBookVector : public OrderBookBase<std::vector<TOrderPtr> > {
	public:
		void init() {
			if (!_orders.empty())
				_orders.clear();

			// Reserve 'reasonable' amount of orders in a vector.
			// NOTE: reserve size might be good candidate to be configurable
			_orders.reserve(2 << 4);
		}

	public:		
		void removeFirst() {
			if (!isEmpty())
				_orders.erase(_orders.begin());
		}		

	private:		
		std::uint16_t _initialSize;
	};

	typedef OrderBookVector TOrderBook;	
	typedef std::vector<TOrderPtr> TOrderBookContainer;	
#else
	# error "Please specify __OrderBookVector__, __OrderBookDeque__ or __OrderBookList__ in Defs.h file"
#endif

	typedef std::shared_ptr<TOrderBook> TOrderBookPtr;	
}
