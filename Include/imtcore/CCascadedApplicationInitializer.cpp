#include <imtcore/CCascadedApplicationInitializer.h>



namespace imtcore
{


void CCascadedApplicationInitializer::Initialize() 
{
	for (const auto& initializer: m_initializers)
	{
		initializer->Initialize();
	}
}


CCascadedApplicationInitializer* CCascadedApplicationInitializer::AddInitializer(IApplicationInitializer* initializer)
{
	m_initializers.emplace_back(initializer);

	return this;
}



} // namespace imtcore
