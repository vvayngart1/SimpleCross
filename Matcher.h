#pragma once

#include <vector>
#include <string>

#include "OrderInfo.h"
#include "PriceLadder.h"
#include "MatcherManager.h"

namespace trading {
	class Matcher {
	public:
		void setSymbol(const std::string& symbol) {
			_orderInfoForPrint._action = eAction::kPrint;

			std::copy(symbol.begin(), symbol.end(), _orderInfoForPrint._symbol._str);
			_orderInfoForPrint._symbol._str[symbol.length()] = '\0';
		}

		std::string getSymbol() const {
			return std::string(_orderInfoForPrint._symbol._str);
		}

	public:
		void add(TOrderInfoPtr& orderInfoPtr, const MatcherManager::TMatcherPtr& matcher, std::list<std::string>& results);
		void cancel(TOrderInfoPtr& orderInfoPtr, std::list<std::string>& results);
		void print(std::list<std::string>& results);

	private:
		template <typename TPriceLadderSame, typename TPriceLadderOpposite>
		void checkCrossesAndAdd(TOrderInfoPtr& orderInfo,
								TPriceLadderSame& same,
								TPriceLadderOpposite& opp,
								const MatcherManager::TMatcherPtr& matcher,
								std::list<std::string>& results) {
			TOrderBookPtr topOpp = opp.getBest();
			if (nullptr != topOpp) {
				bool done = false;
				while (!done) {
					TOrderPtr topOrder = topOpp->getFirst();

					if (!(done = opp.getOpCompare()(orderInfo->_order->_price, topOrder->_price))) {
						Order::Price fillPrice = (orderInfo->_order->_price == topOrder->_price) ? orderInfo->_order->_price : topOrder->_price;
						Order::Size fillQty = std::min(orderInfo->_order->getOpenQty(), topOrder->getOpenQty());

						results.push_back(Parser::orderInfoFillToString(*orderInfo, fillQty, fillPrice));
						_orderInfoForPrint._order = topOrder;
						results.push_back(Parser::orderInfoFillToString(_orderInfoForPrint, fillQty, fillPrice));

						orderInfo->_order->_cumQty += fillQty;
						topOrder->_cumQty += fillQty;
						if (0 == topOrder->getOpenQty()) {
							topOpp->removeFirst();
							if (topOpp->isEmpty()) {
								opp.removeBest();
								topOpp = opp.getBest();
								if (nullptr == topOpp)
									done = true;
							}

							MatcherManager::Instance().removeOpenOrder(topOrder->_orderId);
						}

						if (!done)
							done = (0 == orderInfo->_order->getOpenQty());
					}
				}
			}			

			if (0 < (orderInfo->_order->getOpenQty())) {
				same.add(orderInfo->_order);
				MatcherManager::Instance().addOpenOrder(orderInfo, matcher);
			}
		}
		
		template <typename TIter>
		void printBooks(TIter begin, TIter end, std::list<std::string>& results, bool ordersInReverse) {
			for (; begin != end; ++begin) {
				TOrderBookContainer orders = (*begin)->getAll();
				ordersInReverse ? printOrders(orders.rbegin(), orders.rend(), results) : printOrders(orders.begin(), orders.end(), results);
			}	
		}

		template <typename TIter>
		void printOrders(TIter begin, TIter end, std::list<std::string>& results) {
			for(; begin != end; ++begin) {
				_orderInfoForPrint._order = *begin;
				results.push_back(Parser::orderInfoToString(_orderInfoForPrint));
			}
		}

	private:
		TPriceLadderBids _bids;
		TPriceLadderAsks _asks;
		OrderInfo _orderInfoForPrint;
	};
}
