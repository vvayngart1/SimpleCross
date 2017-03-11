#pragma once

/*
	This file contains #define directives to try:
	1. Different implementations of components for performance tuning
	2. Printing of all performance measurements at the end of test
*/

/*
	The following #define directives relate to OrderBook implementations in OrderBook.h file
	For more information, see comment in OrderBook.h file
*/
//#define __OrderBookDeque__
//#define __OrderBookList__


/*
	The following #define directives relate to OrderBook implementations in ObjectsFactory.h file
	For more information, see comment in ObjectsFactory.h file
*/
//#define __ObjectsFactory_NO_POOL__

/*
	The following #define directive controls if all individual performance
	measurements will be printed out to cout in SimpleCross.h file
*/
//#define __SimpleCrossPrintPerformanceVector__