#include <imtreports/CReportPage.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtreports
{


// public methods

// reimplemented (IReportPage)

IReportPage::ElementIds CReportPage::GetPageElements() const
{
	return m_pageElementsMap.keys();
}


const IGraphicsElement* CReportPage::GetPageElement(const QByteArray& elementId) const
{
	if (m_pageElementsMap.contains(elementId)){
		return m_pageElementsMap[elementId].elementPtr.GetPtr();
	}

	return NULL;
}


// reimplemented (iser::ISerializable)

bool CReportPage::Serialize(iser::IArchive & archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? NULL : this);

	return retVal;
}


} // namespace imtreports


