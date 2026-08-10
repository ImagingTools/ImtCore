#pragma once


// stl includes
#include <list>
#include <memory>

// ImtCore includes
#include <imtcore/IApplicationInitializer.h>


namespace imtcore
{


class CCascadedApplicationInitializer : public IApplicationInitializer
{
public:
	virtual void Initialize() override;
	/**
		\brief Adds an initializer to the initializer chain.
		\warning This method will take ownership of the initializer.
		\warning After calling this method, the initializer MUST not be used!
	*/
	virtual CCascadedApplicationInitializer* AddInitializer(IApplicationInitializer* initializer);

private:
	std::list<std::unique_ptr<IApplicationInitializer>> m_initializers;
};


} // namespace imtcore
