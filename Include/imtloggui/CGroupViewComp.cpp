#include <imtloggui/CGroupViewComp.h>

// Qt includes
#include <QtGui/QColor>


namespace imtloggui
{


QColor CGroupViewComp::GetBackgroundColor() const
{
	QColor color(Qt::transparent);

	if (m_colorAttrPtr.IsValid()){
		color = QColor(*m_colorAttrPtr);
	}

	return color;
}


// reimplemented (imtbase::IObjectCollection)

const istd::IChangeable* CGroupViewComp::GetObjectPtr(const QByteArray& objectId) const
{
	int index = GetIndex(objectId);
	if (index >= 0){
		return m_layerViewCompPtr[index];
	}

	return nullptr;
}


// reimplemented (imtbase::ICollectionInfo)

imtbase::ICollectionInfo::Ids CGroupViewComp::GetElementIds() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_layerViewCompPtr.GetCount());

	imtbase::ICollectionInfo::Ids retVal;
	for (int i = 0; i < count; i++){
		retVal.append(m_idAttrPtr[i]);
	}

	return retVal;
}


QVariant CGroupViewComp::GetElementInfo(const QByteArray& elementId, int infoType) const
{
	int index = GetIndex(elementId);

	QVariant retVal;

	if (index >= 0){
		switch (infoType){
		case EIT_NAME:
			retVal = m_nameAttrPtr[index];
			break;
		}
	}

	return retVal;
}


// private methods

int CGroupViewComp::GetCount() const
{
	int count = qMin(m_idAttrPtr.GetCount(), m_nameAttrPtr.GetCount());
	return qMin(count, m_layerViewCompPtr.GetCount());
}


int CGroupViewComp::GetIndex(const QByteArray& id) const
{
	int count = GetCount();

	for (int i = 0; i < count; i++){
		if (m_idAttrPtr[i] == id){
			return i;
		}
	}

	return -1;
}


} // namespace imtloggui


