#pragma once


// Acf includes
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtbase/CStaticObjectCollection.h>
#include <imtloggui/IScenographer.h>


namespace imtloggui
{


class CLayerViewComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			public imod::TSingleModelObserverBase<iprm::ISelectionParam>,
			virtual public iprm::IIdParam,
			virtual public iprm::IEnableableParam
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::CStaticObjectCollection BaseClass2;

	I_BEGIN_COMPONENT(CLayerViewComp)
		I_REGISTER_INTERFACE(IObjectCollection);
		I_REGISTER_INTERFACE(iprm::IIdParam);
		I_REGISTER_INTERFACE(iprm::IEnableableParam);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "RepresentationViewIds", "Representation view ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "RepresentationViewNames", "Representation view names", false);
		I_ASSIGN_MULTI_0(m_representationViewCompPtr, "RepresentationViews", "Representation views", false);
	I_END_COMPONENT

	CLayerViewComp();

	// reimplemented (iprm::IEnableableParam)
	virtual bool IsEnabled() const override;
	virtual bool IsEnablingAllowed() const override;
	virtual bool SetEnabled(bool isEnabled = true) override;

	// reimplemented (iprm::IIdParam)
	virtual QByteArray GetId() const override;
	virtual void SetId(const QByteArray& id) override;

protected:
	QByteArray GetSelectedRepresentationViewId();

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIREF(IScenographer, m_representationViewCompPtr);

	imod::CModelUpdateBridge m_updateBridge;
	QByteArray m_groupId;
	bool m_isEnabled;
};


} // namespace imtloggui


