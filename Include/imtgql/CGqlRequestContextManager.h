#pragma once


// Qt includes
#include <QtCore/QThreadStorage>

// ImtCore includes
#include <imtgql/IGqlContext.h>


namespace imtgql
{


class CGqlRequestContextManager
{
public:
	static void SetContext(const imtgql::IGqlContext* contextPtr)
	{
		if (contextPtr == nullptr) {
			s_context.reset();
			return;
		}

		IGqlContext* clonedPtr = dynamic_cast<IGqlContext*>(contextPtr->CloneMe());
		if (clonedPtr != nullptr){
			s_context = std::shared_ptr<IGqlContext>(clonedPtr);
		}
		else{
			s_context.reset();
		}
	}

	static IGqlContext* GetContext()
	{
		return s_context.get();
	}

	static void Clear()
	{
		s_context.reset();
	}

private:
	static thread_local std::shared_ptr<IGqlContext> s_context;
};


} // namespace imtgql


