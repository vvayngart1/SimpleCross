#pragma once

#include <vector>
#include <string>

#include "OrderInfo.h"
#include "PriceLadder.h"
#include "MatcherManager.h"

/*
	Main class to manage orders/cancels/fills per symbol (e.g. separate
	Matcher for IBM vs MSFT).
	Matcher's containers for price levels/orders is TPriceLadderBids/TPriceLadderAsks,
	which are defined in PriceLadder.h file.  
	TPriceLadderBids is an abstraction of prices to container of orders in time priority
	at each price level, with prices being organized from largest to smallest. 
	TPriceLadderAsks is the same as TPriceLadderBids, with the difference being of prices
	being organized from smallest to largest (for more information, please see comments
	in PriceLadder.h).
*/

namespace trading {
	class Matcher {
		/*
			Lifetime scope and access of Matchers are managed by MatcherManager,
			and the following constructors/assignment operator semantics enforces
			single instance per symbol (and no copying or assigment) creation by
			MatcherManager.
		*/
	public:
		/*
			Constructor sets Matcher's symbol and configures
			member _orderInfoForPrint instance for printing
		*/
		Matcher(const std::string& symbol) : _bids()
											,_asks()
											,_orderInfoForPrint()
		{
			_orderInfoForPrint._action = eAction::kPrint;
			std::copy(symbol.begin(), symbol.end(), _orderInfoForPrint._symbol._str);
			_orderInfoForPrint._symbol._str[symbol.length()] = '\0';
		}

	private:
		Matcher(const Matcher&) = delete;
		Matcher & operator=(const Matcher&) = delete;

	public:
		std::string getSymbol() const {
			return std::string(_orderInfoForPrint._symbol._str);
		}

	public:
		/*
			Main methods to process actions per symbol
		*/

		/*
			Processes new order request and generates fills if any
		*/
		void add(TOrderInfoPtr& orderInfoPtr, const MatcherManager::TMatcherPtr& matcher, std::list<std::string>& results);

		/*
			Processes cancel order request
		*/
		void cancel(TOrderInfoPtr& orderInfoPtr, std::list<std::string>& results);

		/*
			Processes print orders request
		*/
		void print(std::list<std::string>& results);

	private:
		/*
			Main method which process addition of new order and check crosses/generates fills if any.
			It processes addition of a new bid/ask in a generic way, which is achieved by templatizing of
			TPriceLadder types.

			Peformance note:  this method would be an early choice to target for optimization the way it's written
			now, since it accepts a list of strings to return results (e.f. fills) in. The reason for optimization
			would be that strings are created on the heap, which is time consuming and there are multiple copies
			of the them for the same operation: Parser::orderInfoFillToString() creates string for each fill and than
			it copies it into results vector, so there are at least 2 strings created/destroyed and 1 copy per fill
			(Unfortunately ran out of time to try any of the optimization techniques :) )
		*/
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
