#pragma once

#include "Defs.h"

#include <mutex>

#include "OrderInfo.h"
#include "OrderBook.h"
#include "Pool.h"

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
#else
			return trading::TOrderPtr(_orders.obtain(), _orders.getDeleter());
#endif
		}

		trading::TOrderInfoPtr getNewOrderInfo() {
#if defined(__ObjectsFactory_NO_POOL__)
			return trading::TOrderInfoPtr(new trading::OrderInfo());
#else
			return trading::TOrderInfoPtr(_orderInfos.obtain(), _orderInfos.getDeleter());
#endif
		}

		trading::TOrderBookPtr getNewOrderBook() {
#if defined(__ObjectsFactory_NO_POOL__)
			trading::TOrderBookPtr item = trading::TOrderBookPtr(new trading::TOrderBook());
#else
			trading::TOrderBookPtr item = trading::TOrderBookPtr(_OrderBooks.obtain(), _OrderBooks.getDeleter());			
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
