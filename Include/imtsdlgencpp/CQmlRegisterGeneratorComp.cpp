// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CQmlRegisterGeneratorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
#include <istd/istd.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEntryBase.h>


namespace imtsdlgencpp
{


// static helpers
void GenerateRegisterFunction(QTextStream& stream, const imtsdl::CSdlEntryBase& entry, bool isSingleton)
{
	imtsdl::CSdlTools::FeedStreamHorizontally(stream);

	const QString sdlNamespace = imtsdl::CSdlTools::BuildQmlImportDeclarationFromParams(entry.GetSchemaParams(), "Sdl", false);

	if (isSingleton){
		stream << QStringLiteral("qmlRegisterSingletonType<Enum") << imtsdl::CSdlTools::GetCapitalizedValue(entry.GetName());
		stream << QStringLiteral(">(") ;
		stream << QStringLiteral("\"") << sdlNamespace << QStringLiteral("\", 1, 0, ");
		stream << QStringLiteral("\"") << entry.GetName() << QStringLiteral("\"");
		stream << QStringLiteral(", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("Q_UNUSED(engine)");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("Q_UNUSED(scriptEngine)");
		imtsdl::CSdlTools::FeedStream(stream, 2, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("Enum") << entry.GetName() << QStringLiteral(" *enumType = new ");
		stream << QStringLiteral("Enum") << entry.GetName() << QStringLiteral("();");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return enumType;");
		imtsdl::CSdlTools::FeedStream(stream, 1, false);
		imtsdl::CSdlTools::FeedStreamHorizontally(stream);
		stream << QStringLiteral("});");

	}
	else{
		stream << QStringLiteral("qmlRegisterType<C") << entry.GetName() << QStringLiteral("Object>(") ;
		stream << QStringLiteral("\"") << sdlNamespace << QStringLiteral("\", 1, 0, ");
		stream << QStringLiteral("\"") << entry.GetName() << QStringLiteral("\");");
	}
	imtsdl::CSdlTools::FeedStream(stream, 1, false);
}


template <class T>
void GenerateRegisterFunctions(QTextStream& stream, const QList<T>& entryList, bool isSingleton)
{
	for (const auto& entry: entryList){
		GenerateRegisterFunction(stream, entry, isSingleton);
	}
}


// public methods

bool CQmlRegisterGeneratorComp::ProcessEntry (
			const imtsdl::CSdlEntryBase& /* sdlEntry */,
			QIODevice* headerDevicePtr,
			QIODevice* /*sourceDevicePtr*/,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_customSchemaParamsCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	if (headerDevicePtr == nullptr){
		// nothing todo
		return true;
	}

	QTextStream stream(headerDevicePtr);
	FeedStream(stream, 2, false);

	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	stream << QStringLiteral("#ifdef QT_QML_LIB");
	FeedStream(stream, 1, false);

	// add function
	stream << QStringLiteral("[[maybe_unused]] static void RegisterQmlTypes()");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	GenerateRegisterFunctions(stream, m_sdlTypeListCompPtr->GetSdlTypes(true), false);
	GenerateRegisterFunctions(stream, m_sdlEnumListCompPtr->GetEnums(true), true);

	stream << '}';
	FeedStream(stream, 1, false);

	stream << QStringLiteral("#endif");
	FeedStream(stream, 2, false);

	return true;
}


// reimplemented (ICxxFileProcessor)

QList<imtsdl::IncludeDirective> CQmlRegisterGeneratorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {};

	return retVal;
}


} // namespace imtsdlgencpp


