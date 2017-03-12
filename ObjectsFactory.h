#pragma once

#include "Defs.h"

#include <mutex>

#include "OrderInfo.h"
#include "OrderBook.h"
#include "Pool.h"

/*
	ObjectsFactory creates TOrderPtr, TOrderInfoPtr and TOrderBookPtr objects used
	by other classes.  Idea behind is to enable optimization of creation/recycling
	of multiple objects used throughout the system.  e.g., orders are being created/
	destroyed on a regular bases, so ObjectsFactory has 2 flavors of creation mechanisms
	for them:
	1. Pool of TOrderPtr, with custom deleter defined for shared_ptr, which returns
	he object back to the pool instead of using delete operator, thus cutting down
	new/delete heap operations dramatically at the cost of shared pointer creation/redirections
	2. No pool functionality by returning newly created on the heap Order object wrapped
	by shared_ptr for access

	1. or 2. is decided upon at compile time via __ObjectsFactory_POOL__ or __ObjectsFactory_NO_POOL__
	preprocessor directives in Defs.h file. 
*/

namespace trading {
	class ObjectsFactory
	{	
	public:
		/*
			ObjectsFactory is a singleton, with access to it done through Instance()
			methods and default constructor having default implementation and
			copy constructor and assignment operator being disabled

			Note: below Instance() is thread safe, which is not needed for the task at
			hand, but it's a cool new techinique I found with new C++ 11, so thought
			would give it a try
		*/
		static ObjectsFactory& ObjectsFactory::Instance() {
			std::call_once(ObjectsFactory::onceFlag, []() {
				instance.reset(new ObjectsFactory);
			});

			return *(instance.get());
		}

	private:
		/*
			Implement only default construstor and disallow copy semantics
		*/
		ObjectsFactory() = default;
		ObjectsFactory(const ObjectsFactory&) = delete;
		ObjectsFactory & operator=(const ObjectsFactory&) = delete;

	public:
		trading::TOrderPtr getNewOrder() {
#if defined(__ObjectsFactory_NO_POOL__)
			return trading::TOrderPtr(new trading::Order());
#elif defined(__ObjectsFactory_POOL__)
			return trading::TOrderPtr(_orders.obtain(), _orders.getDeleter());
#else
		# error "Please specify __ObjectsFactory_POOL__ or __ObjectsFactory_NO_POOL__ in Defs.h file"
#endif
		}

		trading::TOrderInfoPtr getNewOrderInfo() {
#if defined(__ObjectsFactory_NO_POOL__)
			return trading::TOrderInfoPtr(new trading::OrderInfo());
#elif defined(__ObjectsFactory_POOL__)
			return trading::TOrderInfoPtr(_orderInfos.obtain(), _orderInfos.getDeleter());
#else
			# error "Please specify __ObjectsFactory_POOL__ or __ObjectsFactory_NO_POOL__ in Defs.h file"
#endif
		}

		trading::TOrderBookPtr getNewOrderBook() {
#if defined(__ObjectsFactory_NO_POOL__)
			trading::TOrderBookPtr item = trading::TOrderBookPtr(new trading::TOrderBook());
#elif defined(__ObjectsFactory_POOL__)
			trading::TOrderBookPtr item = trading::TOrderBookPtr(_OrderBooks.obtain(), _OrderBooks.getDeleter());			
#else
			# error "Please specify __ObjectsFactory_POOL__ or __ObjectsFactory_NO_POOL__ in Defs.h file"
#endif
			return item;
		}

	private:
		static std::unique_ptr<ObjectsFactory> instance;
		static std::once_flag onceFlag;

#ifndef __ObjectsFactory_NO_POOL__
	private:
		typedef Pool<Order> TPoolOrders;
		typedef Pool<OrderInfo> TPoolOrderInfos;
		typedef Pool<TOrderBook> TPoolOrderBooks;

		TPoolOrders _orders;
		TPoolOrderInfos _orderInfos;
		TPoolOrderBooks _OrderBooks;
#endif
	};	
}
