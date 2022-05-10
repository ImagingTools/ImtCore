#include <imtbase/CCollectionFilter.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtbase
{


CCollectionFilter::CCollectionFilter()
	:m_filter(QString()),
	m_sortingOrder(SO_NO_ORDER),
	m_sortingInfoIds(QByteArrayList())
{
}


QString CCollectionFilter::GetTextFilter() const
{
	return m_filter;
}


void CCollectionFilter::SetTextFilter(const QString &filter)
{
	if(m_filter != filter){
	istd::CChangeNotifier changeNotifier(this);
	m_filter = filter;
}
}


ICollectionFilter::SortingOrder CCollectionFilter::GetSortingOrder() const
{
	return m_sortingOrder;
}


void CCollectionFilter::SetSortingOrder(SortingOrder sortingOrder)
{
	if (m_sortingOrder == sortingOrder){
	istd::CChangeNotifier changeNotifier(this);
	m_sortingOrder = sortingOrder;
}
}


QByteArrayList CCollectionFilter::GetSortingInfoId() const
{
	return m_sortingInfoId;
}


void CCollectionFilter::SetSortingInfoId(const QByteArrayList &sortingInfoId)
{
	if(m_sortingInfoId == sortingInfoId){
	istd::CChangeNotifier changeNotifier(this);
		m_sortingInfoId = sortingInfoId;
	}
}


bool CCollectionFilter::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag filterTag("Filter", "Text filter", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(filterTag);
	retVal = retVal && archive.Process(m_filter);
	retVal = retVal && archive.EndTag(filterTag);

	static iser::CArchiveTag sortingOrderTag("SortingOrder", "Type of sorting order", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sortingOrderTag);
	retVal = retVal && I_SERIALIZE_ENUM(SortingOrder, archive, m_sortingOrder);
	retVal = retVal && archive.EndTag(sortingOrderTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_sortingInfoIds, "SortingInfoIds", "SortingInfoId");

	return retVal;
}


int CCollectionFilter::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CCollectionFilter::CopyFrom(const IChangeable &object, CompatibilityMode mode)
{
	const CCollectionFilter* implPtr = dynamic_cast<const CCollectionFilter*>(&object);
	if (implPtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_filter = implPtr->m_filter;
		m_sortingOrder = implPtr->m_sortingOrder;
		m_sortingInfoIds = implPtr->m_sortingInfoIds;

		return true;
	}

	return false;
}


bool CCollectionFilter::IsEqual(const IChangeable &object) const
{
	const CCollectionFilter* implPtr = dynamic_cast<const CCollectionFilter*>(&object);
	if (implPtr != nullptr){
		return
					(m_filter == implPtr->m_filter) &&
					(m_sortingOrder == implPtr->m_sortingOrder) &&
					(m_sortingInfoIds == implPtr->m_sortingInfoIds);
	}

	return false;
}


istd::IChangeable *CCollectionFilter::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CCollectionFilter> clonePtr(new CCollectionFilter);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CCollectionFilter::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_filter.clear();
	m_sortingOrder = SO_NO_ORDER;
	m_sortingInfoIds.clear();

	return true;
}


} // namespace imtbase


