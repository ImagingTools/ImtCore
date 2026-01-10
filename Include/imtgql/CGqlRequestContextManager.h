#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtgql
{


class CGqlRequestContextManager
{
public:
	static void SetContext(const IGqlContext* contextPtr)
	{
		if (contextPtr == nullptr) {
			s_context.Reset();
			return;
		}

		s_context.MoveCastedPtr(contextPtr->CloneMe());
	}

	static IGqlContext* GetContext()
	{
		return s_context.GetPtr();
	}

	static void Clear()
	{
		s_context.Reset();
	}

private:
	static thread_local istd::TSharedInterfacePtr<IGqlContext> s_context;
};


} // namespace imtgql


