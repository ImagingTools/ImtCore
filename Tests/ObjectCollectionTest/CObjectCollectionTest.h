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


#include <ObjectCollectionTest/CObjectCollectionPartituraTestBase.h>


class CObjectCollectionTest: public QObject
{
	Q_OBJECT

private slots:
	void InsertNewObjectWithDataTest();
	void CheckSerializeTest();

private:
	QString m_imtCorePath;
	QString m_registryFileWithoutFixedTypeIds;
	QString m_registryFileWithFixedTypeIds;
	QString m_configFile;
	QByteArray m_typeIdObjectCollection;
};
