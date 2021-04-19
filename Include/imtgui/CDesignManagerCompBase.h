#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtwidgets/CImtStyle.h>


namespace imtgui
{


class CDesignManagerCompBase: public icomp::CComponentBase
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CDesignManagerCompBase);
		I_REGISTER_INTERFACE(iprm::ISelectionParam);
		I_ASSIGN(m_selectionParamCompPtr, "DesignSelectionProvider", "Design selection provider", true, "");
		I_ASSIGN_TO(m_selectionParamModelPtr, m_selectionParamCompPtr, true);
	I_END_COMPONENT;

	CDesignManagerCompBase();

protected:
	typedef int(*ResourceFunctionPtr)();

	virtual void RegisterResourcesFunctions();

	void SetDesignResourcesFunctions(
				imtwidgets::CImtStyle::DesignSchema designSchema,
				ResourceFunctionPtr initResources,
				ResourceFunctionPtr cleanupResources);

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void OnDesignSelectionUpdated(const istd::IChangeable::ChangeSet& changeSet, const iprm::ISelectionParam* selectionParamPtr);

private:
	I_REF(iprm::ISelectionParam, m_selectionParamCompPtr);
	I_REF(imod::IModel, m_selectionParamModelPtr);

	QMap<imtwidgets::CImtStyle::DesignSchema, ResourceFunctionPtr> m_initResources;
	QMap<imtwidgets::CImtStyle::DesignSchema, ResourceFunctionPtr> m_cleanupResources;
	imtbase::TModelUpdateBinder<iprm::ISelectionParam, CDesignManagerCompBase> m_selectionObserver;
};


} // namespace imtgui


