#pragma once


// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <imod/TSingleModelObserverBase.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/ITimeRangeProvider.h>
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CGroupComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			protected imod::TSingleModelObserverBase<imtlog::ITimeRangeProvider>,
			virtual public iprm::IOptionsList,
			virtual public iprm::ISelectionParam
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef imtbase::CStaticObjectCollection BaseClass2;

	I_BEGIN_COMPONENT(CGroupComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(iprm::IOptionsList);
		I_REGISTER_INTERFACE(iprm::ISelectionParam);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerNames", "Layer names", false);
		I_ASSIGN_MULTI_0(m_minTimespanAttrPtr, "LayerMinTimespan", "Minimum timespan for layer", false);
		I_ASSIGN_MULTI_0(m_layerCompPtr, "Layers", "Layers", false);
		I_ASSIGN(m_timeRangeProviderCompPtr, "TimeRangeProvider", "TimeRangeProvider", true, "TimeRangeProvider");
		I_ASSIGN_TO(m_timeRangeProviderModelCompPtr, m_timeRangeProviderCompPtr, true);
	I_END_COMPONENT

	// reimplemented (iprm::ISelectionParam)
	virtual const IOptionsList* GetSelectionConstraints() const override;
	virtual int GetSelectedOptionIndex() const override;
	virtual bool SetSelectedOptionIndex(int index) override;
	virtual ISelectionParam* GetSubselection(int index) const override;

	// reimplemented (iprm::IOptionsList)
	virtual int GetOptionsFlags() const override;
	virtual int GetOptionsCount() const override;
	virtual QString GetOptionName(int index) const override;
	virtual QString GetOptionDescription(int index) const override;
	virtual QByteArray GetOptionId(int index) const override;
	virtual bool IsOptionEnabled(int index) const override;

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	void SetActiveLayer(const QByteArray& layerId);

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIATTR(double, m_minTimespanAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_layerCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);
	I_REF(imod::IModel, m_timeRangeProviderModelCompPtr);

	QMap<uint64_t, QByteArray> m_arrangedIds;

	QByteArray m_activeLayerId;
};


} // namespace imtloggui


