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


private:
	QString m_imtCorePath;
	QString m_registryFile;
	QString m_configFile;
	QByteArray m_typeIdObjectCollection;
};
