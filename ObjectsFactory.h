#pragma once

//#define __ObjectsFactory_NO_POOL__

#include <mutex>

#include "OrderInfo.h"
#include "OrderBook.h"
#include "Pool.h"

namespace trading {
	class ObjectsFactory
	{	
	public:
		ObjectsFactory() = default;
		static ObjectsFactory& ObjectsFactory::Instance() {
			std::call_once(ObjectsFactory::onceFlag, []() {
				instance.reset(new ObjectsFactory);
			});

			return *(instance.get());
		}

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
		ObjectsFactory(const ObjectsFactory&) = delete;
		ObjectsFactory & operator=(const ObjectsFactory&) = delete;

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
