// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtsdlgencpp/CSdlClassModificatorBaseComp.h>
#include <imtsdlgencpp/CObjectModificatorCompBase.h>


namespace imtsdlgencpp
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to Json
	Extends generated classes by add methods, allows to save and load it from/to \c QJsonObject
 */
class CSdlClassJsonModificatorComp: public CObjectModificatorCompBase
{

public:
	typedef CObjectModificatorCompBase BaseClass;

	I_BEGIN_COMPONENT(CSdlClassJsonModificatorComp)
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
	virtual bool AddContainerValueCheckConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected, quint16 horizontalIndents) const override;
	virtual bool AddContainerValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents) const override;
	virtual bool AddContainerListAccessCode(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents, ListAccessResult& result) const override;
	virtual CSdlUnionConverter::ConversionType GetUnionScalarConversionType() const override;
	virtual CSdlUnionConverter::ConversionType GetUnionArrayConversionType() const override;
	virtual void AddUnionFieldValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents) const override;
	virtual void AddUnionFieldValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents) const override;
	virtual void WriteTypenameToObjectCode(QTextStream& stream, const imtsdl::CSdlType& sdlType) const override;

private:
	/// writes end for convert 'value' to 'dest' \example String() (if string) \example Integer (if long)
	[[nodiscard]] QString GetConvertEndForFieldString(const imtsdl::CSdlField& field, bool forType) const;
};


} // namespace imtsdlgencpp

