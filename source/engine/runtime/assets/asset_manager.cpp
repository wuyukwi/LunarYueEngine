#include "asset_manager.h"

namespace runtime
{
asset_manager::asset_manager()
{
}

asset_manager::~asset_manager()
{
}

void asset_manager::clear()
{
	for(auto& pair : storages_)
	{
		auto& storage = pair.second;
		storage->clear();
	}
}

void asset_manager::clear(const std::string& group)
{
	for(auto& pair : storages_)
	{
		auto& storage = pair.second;
		storage->clear(group);
	}
}
}
