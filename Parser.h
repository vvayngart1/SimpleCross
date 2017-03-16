#pragma once

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <sstream>

#include "Utils.h"
#include "Action.h"
#include "OrderInfo.h"

/*
	This class implements a number of static methods to parse incoming action
	string and format fill/cancel/print output strings
*/

namespace trading {
	class Parser {
	private:		
		static const std::uint16_t NUM_FIELDS_ADD = 6;
		static const std::uint16_t NUM_FIELDS_CANCEL = 2;
		static const std::uint32_t MAX_PRECISION = 100000;
		static const char FIELD_SEP = ' ';

	public:
		/*
			Parses incoming action string. As a theme throuht the project,
			doesn't check validity of some of the inputs (e.g. not check
			of msg != nullptr) by implicitly relying on callers of that method
			to check those parameters, so that there check once at the highest
			calling level, instead on each of the nested calls

			This method uses custom specializedStrtok() implemented in Utils.h
			file, which allows to preserve input msg while at the same time
			providing strtok C function tokenization
		*/
		static void parse(const char* msg, OrderInfo& orderInfo) {
			orderInfo._action = static_cast<eAction>(msg[0]);
			switch (orderInfo._action) {
			case eAction::kAdd:
			case eAction::kCancel:
			{	
				if (nullptr == orderInfo._order)
					throw std::runtime_error("order is null");

				Order& order = *(orderInfo._order);
				char tokenCount = 0;
				order._cumQty = 0;

				TStrTokResult tokenResult;
				specializedStrtok(const_cast<char*>(&msg[0]), ' ', tokenResult);				
				char* token = tokenResult.first;
				try {
					while (nullptr != token) {
						++tokenCount;
						switch (tokenCount) {
						case 1:
							break;
						case 2:
						{
							std::int32_t id = std::atol(token);
							if (id <= 0)
								throw std::runtime_error("malformed input for Id");
							order._orderId = std::atol(token);
						}
						break;
						case 3:
						{
							size_t n = strlen(token);
							if (0 == n || n > 8)
								throw std::runtime_error("malformed input for Symbol");

							std::strncpy(orderInfo._symbol._str, token, n);
							orderInfo._symbol._str[n] = '\0';
						}
						break;
						case 4:
						{
							size_t n = strlen(token);
							if (1 != n || n > 8)
								throw std::runtime_error("malformed input for Side");

							order._side = static_cast<eOrderSide>(token[0]);
						}
						break;
						case 5:
						{
							std::int32_t size = std::atoi(token);
							if (0 >= size)
								throw std::runtime_error("malformed input for Size");

							order._qty = size;

						}
						break;
						case 6:
						{
							double price = std::atof(token);
							if (0.0 >= price)
								throw std::runtime_error("malformed input for Price");

							order._price = std::llround(MAX_PRECISION * price);

						}
						break;
						default:
							break;
						}

						if (nullptr != tokenResult.second) {
							*tokenResult.second = ' ';
							specializedStrtok(++tokenResult.second, ' ', tokenResult);
							token = tokenResult.first;
						}
						else {
							token = nullptr;
						}
					}
				} catch (const std::runtime_error e) {
					if (nullptr != tokenResult.second)
						*tokenResult.second = ' ';

					throw e;
				}

				if (eAction::kAdd == orderInfo._action && NUM_FIELDS_ADD != tokenCount)					
						throw std::runtime_error("malformed input for Add action");

				if (eAction::kCancel == orderInfo._action && NUM_FIELDS_CANCEL != tokenCount)
					throw std::runtime_error("malformed input for Cancel action");				
			}
				break;
			case eAction::kPrint:
				break;
			default:
				throw std::runtime_error("unsupported order action");
				break;
			}			
		}		

		static std::string orderInfoToString(const OrderInfo& orderInfo) {
			std::stringstream s;
			s << static_cast<char>(orderInfo._action);
			switch(orderInfo._action) {
			case eAction::kAdd:
			case eAction::kCancel:
			case eAction::kPrint:
				s << FIELD_SEP << orderInfo._order->_orderId;
				if (eAction::kCancel != orderInfo._action) {
					s.precision(5);
					s << FIELD_SEP << orderInfo._symbol._str
						<< FIELD_SEP << static_cast<char>(orderInfo._order->_side)
						<< FIELD_SEP << orderInfo._order->getOpenQty()
						<< FIELD_SEP << std::fixed << static_cast<double>(orderInfo._order->_price) / MAX_PRECISION;
						;
				}
				break;
			default:
				break;
			}

			return s.str();
		}

		static std::string orderInfoFillToString(const OrderInfo& orderInfo, const Order::Size& fillQty, const Order::Price& fillPrice) {
			std::stringstream s;
			s.precision(5);
			s << static_cast<char>(eAction::kFill)	
				<< FIELD_SEP << orderInfo._order->_orderId
				<< FIELD_SEP << orderInfo._symbol._str
				<< FIELD_SEP << fillQty
				<< FIELD_SEP << std::fixed << static_cast<double>(fillPrice) / MAX_PRECISION;
			;

			return s.str();
		}

		static std::string orderInfoErrorToString(const OrderInfo& orderInfo, const std::string& msg) {
			std::stringstream s;			
			s << static_cast<char>(eAction::kError)
				<< FIELD_SEP << (nullptr != orderInfo._order ? orderInfo._order->_orderId : 0)				
				<< FIELD_SEP << msg;
			;

			return s.str();
		}

		static std::string orderInfoErrorToString(const std::string& msg) {
			std::stringstream s;
			s << static_cast<char>(eAction::kError)				
				<< FIELD_SEP << msg;
			;

			return s.str();
		}
	};
}
