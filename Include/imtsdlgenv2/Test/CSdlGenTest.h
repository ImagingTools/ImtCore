#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QDir>

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
	void TestComplexUnion();
	void PrinterTest();

	void cleanupTestCase();

private:
	QDir m_tempOutputDir;

};


} //namespace imtsdlgentest
