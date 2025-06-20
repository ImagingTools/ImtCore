#pragma once


// ImtCore includes
#include <imtsdlgen/CSdlClassModificatorBaseComp.h>
#include <imtsdlgenv2/CObjectModificatorCompBase.h>


namespace imtsdlgenv2
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to/from  GraphQL object
	Extends generated classes by add methods, allows to save and load it from/to \c imtgql::CGqlParamObject
 */
class CSdlClassGqlModificatorComp: public CObjectModificatorCompBase
{

public:
	typedef imtsdlgenv2::CObjectModificatorCompBase BaseClass;

	I_BEGIN_COMPONENT(CSdlClassGqlModificatorComp)
	I_END_COMPONENT

public:
	// reimplemented (IIncludeDirectivesProvider)
	[[nodiscard]] virtual QList<imtsdl::IncludeDirective> GetIncludeDirectives() const override;

	// reimplemented (ICxxModifier)
	[[nodiscard]] virtual QString GetReadMethodName() const override;
	[[nodiscard]] virtual QString GetWriteMethodName() const override;
	[[nodiscard]] virtual ArgumentList GetArguments() const override;

protected:
	// reimplemented (CObjectModificatorCompBase)
	virtual QString GetContainerObjectClassName() const override;
	virtual QString GetContainerObjectVariableName() const override;
	virtual QString GetScalarArrayContainerObjectClassName() const override;
	virtual QString GetCustomArrayContainerObjectClassName() const override;
	virtual QString GetArrayContainerObjectVariableName() const override;
	virtual bool AddFieldValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint horizontalIndents) const override;
	virtual bool AddObjectValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint horizontalIndents) const override;
	virtual QString AddCreationOfTemporaryArray(QTextStream& stream, const imtsdl::CSdlField& field) const override;
	virtual bool AddFieldValueAppendToObjectArray(QTextStream& stream, const imtsdl::CSdlField& field, const QString& arrayContainerVariableName, const QString& variableName, uint horizontalIndents) const override;
	virtual bool AddArrayWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, const QString& targetObjectVariableName, uint horizontalIndents) const override;
	virtual bool AddContainerValueCheckConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected, quint16 horizontalIndents) override;
	virtual bool AddContainerValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents) override;
	virtual bool AddContainerListAccessCode(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents, ListAccessResult& result) override;
	virtual imtsdl::CSdlUnionConverter::ConversionType GetUnionScalarConversionType() const override;
	virtual imtsdl::CSdlUnionConverter::ConversionType GetUnionArrayConversionType() const override;

private:
	/// writes end for convert 'value' to 'dest' \example String() (if string) \example Integer (if long)
	[[nodiscard]] QString GetConvertEndForFieldString(const imtsdl::CSdlField& field, bool forType) const;
};


} // namespace imtsdlgenv2

