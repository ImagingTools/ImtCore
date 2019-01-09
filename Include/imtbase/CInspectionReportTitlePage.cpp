#include <imtbase/CInspectionReportTitlePage.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtbase
{


// public methods

QString CInspectionReportTitlePage::GetProductName() const
{
	return m_productName;
}


void CInspectionReportTitlePage::SetProductName(const QString & productName)
{
	if (m_productName != productName){
		istd::CChangeNotifier changeNotifier(this);

		m_productName = productName;
	}
}


// reimplemented (iser::ISerializable)

bool CInspectionReportTitlePage::Serialize(iser::IArchive & archive)
{
	bool retVal = true;
	static iser::CArchiveTag productNameTag("ProductName", "Name of the product", iser::CArchiveTag::TT_LEAF);

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? NULL : this);

	retVal = retVal && archive.BeginTag(productNameTag);
	retVal = retVal && archive.Process(m_productName);
	retVal = retVal && archive.EndTag(productNameTag);

	return true;
}


} // namespace imtbase


