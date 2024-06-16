#pragma once


//Acf includes
#include <istd/IPolymorphic.h>

// imtsdl includes
#include <imtsdl/imtsdl.h>


namespace imtsdl
{


class ISdlProcessArgumentsParser: virtual public istd::IPolymorphic
{
public:
	virtual bool SetArguments(int argc, char** argv) = 0;
	virtual QString GetSchemaFilePath() const = 0;
	virtual QString GetOutputDirectoryPath() const = 0;
	virtual QString GetNamespace() const = 0;
	virtual bool IsDependenciesMode() const = 0;
	virtual bool IsGenerateMode() const = 0;
	virtual bool IsModificatorEnabled(const QString& modificatorName) const = 0;
	virtual bool IsCppEnabled() const = 0;
	virtual bool IsQmlEnabled() const = 0;
	virtual bool IsGqlEnabled() const = 0;
	virtual QMap<QString/*class name*/, QString/*include directive*/> GetBaseClassList() const = 0;
};


} // namespace imtsdl


