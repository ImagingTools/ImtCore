#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtsdl/ISdlEditableProcessArgumentsParser.h>


namespace imtsdlgentest
{


class CSdlGenTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();

	void TestBasicSchema();
	void TestComplexCollectionFilter();
	void TestUnion();

	void cleanupTestCase();

private:
	QList<std::shared_ptr<QTemporaryDir>> m_tempDirPtrList;

};


} //namespace imtsdlgentest
