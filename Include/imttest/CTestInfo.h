// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imttest/ITestInfo.h>
#include <imtbase/CCollectionInfo.h>
#include <imtbase/IObjectCollection.h>


namespace imttest
{


class CTestInfo: virtual public ITestInfo
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
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	const imtbase::IObjectCollection* m_testCollectionPtr;

private:
	QByteArray m_testId;
	QString m_testName;
	QString m_testDescription;
	imod::CModelUpdateBridge m_testUpdateBridge;
};


} // namespace imttest


