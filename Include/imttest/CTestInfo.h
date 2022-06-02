#pragma once

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imttest/ITestInfo.h>
#include <imod/CModelUpdateBridge.h>
#include <imtbase/IObjectCollection.h>


namespace imttest
{


/**
	Common implementation of ITestInfo interface.
	\sa ITestInfo
	\ingroup LicenseManagement
*/
class CTestInfo: virtual public imttest::ITestInfo
{
public:
	CTestInfo();

	// reimplemented (imtlic::ITestInfo)
	virtual QByteArray GetTestId() const override;
	virtual QString GetTestName() const override;
	virtual QString GetTestDescription() const override;
	virtual void SetTestId(QByteArray testId) override;
	virtual void SetTestName(QString testName) override;
	virtual void SetTestDescription(QString testDescription) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	const imtbase::IObjectCollection* m_testCollectionPtr;

private:
	QByteArray m_testId;
	QString m_testName;
	QString m_testDescription;
	imod::CModelUpdateBridge m_testUpdateBridge;
};


} // namespace imtlic


