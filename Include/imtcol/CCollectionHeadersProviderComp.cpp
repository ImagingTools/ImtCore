#include <imtcol/CCollectionHeadersProviderComp.h>


namespace imtcol
{


// protected methods

// reimplemented (imtcol::ICollectionHeadersProvider)

imtcol::ICollectionHeadersProvider::HeaderIds CCollectionHeadersProviderComp::GetHeaderIds() const
{
	imtcol::ICollectionHeadersProvider::HeaderIds retVal;

	for (int i = 0; i < m_headersIdsAttrPtr.GetCount(); ++i){
		retVal << m_headersIdsAttrPtr[i];
	}

	return retVal;
}


bool CCollectionHeadersProviderComp::GetHeaderInfo(const QByteArray& headerId, HeaderInfo& headerInfo) const
{
	int headerIndex = m_headersIdsAttrPtr.FindValue(headerId);
	if (headerIndex < 0){
		return false;
	}

	if (m_headersIdsAttrPtr.GetCount() > headerIndex){
		headerInfo.headerId = m_headersIdsAttrPtr[headerIndex];
	}

	if (m_headersNamesAttrPtr.GetCount() > headerIndex){
		headerInfo.headerName = m_headersNamesAttrPtr[headerIndex];
	}

	if (m_headerSortableAttrPtr.GetCount() > headerIndex){
		headerInfo.sortable = m_headerSortableAttrPtr[headerIndex];
	}

	if (m_headerFilterableAttrPtr.GetCount() > headerIndex){
		headerInfo.filterable = m_headerFilterableAttrPtr[headerIndex];
	}

	if (m_headerPermissionsAttrPtr.GetCount() > headerIndex){
		headerInfo.permissionId = m_headerPermissionsAttrPtr[headerIndex];
	}

	return true;
}


} // namespace imtcol


