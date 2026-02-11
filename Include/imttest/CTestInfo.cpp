// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imttest/CTestInfo.h>


// ACF includes
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


// reimplemented (imtlic::ITestInfo)

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


// reimplemented (iser::ISerializable)

bool CTestInfo::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag testIdTag("TestId", "Test id", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(testIdTag);
	retVal = retVal && archive.Process(m_testId);
	retVal = retVal && archive.EndTag(testIdTag);

	iser::CArchiveTag testNameTag("TestName", "Test name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(testNameTag);
	retVal = retVal && archive.Process(m_testName);
	retVal = retVal && archive.EndTag(testNameTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CTestInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CTestInfo::CopyFrom(const IChangeable &object, CompatibilityMode /* mode */)
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


istd::IChangeableUniquePtr CTestInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CTestInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
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


