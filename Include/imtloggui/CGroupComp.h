#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
//#include <imtloggui/IGroup.h>
#include <imtloggui/ILayerFinder.h>
#include <imtloggui/CProviderBase.h>


namespace imtloggui
{


class CGroupComp:
			public icomp::CComponentBase,
			public CProviderBase,
			virtual public ILayerFinder
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(ILayerFinder);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerNames", "Layer names", false);
		I_ASSIGN_MULTI_0(m_minTimespanAttrPtr, "LayerMinTimespan", "Minimum timespan for layer", false);
		I_ASSIGN_MULTI_0(m_layerCompPtr, "Layers", "Layers", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::ILayerFinder)
	virtual QByteArray GetLayerIdForTimespan(uint64_t timespan) const override;

	// reimplemented (imtbase::IObjectCollection)
	virtual const istd::IChangeable* GetObjectPtr(const QByteArray& objectId) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual imtbase::ICollectionInfo::Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	int GetCount() const;
	int GetIndex(const QByteArray& id) const;

private:
	I_MULTIATTR(QByteArray, m_idAttrPtr);
	I_MULTIATTR(QString, m_nameAttrPtr);
	I_MULTIATTR(double, m_minTimespanAttrPtr);
	I_MULTIREF(imtbase::IObjectCollection, m_layerCompPtr);

	QMap<uint64_t, QByteArray> m_arrangedIds;
};


} // namespace imtloggui


