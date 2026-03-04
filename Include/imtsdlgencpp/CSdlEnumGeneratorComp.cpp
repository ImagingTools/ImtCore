// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlEnumGeneratorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEnum.h>


namespace imtsdlgencpp
{


// reimplemented (ICxxFileProcessor)

bool CSdlEnumGeneratorComp::ProcessEntry(
			const imtsdl::CSdlEntryBase& sdlEntry,
			QIODevice* headerDevicePtr,
			QIODevice* /*sourceDevicePtr*/,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_customSchemaParamsCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	const auto* sdlEnum = dynamic_cast<const imtsdl::CSdlEnum*>(&sdlEntry);
	if (sdlEnum == nullptr){
		return false;
	}

	QTextStream stream(headerDevicePtr);
	// enum create
	stream << QStringLiteral("enum class ");
	stream << sdlEnum->GetName();
	stream << QStringLiteral(" {");
	FeedStream(stream, 1, false);

	// add all enum values
	for (const std::pair<QString, QString>& enumValue: sdlEnum->GetValues()){
		FeedStreamHorizontally(stream);
		stream << enumValue.first;
		stream << ',';
		FeedStream(stream, 1, false);
	}

	stream << QStringLiteral("};");
	FeedStream(stream, 2, false);

	stream << QStringLiteral("Q_ENUM_NS(") << sdlEnum->GetName() << QStringLiteral(")");
	FeedStream(stream, 3, false);

	stream << QStringLiteral("class Enum") << GetCapitalizedValue(sdlEnum->GetName());
	stream << QStringLiteral(": public QObject");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_OBJECT");
	FeedStream(stream, 1, false);

	for (const auto& enumValue: sdlEnum->GetValues()){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("Q_PROPERTY(QString ") << enumValue.first;
		stream << QStringLiteral(" READ Get") << GetCapitalizedValue(enumValue.first);
		stream << QStringLiteral(" NOTIFY ") << GetCapitalizedValue(enumValue.first);
		stream << QStringLiteral("Changed)");
		FeedStream(stream, 1, false);
	}
	FeedStream(stream, 1, false);

	// getters of class
	stream << QStringLiteral("protected:");
	FeedStream(stream, 1, false);

	for (const auto& enumValue: sdlEnum->GetValues()){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("static QString Get") << GetCapitalizedValue(enumValue.first);
		stream << QStringLiteral("() { return \"") << enumValue.first;
		stream << QStringLiteral("\"; }");
		FeedStream(stream, 1, false);
	}
	FeedStream(stream, 1, false);

	// signals of class
	stream << QStringLiteral("signals:");
	FeedStream(stream, 1, false);

	for (const auto& enumValue: sdlEnum->GetValues()){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("void ") << GetCapitalizedValue(enumValue.first);
		stream << QStringLiteral("Changed();");
		FeedStream(stream, 1, false);
	}

	// end of class
	stream << QStringLiteral("};");
	FeedStream(stream, 3, false);


	return true;
}

// reimplemented (IIncludeDirectivesProvider)

QList<imtsdl::IncludeDirective> CSdlEnumGeneratorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {};

	return retVal;
}


} // namespace imtsdlgencpp

