#include <imtdb/CStandardSqlMetaInfoTableDelegateComp.h>


// Qt includes
#include <QtGui/QImage>


namespace imtdb
{


// reimplemented (imtdb::IMetaInfoTableDelegate)

QByteArrayList CStandardSqlMetaInfoTableDelegateComp::GetColumnIds() const
{
	QByteArrayList retVal;

	for (const QPair<int, QByteArray>& item : m_metaInfoIds){
		retVal += QByteArray("\"") + item.second + QByteArray("\"");
	}

	return retVal;
}


int CStandardSqlMetaInfoTableDelegateComp::GetMetaInfoType(const QByteArray& columnId) const
{
	QString temp = columnId;
	temp.remove("\"");

	QByteArray metaInfoId = temp.toLocal8Bit();
	
	for (const QPair<int, QByteArray>& item : m_metaInfoIds){
		if (item.second == metaInfoId){
			return item.first;
		}
	}
	
	return -1;
}


QVariant CStandardSqlMetaInfoTableDelegateComp::ToTableRepresentation(const QVariant& data, const QByteArray& /*columnId*/) const
{
	switch (data.type()){
	case QVariant::Image: {
		QImage image = data.value<QImage>();

		QByteArray imageBuffer;
		QDataStream dataStream(&imageBuffer, QIODevice::ReadWrite);

		dataStream << image;

		return imageBuffer.toBase64();
	}
	break;

	default:
		return data;
	}
}


QVariant CStandardSqlMetaInfoTableDelegateComp::FromTableRepresentation(const QVariant& data, const QByteArray& columnId) const
{
	if (columnId == "Preview"){
		QByteArray imageBuffer = QByteArray::fromBase64(data.toByteArray());
		
		QDataStream dataStream(imageBuffer);
		QImage image;

		dataStream >> image;

		qDebug() << image;

		return image;
	}

	return data;
}


idoc::MetaInfoPtr CStandardSqlMetaInfoTableDelegateComp::CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId) const
{
	if (m_objectMetaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr retVal;
		if (m_objectMetaInfoCreatorCompPtr->CreateMetaInfo(dataPtr, typeId, retVal)){
			return retVal;
		}

		return retVal;
	}

	return idoc::MetaInfoPtr();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CStandardSqlMetaInfoTableDelegateComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_objectMetaInfoCreatorCompPtr.IsValid()){
		idoc::MetaInfoPtr retVal;
		if (m_objectMetaInfoCreatorCompPtr->CreateMetaInfo(nullptr, *m_objectTypeIdAttrPtr, retVal)){
			idoc::IDocumentMetaInfo::MetaInfoTypes types = retVal->GetMetaInfoTypes();
			for (int type : types){
				m_metaInfoIds.push_back(QPair<int, QByteArray>(type, retVal->GetMetaInfoId(type)));
			}
		}
	}
}


} // namespace imtdb


