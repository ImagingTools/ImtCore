// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenIntegrationTest.h"


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QMetaEnum>
#include <QtCore/QMetaObject>
#include <QtTest/QTest>

// ImtCore includes
#include <GeneratedFiles/sdlgentestnoobjectssdl/SDL/1.0/CPP/NoObjects.h>
#include <GeneratedFiles/sdlgentestnoobjectssdl/SDL/1.0/CPP/NoObjectsConsumer.h>


/*
	The schemas 'NoObjects' and 'NoObjectsConsumer' are generated with NO_OBJECTS (see the CMake of this test),
	so the '<Schema>Objects' files (the QObject based classes) are not created.
	If the generated source file lost the moc output of the main header, this test is not linked.
*/
#if __has_include(<GeneratedFiles/sdlgentestnoobjectssdl/SDL/1.0/CPP/NoObjectsObjects.h>)
	#define IMT_TEST_NO_OBJECTS_HEADER_EXISTS 1
#else
	#define IMT_TEST_NO_OBJECTS_HEADER_EXISTS 0
#endif

#if __has_include(<GeneratedFiles/sdlgentestnoobjectssdl/SDL/1.0/CPP/NoObjectsConsumerObjects.h>)
	#define IMT_TEST_NO_OBJECTS_CONSUMER_HEADER_EXISTS 1
#else
	#define IMT_TEST_NO_OBJECTS_CONSUMER_HEADER_EXISTS 0
#endif


namespace imtsdlgentest
{


namespace
{


using CTask = sdl::V1_0::sdlgentestnoobjects::CTask;
using CTaskList = sdl::V1_0::sdlgentestnoobjects::CTaskList;
using Priority = sdl::V1_0::sdlgentestnoobjects::Priority;
using EnumPriority = sdl::V1_0::sdlgentestnoobjects::EnumPriority;


CTask CreateTask(const QString& id, Priority priority, const QStringList& tags)
{
	CTask task;
	task.id = id;
	task.priority = priority;
	task.estimate = 5;
	task.tags.emplace();
	for (const QString& tag: tags){
		task.tags->append(tag);
	}

	return task;
}


/// The generated 'operator==' is not used: the arrays are compared element by element
void VerifyTasksEqual(const CTask& actual, const CTask& expected)
{
	QCOMPARE(actual.id.HasValue(), expected.id.HasValue());
	QCOMPARE(*actual.id, *expected.id);
	QCOMPARE(actual.priority.HasValue(), expected.priority.HasValue());
	QCOMPARE(*actual.priority, *expected.priority);
	QCOMPARE(actual.estimate.HasValue(), expected.estimate.HasValue());
	QCOMPARE(*actual.estimate, *expected.estimate);
	QCOMPARE(actual.tags.HasValue(), expected.tags.HasValue());
	QCOMPARE(actual.tags->size(), expected.tags->size());
	for (int i = 0; i < expected.tags->size(); ++i){
		QCOMPARE(actual.tags->at(i).GetValue(), expected.tags->at(i).GetValue());
	}
}


}


void CSdlGenIntegrationTest::TestNoObjectsFilesAreNotGenerated()
{
	QCOMPARE(IMT_TEST_NO_OBJECTS_HEADER_EXISTS, 0);
	QCOMPARE(IMT_TEST_NO_OBJECTS_CONSUMER_HEADER_EXISTS, 0);
}


void CSdlGenIntegrationTest::TestNoObjectsEnumMetaObject()
{
	// the meta object of the namespace is defined by the moc output of the main header
	const QMetaEnum metaEnum = QMetaEnum::fromType<Priority>();
	QVERIFY(metaEnum.isValid());
	QCOMPARE(metaEnum.keyCount(), 3);
	QCOMPARE(QString::fromLatin1(metaEnum.valueToKey(static_cast<int>(Priority::Low))), QStringLiteral("Low"));
	QCOMPARE(QString::fromLatin1(metaEnum.valueToKey(static_cast<int>(Priority::High))), QStringLiteral("High"));

	// the QObject based wrapper of the enum (for QML) is a part of the main header too
	EnumPriority enumObject;
	QVERIFY(enumObject.metaObject() != nullptr);
	QCOMPARE(enumObject.property("Low").toString(), QStringLiteral("Low"));
	QCOMPARE(enumObject.property("Normal").toString(), QStringLiteral("Normal"));
	QCOMPARE(enumObject.property("High").toString(), QStringLiteral("High"));
}


void CSdlGenIntegrationTest::TestNoObjectsJsonRoundTrip()
{
	const CTask source = CreateTask(QStringLiteral("task-1"), Priority::High, {QStringLiteral("a"), QStringLiteral("b")});

	QJsonObject jsonObject;
	QVERIFY(source.WriteToJsonObject(jsonObject));
	QCOMPARE(jsonObject.value(QStringLiteral("id")).toString(), QStringLiteral("task-1"));
	QCOMPARE(jsonObject.value(QStringLiteral("estimate")).toInt(), 5);
	QCOMPARE(jsonObject.value(QStringLiteral("tags")).toArray().size(), 2);

	CTask result;
	QVERIFY(result.ReadFromJsonObject(jsonObject));
	VerifyTasksEqual(result, source);
	QCOMPARE(*result.priority, Priority::High);

	// the schema doesn't accept a task without the required fields
	CTask invalid;
	QVERIFY(!invalid.ReadFromJsonObject(QJsonObject()));
}


void CSdlGenIntegrationTest::TestNoObjectsImportedSchemaJsonRoundTrip()
{
	// the schema without objects imports the types of another schema without objects
	CTaskList source;
	source.title = QStringLiteral("backlog");
	source.defaultPriority = Priority::Normal;
	source.tasks.emplace();
	source.tasks->append(CreateTask(QStringLiteral("task-1"), Priority::Low, {QStringLiteral("x")}));
	source.tasks->append(CreateTask(QStringLiteral("task-2"), Priority::High, {}));

	QJsonObject jsonObject;
	QVERIFY(source.WriteToJsonObject(jsonObject));
	QCOMPARE(jsonObject.value(QStringLiteral("tasks")).toArray().size(), 2);

	CTaskList result;
	QVERIFY(result.ReadFromJsonObject(jsonObject));
	QCOMPARE(*result.title, *source.title);
	QCOMPARE(*result.defaultPriority, Priority::Normal);
	QCOMPARE(result.tasks->size(), 2);
	for (int i = 0; i < 2; ++i){
		VerifyTasksEqual(result.tasks->at(i).GetValue(), source.tasks->at(i).GetValue());
	}
}


} // namespace imtsdlgentest
