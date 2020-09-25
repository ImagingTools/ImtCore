#include <imtloggui/CGroupComp.h>


// Qt includes
#include <QtGui/QColor.h>


namespace imtloggui
{


// reimplemented (imtloggui::IGroup)

QByteArray CGroupComp::GetId() const
{
	return *m_idAttrPtr;
}


QString CGroupComp::GetName() const
{
	return *m_nameAttrPtr;
}


int CGroupComp::GetHeight() const
{
	return *m_heightAttrPtr;
}


QColor CGroupComp::GetColor() const
{
	return QColor(QString(*m_colorAttrPtr));
}


QVector<int> CGroupComp::GetSupportedMessageIds() const
{
	QVector<int> retVal;

	retVal.reserve(m_messageIdsAttrPtr.GetCount());
	for (int i = 0; i < m_messageIdsAttrPtr.GetCount(); i++){
		retVal.append(*m_messageIdsAttrPtr[i]);
	}

	return retVal;
}


// reimplemented (imtloggui::ILayerProvider)

ILayer* CGroupComp::GetLayer(const QByteArray& id) const
{
	if (m_layers.contains(id)){
		return m_layers[id];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CGroupComp::GetElementIds() const
{
	return m_layers.keys().toVector();
}


QVariant CGroupComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	QVariant retVal;

	if (m_layers.contains(elementId)){
		switch (infoType){
		case EIT_NAME:
			retVal = m_layers[elementId]->GetName();
			break;
		}
	}

	return retVal;
}


// reimplemented (icomp::CComponentBase)

void CGroupComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	for (int i = 0; i < m_layersCompPtr.GetCount(); i++){
		m_layers[m_layersCompPtr[i]->GetId()] = m_layersCompPtr[i];
	}
}


} // namespace imtloggui


