#include <imtbase/CReportPage.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// public methods

// reimplemented (IReportPage)

IReportPage::ElementIds CReportPage::GetPageElements() const
{
	ElementIds retVal;

	return retVal;
}


const IGraphicsElement* CReportPage::GetPageElement(const QByteArray& elementId) const
{
	return NULL;
}


// reimplemented (iser::ISerializable)

bool CReportPage::Serialize(iser::IArchive & archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? NULL : this);

	return retVal;
}


} // namespace imtbase


