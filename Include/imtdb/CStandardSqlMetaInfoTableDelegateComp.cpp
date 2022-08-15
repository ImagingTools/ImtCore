#include <imtdb/CStandardSqlMetaInfoTableDelegateComp.h>


namespace imtdb
{


// reimplemented (imtdb::IMetaInfoTableDelegate)

QByteArrayList CStandardSqlMetaInfoTableDelegateComp::GetColumnIds() const
{
	QByteArrayList retVal;

	for (const QPair<int, QByteArray>& item : m_metaInfoIds){
		retVal += item.second;
	}

	return retVal;
}


imtdb::IMetaInfoTableDelegate::ColumnDataType CStandardSqlMetaInfoTableDelegateComp::GetColumnType(const QByteArray& columnId) const
{
	return CDI_UNKNOWN;
}


int CStandardSqlMetaInfoTableDelegateComp::GetMetaInfoType(const QByteArray& columnId) const
{
	for (const QPair<int, QByteArray>& item : m_metaInfoIds){
		if (item.second == columnId){
			return item.first;
		}
	}
	
	return -1;
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


