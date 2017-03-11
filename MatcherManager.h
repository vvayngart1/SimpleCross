#pragma once

#include <mutex>
#include <unordered_map>

#include "OrderInfo.h"

namespace trading {
	class Matcher;

	class MatcherManager	
	{
	public:
		typedef std::shared_ptr<Matcher> TMatcherPtr;

	public:
		MatcherManager() = default;
		static MatcherManager& MatcherManager::Instance() {
			std::call_once(MatcherManager::onceFlag, []() {
				instance.reset(new MatcherManager);
				instance->init();
			});	

			return *(instance.get());
		}

	public:
		void clear() {
			_matchers.clear();
			_openOrders.clear();
		}

	public:
		std::list<std::string> action(const std::string& msg);
		void addOpenOrder(const TOrderInfoPtr& orderInfo, const TMatcherPtr& matcher);
		void removeOpenOrder(const Order::Id& orderId);

	private:
		void init() {
			// Initialize hash map number of buckets to prevent rehashing. Might not be a significant
			// issue since number of symbols is fairly stable once trading day progresses.
			_matchers.reserve(2 << 6);
		}

	private:
		MatcherManager(const MatcherManager&) = delete;
		MatcherManager & operator=(const MatcherManager&) = delete;

		static std::unique_ptr<MatcherManager> instance;
		static std::once_flag onceFlag;

	private:
		typedef std::pair<TOrderInfoPtr, TMatcherPtr> TExtendedOrderInfo;
		typedef std::unordered_map<Order::Id, TExtendedOrderInfo> TOpenOrders;

		typedef std::unordered_map<std::uint64_t, TMatcherPtr> TMatchers;

		TMatchers _matchers;
		TOpenOrders _openOrders;
	};
}
