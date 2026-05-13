// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlUnionGeneratorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlField.h>


namespace imtsdlgencpp
{


// public methods

// reimplemented (ICxxFileProcessor)

bool CSdlUnionGeneratorComp::ProcessEntry(
			const imtsdl::CSdlEntryBase& sdlEntry,
			QIODevice* headerDevicePtr,
			QIODevice* /*sourceDevicePtr*/,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());
	Q_ASSERT(m_customSchemaParamsCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	const imtsdl::CSdlUnion* sdlUnion = dynamic_cast<const imtsdl::CSdlUnion*>(&sdlEntry);
	if (sdlUnion == nullptr || headerDevicePtr == nullptr){
		return true;
	}

	QTextStream stream(headerDevicePtr);

	QList<QString> convertedTypeList;
	for (const auto& sdlType : sdlUnion->GetTypes()){
		imtsdl::CSdlField field;
		field.SetType(sdlType);
		bool isCustom = false;
		QString convertedType = OptListConvertTypeWithNamespaceStruct(
			field,
			GetNamespaceFromSchemaParams(sdlUnion->GetSchemaParams()),
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			true,
			&isCustom,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			false);
		convertedTypeList << convertedType;

		if (isCustom){
			imtsdl::CSdlField lookupField;
			lookupField.SetType(sdlType);
			std::shared_ptr<imtsdl::CSdlEntryBase> foundType = GetSdlTypeOrEnumOrUnionForField(lookupField, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
			if (!foundType){
				SendCriticalMessage(0, QString("Unable to find type %1").arg(sdlType));
				I_CRITICAL();

				return false;
			}
		}
	}

	FeedStream(stream, 2, false);

	// union class create
	stream << QStringLiteral("class ") << sdlUnion->GetName() << QStringLiteral(": public");
	stream << QStringLiteral(" std::variant<");

	// add all union types
	qsizetype countTypes = convertedTypeList.size();
	for (int i = 0; i < countTypes; ++i){
		stream << convertedTypeList[i];
		if (i < countTypes - 1){
			stream << QStringLiteral(", ");
		}
	}
	stream << QStringLiteral("> ");
	stream << QStringLiteral("{");
	FeedStream(stream, 2, false);

	stream << QStringLiteral("public:");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("typedef std::variant<");

	// add all union types
	for (int i = 0; i < countTypes; ++i){
		stream << convertedTypeList[i];
		if (i < countTypes - 1){
			stream << QStringLiteral(", ");
		}
	}
	stream << QStringLiteral("> BaseClass;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, 1);
	stream << sdlUnion->GetName() << QStringLiteral("(){};");
	FeedStream(stream, 1, false);

	for (int i = 0; i < countTypes; ++i){
		FeedStreamHorizontally(stream, 1);
		stream << sdlUnion->GetName() << QStringLiteral("(const ") << convertedTypeList[i] << QStringLiteral("& ref)");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral(": BaseClass(ref){};");
		FeedStream(stream, 2, false);
	}
	stream << QStringLiteral("};");
	FeedStream(stream, 2, false);

	// QString unionTypeObjectName = QStringLiteral("C%1Object").arg(sdlUnion->GetName());
	// stream << QStringLiteral("class %1: public ::imtbase::CItemModelBase").arg(unionTypeObjectName);
	// FeedStream(stream, 1, false);
	// FeedStreamHorizontally(stream, 0);
	// stream << QStringLiteral("{");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream);
	// stream << QStringLiteral("Q_OBJECT");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream);
	// stream << QStringLiteral("Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream);
	// stream << QStringLiteral("Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)");
	// FeedStream(stream, 2, false);

	// FeedStreamHorizontally(stream, 0);
	// stream << QStringLiteral("public:");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("typedef ::imtbase::CItemModelBase BaseClass;");
	// FeedStream(stream, 2, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("%1(QObject* parent = nullptr): BaseClass(parent) {}").arg(unionTypeObjectName);
	// FeedStream(stream, 2, false);

	// // GetType function begin
	// FeedStreamHorizontally(stream);
	// stream << QStringLiteral("Q_INVOKABLE QString GetType() const{");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 2);
	// stream << QStringLiteral("return m_type;");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream);
	// stream << QStringLiteral("}");
	// FeedStream(stream, 2, false);
	// // GetType function end

	// // SetValue function begin
	// FeedStreamHorizontally(stream);
	// stream << QStringLiteral("Q_INVOKABLE void SetValue(const QVariant& value){");
	// FeedStream(stream, 1, false);

	// QStringList unionTypeList = sdlUnion->GetTypes();
	// for (int unionIndex = 0; unionIndex < unionTypeList.count(); unionIndex++){
	// 	QString unionTypeWithNamespace = convertedTypeList[unionIndex];
	// 	QString unionType = unionTypeList[unionIndex];
	// 	FeedStreamHorizontally(stream, 2);
	// 	stream << QStringLiteral("if (value.canConvert<") << unionTypeWithNamespace;
	// 	stream << QStringLiteral(">()){");
	// 	FeedStream(stream, 1, false);

	// 	FeedStreamHorizontally(stream, 3);
	// 	stream << QStringLiteral("m_type = ") << QStringLiteral("\"") << unionType << QStringLiteral("\";");
	// 	FeedStream(stream, 1, false);

	// 	FeedStreamHorizontally(stream, 2);
	// 	stream << QStringLiteral("}");
	// 	FeedStream(stream, 2, false);
	// }

	// FeedStreamHorizontally(stream, 2);
	// stream << QStringLiteral("m_value = value;");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("}");
	// FeedStream(stream, 2, false);
	// // SetValue function end

	// // GetValue function begin
	// FeedStreamHorizontally(stream);
	// stream << QStringLiteral("Q_INVOKABLE QVariant GetValue(){");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 2);
	// stream << QStringLiteral("return QVariant();");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("}");
	// FeedStream(stream, 2, false);
	// // GetValue function end

	// FeedStreamHorizontally(stream, 0);
	// stream << QStringLiteral("signals:");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("void typeChanged();");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("void valueChanged();");
	// FeedStream(stream, 2, false);

	// FeedStreamHorizontally(stream, 0);
	// stream << QStringLiteral("public:");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("istd::TSharedNullable<std::shared_ptr<%1>> Version_1_0;").arg(sdlUnion->GetName());
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("QVariant m_value;");
	// FeedStream(stream, 1, false);

	// FeedStreamHorizontally(stream, 1);
	// stream << QStringLiteral("QString m_type;");
	// FeedStream(stream, 1, false);

	// // Union Type end
	// FeedStreamHorizontally(stream, 0);
	// stream << QStringLiteral("};");
	// FeedStream(stream, 2, false);


	return true;
}


// reimplemented (IIncludeDirectivesProvider)

QList<imtsdl::IncludeDirective> CSdlUnionGeneratorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {CreateImtDirective("<imtbase/CItemModelBase.h>")};

	return retVal;
}


} // namespace imtsdlgencpp

