#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/CSelectionParamComp.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtwidgets/CImtStyle.h>


namespace imtgui
{


class CDesignManagerCompBase: public iprm::CSelectionParamComp
{
public:
	typedef iprm::CSelectionParamComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CDesignManagerCompBase);
		I_ASSIGN_MULTI_0(m_slaveCompPtr, "SlaveDesignManagers", "Slave design managers", false);
	I_END_COMPONENT;

protected:
	typedef int(*ResourceFunctionPtr)();

	virtual void RegisterResourcesFunctions();

	void SetDesignResourcesFunctions(
				imtwidgets::CImtStyle::DesignSchema designSchema,
				ResourceFunctionPtr initResources,
				ResourceFunctionPtr cleanupResources);

	virtual bool SetSelectedOptionIndex(int index) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_MULTIREF(iprm::ISelectionParam, m_slaveCompPtr);

	QMap<imtwidgets::CImtStyle::DesignSchema, ResourceFunctionPtr> m_initResources;
	QMap<imtwidgets::CImtStyle::DesignSchema, ResourceFunctionPtr> m_cleanupResources;
};


} // namespace imtgui


