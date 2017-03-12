#pragma once

#include <mutex>
#include <unordered_map>

#include "OrderInfo.h"

/*
	MatcherManager creates and maintains matchers (with 1 to 1 mapping 
	between symbol and matcher instance) and a list of open orders to check 
	for duplicate order ids accross all matchers.

	Matchers are created at the first add action for a new symbol and are
	stored in hash map with the uint64_t key, which corresponds to char[8]
	maximum length value of symbols.

	Peformance note: perhaps storing Matchers as raw pointers instead of
	shared_ptr objects would be a better choice, since MatcherManager is a singleton
	which controls the lifetime scope of Matchers
*/

namespace trading {
	/*
		Forward declaration of Matcher class to declare shared pointer
	*/
	class Matcher;

	class MatcherManager	
	{
	public:
		typedef std::shared_ptr<Matcher> TMatcherPtr;

	public:
		/*
			MatcherManager is a singleton, with access to it done through Instance()
			methods and default constructor having default implementation and
			copy constructor and assignment operator being disabled

			Note: below Instance() is thread safe, which is not needed for the task at
			hand, but it's a cool new techinique I found with new C++ 11, so thought
			would give it a try
		*/
		static MatcherManager& MatcherManager::Instance() {
			std::call_once(MatcherManager::onceFlag, []() {
				instance.reset(new MatcherManager);
				instance->init();
			});	

			return *(instance.get());
		}

	private:
		/*
			Implement only default construstor and disallow copy semantics
		*/
		MatcherManager() = default;
		MatcherManager(const MatcherManager&) = delete;
		MatcherManager & operator=(const MatcherManager&) = delete;

	public:
		/*
			Clear Matchermanager on exit
		*/
		void clear() {
			_matchers.clear();
			_openOrders.clear();
		}

	public:
		/*
			Main method to process an action
		*/
		std::list<std::string> action(const std::string& msg);

	private:
		/*
			addOpenOrder() and removeOpenOrder() methods are being called by Matcher
			class, hence friend Matcher and private access to those methods
		*/
		friend Matcher;

		void addOpenOrder(const TOrderInfoPtr& orderInfo, const TMatcherPtr& matcher);
		void removeOpenOrder(const Order::Id& orderId);

	private:
		void init() {
			/*
				Initialize hash map number of buckets to prevent rehashing. Might not be a significant
				issue since number of symbols is fairly stable once trading day progresses.
			*/
			_matchers.reserve(2 << 6);
		}

	private:
		static std::unique_ptr<MatcherManager> instance;
		static std::once_flag onceFlag;

	private:
		typedef std::pair<TOrderInfoPtr, TMatcherPtr> TExtendedOrderInfo;
		typedef std::unordered_map<Order::Id, TExtendedOrderInfo> TOpenOrders;

		/*
			hash map of symbols (represented as uint64_t, since uint64_t capacity
			is 8 bytes and symbol's max length is 8 bytes as well)
		*/
		typedef std::unordered_map<std::uint64_t, TMatcherPtr> TMatchers;

		TMatchers _matchers;
		TOpenOrders _openOrders;

	public:
		/*
			vector which contains performance measurements for add/cancel
			operations on Matchers.  Was implemented at the last minute, so
			not that well thought out access to it - just exposed it as a public
			member (not a vyer good practice :) ).  Measurements are in microseconds
		*/
		std::vector<double> _performance;
	};
}
