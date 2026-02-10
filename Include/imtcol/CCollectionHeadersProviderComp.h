// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtcol/ICollectionHeadersProvider.h>


namespace imtcol
{


class CCollectionHeadersProviderComp : public icomp::CComponentBase, virtual public ICollectionHeadersProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionHeadersProviderComp);
		I_REGISTER_INTERFACE(imtcol::ICollectionHeadersProvider);
		I_ASSIGN_MULTI_0(m_headersIdsAttrPtr, "HeaderIds", "Headers Ids", true);
		I_ASSIGN_MULTI_0(m_arrayTypeHeadersIdsAttrPtr, "ArrayTypeHeaderIds", "List of header IDs that represent ARRAY-type fields. Any header NOT listed here is treated as SCALAR by default.", false);
		I_ASSIGN_MULTI_0(m_headersNamesAttrPtr, "HeaderNames", "Headers names", true);
		I_ASSIGN_MULTI_0(m_sortableHeaderIdsAttrPtr, "SortableHeaderIds", "Header-IDs that can be sorted", false);
		I_ASSIGN_MULTI_0(m_filterableHeaderIdsAttrPtr, "FilterableHeaderIds", "Header-IDs that can be filtered", false);
		I_ASSIGN_MULTI_0(m_headerPermissionsAttrPtr, "HeaderPermissions", "Header access permissions", false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtcol::ICollectionHeadersProvider)
	virtual HeaderIds GetHeaderIds() const override;
	virtual bool GetHeaderInfo(const QByteArray& headerId, HeaderInfo& headerInfo) const override;

private:
	I_MULTIATTR(QByteArray, m_headersIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_arrayTypeHeadersIdsAttrPtr);
	I_MULTITEXTATTR(m_headersNamesAttrPtr);
	I_MULTIATTR(QByteArray, m_sortableHeaderIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_filterableHeaderIdsAttrPtr);
	I_MULTIATTR(QByteArray, m_headerPermissionsAttrPtr);
};


} // namespace imtcom


