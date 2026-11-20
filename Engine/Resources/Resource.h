#pragma once
#include "Framework/Object.h"
#include <memory>

namespace nu
{
	class Resource : public Object
	{
	public:
		Resource() = default;
		virtual ~Resource() = default;
	};


	template <typename T = Resource>
	using res_t = std::shared_ptr<T>;
}