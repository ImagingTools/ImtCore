// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcol/CCollectionHeadersProviderComp.h>


namespace imtcol
{


// protected methods

// reimplemented (imtcol::ICollectionHeadersProvider)

ICollectionHeadersProvider::HeaderIds CCollectionHeadersProviderComp::GetHeaderIds() const
{
	HeaderIds retVal;

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

	if (m_filterableHeaderIdsAttrPtr.IsValid()){
		headerInfo.filterable = m_filterableHeaderIdsAttrPtr.FindValue(headerId) >= 0;
	}

	if (m_sortableHeaderIdsAttrPtr.IsValid()){
		headerInfo.sortable =  m_sortableHeaderIdsAttrPtr.FindValue(headerId) >= 0;
	}

	if (m_headerPermissionsAttrPtr.GetCount() > headerIndex){
		headerInfo.permissionId = m_headerPermissionsAttrPtr[headerIndex];
	}

	if (m_arrayTypeHeadersIdsAttrPtr.IsValid()){
		if (m_arrayTypeHeadersIdsAttrPtr.FindValue(headerId) >= 0){
			headerInfo.fieldType = FT_ARRAY;
		}
	}

	return true;
}


} // namespace imtcol


