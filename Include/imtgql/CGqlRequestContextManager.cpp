#include <imtgql/CGqlRequestContextManager.h>


namespace imtgql
{


thread_local istd::TSharedInterfacePtr<IGqlContext> CGqlRequestContextManager::s_context;


} // namespace imtgql
