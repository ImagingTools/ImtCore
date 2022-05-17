#pragma once

// std includes
#include <memory>

// Qt includes
#include <QtCore/QObject>
#include <QtTest/QtTest>

// ACF includes
#include <itest/CStandardTestExecutor.h>
#include <ipackage/CComponentAccessor.h>
#include <istd/TDelPtr.h>

#include <imtbase/IObjectCollection.h>


class CObjectCollectionPartituraTestBase: public QObject
{
    Q_OBJECT

protected:
	virtual void initTestCase() = 0;

private slots:

    void InsertNewObjectWithRequiredParamsTest_data();
    void InsertNewObjectWithRequiredParamsTest();
    void InsertNewObjectWithNonExistElementTest();
    void InsertNewObjectWithExistElementTest();
	void InsertNewObjectWithDataTest();
	void InsertNewObjectWithMetaObjectTest();
    void RemoveExistObjectTest();
    void RemoveNonExistObjectTest();
	void ResetCollectionWithFixedObjectTest();
    void ResetCollectionWithoutFixedObjectsTest();
	void SetObjectNameTest();
	void SetObjectDescriptionTest();
	void GetObjectTypeIdTest();
	void GetOperationFlagsFixedObjectTest();
	void GetOperationFlagsInsertedObjectTest();
	void PaginationTest_data();
	void PaginationTest();
    void cleanupTestCase();

protected:
    QString m_imtCorePath;
    QString m_registryFile;
    QString m_configFile;
	QString m_nameAccessor;
    QByteArray m_typeIdObjectCollection;
};

template <const char* partituraPath>
class CObjectCollectionPartituraTestBaseRegister : public CObjectCollectionPartituraTestBase
{
protected:
    virtual void initTestCase(){
        // paths to test component accessor and config for him
        m_imtCorePath = qEnvironmentVariable("IMTCOREDIR");
		m_registryFile = m_imtCorePath + QString("/Tests/ObjectCollectionTest/Partitura/%1.acc").arg(partituraPath);
        m_configFile = m_imtCorePath + "/Config/ImtCoreTest.awc";
		m_nameAccessor = partituraPath;
        // set type id for tests
        m_typeIdObjectCollection = "AccountInfo";
    };
};
