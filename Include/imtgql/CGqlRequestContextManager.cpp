#include <imtgql/CGqlRequestContextManager.h>


namespace imtgql
{


thread_local std::shared_ptr<IGqlContext> CGqlRequestContextManager::s_context;


} // namespace imtgql
