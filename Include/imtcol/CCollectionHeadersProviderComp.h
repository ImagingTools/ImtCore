#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtcol/ICollectionHeadersProvider.h>


namespace imtcol
{


class CCollectionHeadersProviderComp : public icomp::CComponentBase, virtual public imtcol::ICollectionHeadersProvider
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CCollectionHeadersProviderComp);
		I_REGISTER_INTERFACE(imtcol::ICollectionHeadersProvider);
		I_ASSIGN_MULTI_0(m_headersIdsAttrPtr, "HeaderIds", "Headers Ids", true);
		I_ASSIGN_MULTI_0(m_headersNamesAttrPtr, "HeaderNames", "Headers names", true);
		I_ASSIGN_MULTI_0(m_headerSortableAttrPtr, "HeaderSortable", "Whether header is sortable", false);
		I_ASSIGN_MULTI_0(m_headerFilterableAttrPtr, "HeaderFilterable", "Whether header is filterable", false);
		I_ASSIGN_MULTI_0(m_headerPermissionsAttrPtr, "HeaderPermissions", "Header access permissions", false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtcol::ICollectionHeadersProvider)
	virtual HeaderIds GetHeaderIds() const override;
	virtual bool GetHeaderInfo(const QByteArray& headerId, HeaderInfo& headerInfo) const override;

private:
	I_MULTIATTR(QByteArray, m_headersIdsAttrPtr);
	I_MULTITEXTATTR(m_headersNamesAttrPtr);
	I_MULTIATTR(bool, m_headerSortableAttrPtr);
	I_MULTIATTR(bool, m_headerFilterableAttrPtr);
	I_MULTIATTR(QByteArray, m_headerPermissionsAttrPtr);
};


} // namespace imtcom


