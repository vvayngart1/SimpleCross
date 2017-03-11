#include "Matcher.h"
#include "Parser.h"

namespace trading {
	void Matcher::add(TOrderInfoPtr& orderInfo, const MatcherManager::TMatcherPtr& matcher, std::list<std::string>& results) {
		switch (orderInfo->_order->_side) {
		case eOrderSide::kBuy:		
			checkCrossesAndAdd(orderInfo, _bids, _asks, matcher, results);
			break;	
		case eOrderSide::kSell:
			checkCrossesAndAdd(orderInfo, _asks, _bids, matcher, results);
			break;			
		default:
			results.push_back(Parser::orderInfoErrorToString(*orderInfo, "Can't process order for unknown side"));
			break;
		}		
	}

	void Matcher::cancel(TOrderInfoPtr& orderInfo, std::list<std::string>& results) {
		bool success = (eOrderSide::kBuy == orderInfo->_order->_side) ? _bids.remove(orderInfo->_order) : _asks.remove(orderInfo->_order);
		results.push_back(success ? Parser::orderInfoToString(*orderInfo) : Parser::orderInfoErrorToString(*orderInfo, "Can't find order to cancel in price ladder"));		
	}

	void Matcher::print(std::list<std::string>& results) {
		std::vector<TOrderBookPtr> bids = _bids.getAll();
		std::vector<TOrderBookPtr> asks = _asks.getAll();

		printBooks(asks.rbegin(), asks.rend(), results, true);
		printBooks(bids.begin(), bids.end(), results, false);
	}
}
