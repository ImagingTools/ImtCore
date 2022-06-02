#include <imttest/CTestInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imttest
{


// public methods

CTestInfo::CTestInfo()
	:m_testCollectionPtr(nullptr),
	m_testUpdateBridge(this)
{
}

QByteArray CTestInfo::GetTestId() const
{
	return m_testId;
}

QString CTestInfo::GetTestName() const
{
	return m_testName;
}

QString CTestInfo::GetTestDescription() const
{
	return m_testDescription;
}

void CTestInfo::SetTestId(QByteArray testId)
{
	if (m_testId != testId){
		istd::CChangeNotifier notifier(this);
		m_testId = testId;
	}
}

void CTestInfo::SetTestName(QString testName)
{
	if (m_testName != testName){
		istd::CChangeNotifier notifier(this);
		m_testName = testName;
	}
}

void CTestInfo::SetTestDescription(QString testDescription)
{
	if (m_testDescription != testDescription){
		istd::CChangeNotifier notifier(this);
		m_testDescription = testDescription;
	}
}

bool CTestInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag testIdTag("TestId", "Test id", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(testIdTag);
	retVal = retVal && archive.Process(m_testId);
	retVal = retVal && archive.EndTag(testIdTag);

	static iser::CArchiveTag testNameTag("TestName", "Test name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(testNameTag);
	retVal = retVal && archive.Process(m_testName);
	retVal = retVal && archive.EndTag(testNameTag);

	return retVal;
}

int CTestInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}

bool CTestInfo::CopyFrom(const IChangeable &object, CompatibilityMode mode)
{
	istd::CChangeGroup changeGroup(this);

	const CTestInfo* sourcePtr = dynamic_cast<const CTestInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_testId = sourcePtr->m_testId;
		m_testName = sourcePtr->m_testName;

		bool retVal = true;

		return retVal;
	}

	return false;
}

istd::IChangeable *CTestInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CTestInfo> clonePtr(new CTestInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}

bool CTestInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_testDescription.clear();
	m_testId.clear();
	m_testName.clear();

	return true;
}


} // namespace imtlic


