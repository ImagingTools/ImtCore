#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGroup.h>
#include <imtloggui/ILayer.h>
#include <imtloggui/CProviderBase.h>


namespace imtloggui
{


class CGroupComp:
			public icomp::CComponentBase,
			public CProviderBase,
			virtual public IGroup
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupComp)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection);
		I_REGISTER_INTERFACE(IGroup);
		I_ASSIGN_MULTI_0(m_idAttrPtr, "LayerIds", "Layer ids", false);
		I_ASSIGN_MULTI_0(m_nameAttrPtr, "LayerNames", "Layer names", false);
		I_ASSIGN_MULTI_0(m_messageIdListAttrPtr, "MessageIdList", "Supported message id's. Empty list for any", false);
		I_ASSIGN_MULTI_0(m_layerCompPtr, "Layers", "Layers", false);
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroup)
	virtual QList<int> GetSupportedMessageIds() const override;
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
	I_MULTIATTR(int, m_messageIdListAttrPtr);
	I_MULTIREF(ILayer, m_layerCompPtr);

	QMap<uint64_t, QByteArray> m_arrangedIds;
};


} // namespace imtloggui


