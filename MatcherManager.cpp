#include "MatcherManager.h"
#include "Parser.h"
#include "ObjectsFactory.h"
#include "Matcher.h"

namespace trading {
	std::unique_ptr<MatcherManager> MatcherManager::instance;
	std::once_flag MatcherManager::onceFlag;

	std::list<std::string> MatcherManager::action(const std::string& msg) {
		std::list<std::string> results;
		try {
			if (msg.empty())
				throw std::runtime_error("input data is empty");

			trading::TOrderInfoPtr orderInfo = trading::ObjectsFactory::Instance().getNewOrderInfo();
			orderInfo->_order = trading::ObjectsFactory::Instance().getNewOrder();			
			
			Parser::parse(msg.c_str(), *orderInfo);

			switch (orderInfo->_action) {
			case eAction::kAdd:
			{
				auto iter = _openOrders.find(orderInfo->_order->_orderId);
				if (_openOrders.end() != iter) {
					results.push_back(Parser::orderInfoErrorToString(*orderInfo, "Duplicate order id"));
				} else {
					TMatcherPtr matcher;
					auto iterMatcher = _matchers.find(orderInfo->_symbol._num);
					if (_matchers.end() == iterMatcher) {
						matcher.reset(new Matcher());
						matcher->setSymbol(std::string(orderInfo->_symbol._str));
						_matchers[orderInfo->_symbol._num] = matcher;
					} else {
						matcher = iterMatcher->second;
					}

					matcher->add(orderInfo, matcher, results);
				}
			}
				break;
			case eAction::kCancel:
			{
				auto iter = _openOrders.find(orderInfo->_order->_orderId);
				TOrderInfoPtr& orderInfo = iter->second.first;
				if (_openOrders.end() == iter) {
					results.push_back(Parser::orderInfoErrorToString(*orderInfo, " Order doesn't exist"));
				} else {
					orderInfo->_action = eAction::kCancel;
					iter->second.second->cancel(orderInfo, results);
					removeOpenOrder(orderInfo->_order->_orderId);
				}
			}
				break;
			case eAction::kPrint:
			{
				std::map<std::string, TMatcherPtr> alphabeticalMatchers;
				for (auto item : _matchers)
					alphabeticalMatchers[item.second->getSymbol()] = item.second;
				
				for (auto item : alphabeticalMatchers)
					item.second->print(results);
			}
				break;
			}
		} catch(const std::exception& e) {
			std::string s = e.what();
			results.push_back(Parser::orderInfoErrorToString(msg + " " + e.what()));
		}

		return results;
	}	

	void MatcherManager::addOpenOrder(const TOrderInfoPtr& orderInfo, const TMatcherPtr& matcher) {
		_openOrders.insert(TOpenOrders::value_type(orderInfo->_order->_orderId, TExtendedOrderInfo(orderInfo, matcher)));
	}

	void MatcherManager::removeOpenOrder(const Order::Id& orderId) {
		auto iter = _openOrders.find(orderId);
		if (_openOrders.end() != iter)
			_openOrders.erase(iter);
	}
}