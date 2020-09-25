#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IGroup.h>
#include <imtloggui/ILayerProvider.h>


namespace imtloggui
{


class CGroupComp:
			public icomp::CComponentBase,
			virtual public IGroup,
			virtual public ILayerProvider	
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGroupComp)
		I_REGISTER_INTERFACE(IGroup);
		I_REGISTER_INTERFACE(ILayerProvider);
		I_ASSIGN_MULTI_0(m_layersCompPtr, "Layers", "Layer components", false);
		I_ASSIGN(m_idAttrPtr, "GroupId", "Group id", true, "");
		I_ASSIGN(m_nameAttrPtr, "GroupName", "Group name", true, "Default name");
		I_ASSIGN(m_heightAttrPtr, "Pixel height", "Group pixel height", true, 100);
		I_ASSIGN(m_colorAttrPtr, "Color", "Background color", true, "#FFFFFFFF");
		I_ASSIGN_MULTI_0(m_messageIdsAttrPtr, "MessageIds", "General event group component", true);
	I_END_COMPONENT

	// reimplemented (imtloggui::IGroup)
	virtual QByteArray GetId() const override;
	virtual QString GetName() const override;
	virtual int GetHeight() const override;
	virtual QColor GetColor() const override;
	virtual QVector<int> GetSupportedMessageIds() const override;

	// reimplemented (imtloggui::ILayerProvider)
	virtual ILayer* GetLayer(const QByteArray& id) const override;

	// reimplemented (imtbase::ICollectionInfo)
	virtual Ids GetElementIds() const override;
	virtual QVariant GetElementInfo(const QByteArray& elementId, int infoType) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIREF(ILayer, m_layersCompPtr);
	I_ATTR(QByteArray, m_idAttrPtr);
	I_ATTR(QString, m_nameAttrPtr);
	I_ATTR(int, m_heightAttrPtr);
	I_ATTR(QByteArray, m_colorAttrPtr);
	I_MULTIATTR(QByteArray, m_messageIdsAttrPtr);

	QMap<QByteArray, ILayer*> m_layers;
};


} // namespace imtloggui


