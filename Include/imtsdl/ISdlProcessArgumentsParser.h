#pragma once


//Acf includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


class ISdlProcessArgumentsParser: virtual public istd::IPolymorphic
{
public:
	static inline const QString s_sourceFileType = QStringLiteral("CPP");
	static inline const QString s_headerFileType = QStringLiteral("H");

public:
	enum GeneratorType
	{
		GT_CMAKE,
		GT_QMAKE,
		GT_CMAKE_PIPE
	};

	enum AutoLinkLevel
	{
		ALL_NONE = 0,					///< Dasabled. ALL files and deps will be compiled
		ALL_SAME_NAMESPACE = 1,			///< Compile only files with same namespace
		ALL_ONLY_FILE = 2,				///< Compile only input file
	};

	virtual bool SetArguments(int argc, char** argv) = 0;
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
	virtual QMap<QString/*File extention*/, QString/*Destination path*/> GetJoinRules() const = 0;
	virtual QStringList GetIncludePaths() const = 0;
	virtual GeneratorType GetGeneratorType() const = 0;
	virtual bool IsAutoJoinEnabled() const = 0;
	virtual AutoLinkLevel GetAutoLinkLevel() const = 0;
	virtual QStringList GetHeadersIncludePaths() const = 0;
	virtual QString GetCachePath() = 0;
	virtual QStringList GetAdditionalCachePaths() = 0;
};


} // namespace imtsdl


