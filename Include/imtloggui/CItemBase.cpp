#include <imtloggui/CItemBase.h>


// ACF includes
#include <istd/CChangeNotifier.h>


namespace imtloggui
{


// public methods

void CItemBase::SetParams(QGraphicsItem *parent)
{
	setParentItem(parent);
}


// reimplemented (imtloggui::IItemBase)

bool CItemBase::CollidesWithItem(const IItemBase* otherPtr, double scale) const
{
	const QGraphicsItem* graphicsItemPtr = dynamic_cast<const QGraphicsItem*>(otherPtr);

	if (graphicsItemPtr == nullptr){
		return false;
	}

	int w1 = this->boundingRect().width();
	int w2 = graphicsItemPtr->boundingRect().width();

	int dist = abs(pos().x() - graphicsItemPtr->pos().x()) * scale;

	return dist < (w1 + w2)*0.5;
}


// reimplemented (idoc::IDocumentMetaInfo)

idoc::IDocumentMetaInfo::MetaInfoTypes CItemBase::GetMetaInfoTypes(bool allowReadOnly) const
{
	MetaInfoTypes retVal;

	for (int key : m_metaInfo.keys()){
		if (!allowReadOnly && !m_metaInfo[key].isWrittable){
			continue;
		}

		retVal.insert(key);
	}

	return retVal;
}


QVariant CItemBase::GetMetaInfo(int metaInfoType) const
{
	if (m_metaInfo.contains(metaInfoType)){
		return m_metaInfo[metaInfoType].value;
	}

	return QVariant();
}


bool CItemBase::SetMetaInfo(int metaInfoType, const QVariant& metaInfo)
{
	if (m_metaInfo.contains(metaInfoType)){
		if (m_metaInfo[metaInfoType].isWrittable){
			istd::CChangeNotifier notifier(this);

			m_metaInfo[metaInfoType].value = metaInfo;

			UpdateToolTip();

			return true;
		}
	}

	return false;
}


QString CItemBase::GetMetaInfoName(int metaInfoType) const
{
	if (m_metaInfo.contains(metaInfoType)){
		return m_metaInfo[metaInfoType].name;
	}

	return QString();
}


QString CItemBase::GetMetaInfoDescription(int /*metaInfoType*/) const
{
	return QString();
}


bool CItemBase::IsMetaInfoWritable(int metaInfoType) const
{
	if (m_metaInfo.contains(metaInfoType)){
		return m_metaInfo[metaInfoType].isWrittable;
	}

	return false;
}


// protected methods

bool CItemBase::AddMetaInfo(int metaInfoType, const QString& metaInfoName, bool isMetaInfoWritable)
{
	if (!m_metaInfo.contains(metaInfoType)){
		m_metaInfo[metaInfoType].name = metaInfoName;
		m_metaInfo[metaInfoType].isWrittable = isMetaInfoWritable;

		return true;
	}

	return false;
}


bool CItemBase::RemoveMetaInfo(int metaInfoType)
{
	return m_metaInfo.remove(metaInfoType) > 0;
}


// private methods

void CItemBase::UpdateToolTip()
{
	QString toolTip;

	for (MetaInfoItem metaInfoItem : m_metaInfo){
		QVariant var = metaInfoItem.value;
		QString varString = var.toString();
		toolTip += metaInfoItem.name + ": " + varString + "\n";
	}

	if (!toolTip.isEmpty()){
		toolTip.chop(1);
	}

	setToolTip(toolTip);
}


} // namespace imtloggui


