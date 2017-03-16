#pragma once

/*
	Enum to indicate trading actions (both requests and responses)
*/

namespace trading {
	enum class eAction {
		kAdd = 'O',
		kCancel = 'X',
		kPrint = 'P',
		kFill = 'F',
		kError = 'E',
		kUnknown = 0
	};
}
	