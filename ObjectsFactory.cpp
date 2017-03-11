#include "ObjectsFactory.h"

namespace trading {
	std::unique_ptr<ObjectsFactory> ObjectsFactory::instance;
	std::once_flag ObjectsFactory::onceFlag;
}