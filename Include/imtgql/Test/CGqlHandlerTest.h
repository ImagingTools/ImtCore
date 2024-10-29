#pragma once


#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>
#include <ipackage/CComponentAccessor.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class CGqlHandlerTest: public QObject
{
	Q_OBJECT

public:
	virtual imtgql::CGqlRequest* CreateGqlRequest() const = 0;
	virtual imtbase::CTreeItemModel* CreateExpectedModel() const = 0;

protected:
	virtual bool VerifyResponse(const imtbase::CTreeItemModel& actualModel, const imtbase::CTreeItemModel& expectedModel) const;
	virtual ipackage::CComponentAccessor* GetComponentAccessor() const;

private Q_SLOTS:
	virtual void initTestCase();
	virtual void CreateInternalResponseTest();
	virtual void cleanupTestCase();
protected:
	mutable istd::TSmartPtr<ipackage::CComponentAccessor> m_componentAccessorPtr;
	QString m_registryFile;
	QString m_configFile;
};


} // namespace imtgql


