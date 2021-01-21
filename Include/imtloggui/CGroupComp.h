#pragma once


// Acf includes
#include <imod/TSingleModelObserverBase.h>
#include <imod/CModelProxy.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlog/ITimeRangeProvider.h>
#include <imtbase/CStaticObjectCollection.h>


namespace imtloggui
{


class CGroupComp:
			public icomp::CComponentBase,
			public imtbase::CStaticObjectCollection,
			public imod::TSingleModelObserverBase<imtlog::ITimeRangeProvider>
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef public imtbase::CStaticObjectCollection BaseClass2;

	I_BEGIN_COMPONENT(CGroupComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_SUBELEMENT(RepresentationProxy);
		I_REGISTER_SUBELEMENT_INTERFACE(RepresentationProxy, imod::IModel, ExtractRepresentationProxy);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerNames", "Layer names", false);
		I_ASSIGN_MULTI_0(m_minTimespanAttrPtr, "LayerMinTimespan", "Minimum timespan for layer", false);
		I_ASSIGN_MULTI_0(m_layerCompPtr, "Layers", "Layers", false);
		I_ASSIGN(m_timeRangeProviderCompPtr, "TimeRangeProvider", "TimeRangeProvider", false, "TimeRangeProvider");
	I_END_COMPONENT

protected:
	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	template <typename InterfaceType>
	static InterfaceType* ExtractRepresentationProxy(CGroupComp& component)
	{
		return &component.m_representationProxy;
	}

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIATTR(double, m_minTimespanAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_layerCompPtr);
	I_REF(imtlog::ITimeRangeProvider, m_timeRangeProviderCompPtr);

	QMap<uint64_t, QByteArray> m_arrangedIds;
	imod::CModelProxy m_representationProxy;
};


} // namespace imtloggui


