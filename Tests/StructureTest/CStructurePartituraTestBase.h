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


class CStructurePartituraTestBase: public QObject
{
    Q_OBJECT

protected:
	virtual void initTestCase() = 0;

private slots:
	//void cleanup();
	void InsertNewNodeWithRequiredParamsTest_data();
	void InsertNewNodeWithRequiredParamsTest();
	void SetNodeNameTest();
	void SetNodeDescriptionTest();
	void MoveNodeTest();
	void RemoveNodeTest();
	void InsertObjectWithRequiredParamsTest_data();
	void InsertObjectWithRequiredParamsTest();
	void MoveObjectTest();
	void RemoveObjectTest();
	void GetNodePathTest();
	void cleanupTestCase();

protected:
    QString m_imtCorePath;
    QString m_registryFile;
    QString m_configFile;
	QString m_nameAccessor;
};

template <const char* partituraPath>
class CStructurePartituraTestBaseRegister : public CStructurePartituraTestBase
{
protected:
    virtual void initTestCase(){
        // paths to test component accessor and config for him
        m_imtCorePath = qEnvironmentVariable("IMTCOREDIR");
		m_registryFile = m_imtCorePath + QString("/Tests/StructureTest/Partitura/%1.acc").arg(partituraPath);
        m_configFile = m_imtCorePath + "/Config/ImtCoreTest.awc";
		m_nameAccessor = partituraPath;
    };
};
