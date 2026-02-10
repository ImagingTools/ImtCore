// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// Qt includes
#include <QtCore/QMetaObject>

// Acf includes
#include <istd/IPolymorphic.h>


namespace imtsdl
{


class ISdlProcessArgumentsParser: virtual public istd::IPolymorphic
{
	Q_GADGET
public:
	static inline const QString s_sourceFileType = QStringLiteral("CPP");
	static inline const QString s_headerFileType = QStringLiteral("H");

public:
	enum GeneratorType
	{
		GT_CMAKE,
		GT_QMAKE,
		GT_CMAKE_PIPE,
		GT_DEP_FILE
	};
	Q_ENUM(GeneratorType)

	enum CppGenerationMode
	{
		CGM_FULL,
		CGM_HEADER_ONLY,
		CGM_IMPLEMENTATION_ONLY,
	};
	Q_ENUM(CppGenerationMode)

	enum TypenameWriteMode
	{
		TWM_ALWAYS,
		TWM_IF_REQUIRED,
		TWM_NEVER
	};
	Q_ENUM(TypenameWriteMode)

	virtual bool SetArguments(int argc, char** argv);
	virtual bool SetArguments(const QStringList& arguments) = 0;
	virtual QString GetSchemaFilePath() const = 0;
	virtual QString GetOutputDirectoryPath() const = 0;
	virtual QString GetNamespace() const = 0;
	virtual QString GetNamespacePrefix() const = 0;
	virtual bool IsDependenciesMode() const = 0;
	virtual bool IsGenerateMode() const = 0;
	virtual bool IsModificatorEnabled(const QString& modificatorName) const = 0;
	virtual bool IsCppEnabled() const = 0;
	virtual bool IsQmlEnabled() const = 0;
	virtual bool IsGqlEnabled() const = 0;
	virtual bool IsSchemaDependencyModeEnabled() const = 0;
	virtual QMap<QString/*class name*/, QString/*include directive*/> GetBaseClassList() const = 0;
	/// \returns paths where schemas should be loacted
	virtual QStringList GetIncludePaths() const = 0;
	virtual GeneratorType GetGeneratorType() const = 0;
	virtual QStringList GetHeadersIncludePaths() const = 0;
	virtual QString GetDepFilePath() const = 0;
	virtual CppGenerationMode GetCppGenerationMode() const = 0;
	virtual TypenameWriteMode GetTypenameWriteMode() const = 0;

	// template
	virtual bool IsTemplateEnabled() const = 0;
	virtual QString GetTemplateIncludePath() const = 0;
	virtual QString GetTemplateOutputPath() const = 0;
	virtual QString GetTemplateQmlOutputPath() const = 0;
};


inline bool ISdlProcessArgumentsParser::SetArguments(int argc, char** argv)
{
	QStringList arguments;
	for (int i = 0; i < argc; ++i){
		arguments << QString(argv[i]);
	}

	return SetArguments(arguments);
}


} // namespace imtsdl


