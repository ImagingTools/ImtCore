// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtsdlgencpp/CSdlClassModificatorBaseComp.h>
#include <imtsdlgencpp/CSdlGenTools.h>
#include <imtsdlgencpp/CSdlUnionConverter.h>
#include <imtsdl/CSdlEnumConverter.h>


namespace imtsdlgencpp
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to TreeItemModel
	Extends generated classes by add methods, allows to save and load it from/to \c imtbase::CTreeItemModel
 */
class CSdlClassTreeModelModificatorComp:
			public CSdlClassModificatorBaseComp,
			protected CSdlGenTools,
			protected imtsdl::CSdlTools,
			protected imtsdl::CSdlEnumConverter,
			protected CSdlUnionConverter
{
	using CSdlGenTools::AddArrayInternalChecksFail;

public:
	typedef CSdlClassModificatorBaseComp BaseClass;

	I_BEGIN_COMPONENT(CSdlClassTreeModelModificatorComp)
	I_END_COMPONENT;

protected:
	// reimplemented (IIncludeDirectivesProvider)
	[[nodiscard]] virtual QList<imtsdl::IncludeDirective> GetIncludeDirectives() const override;

	// reimplemented (ICxxModifier)
	[[nodiscard]] virtual QString GetReadMethodName() const override;
	[[nodiscard]] virtual QString GetWriteMethodName() const override;
	[[nodiscard]] virtual ArgumentList GetArguments() const override;

	// reimplemented (CSdlClassModificatorBaseComp)
	virtual bool ProcessSourceClassFile(const imtsdl::CSdlType& sdlType, QIODevice* sourceDevicePtr, const iprm::IParamsSet* paramsPtr) const override;
	virtual bool ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType, QIODevice* headerDevicePtr, const iprm::IParamsSet* paramsPtr) const override;

private:
	/// \todo add error handling for it
	void AddFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional) const;
	void AddFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional) const;
	void AddCustomFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional) const;
	void AddCustomFieldWriteToModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional, quint16 hIndents = 1) const;
	void AddCustomFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional) const;
	void AddCustomFieldReadFromModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional, quint16 hIndents = 1) const;
	void AddPrimitiveArrayFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool isEnum, bool isUnion, bool optional) const;
	void AddPrimitiveArrayFieldWriteToModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool isEnum, bool isUnion, bool optional, quint16 hIndents = 1) const;
	void AddPrimitiveArrayFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool isEnum, bool isUnion, bool optional) const;
	void AddPrimitiveArrayFieldReadFromModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool isEnum, bool isUnion, bool optional, quint16 hIndents = 1) const;
	void AddCustomArrayFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional) const;
	void AddCustomArrayFieldWriteToModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional, quint16 hIndents = 1) const;
	void AddCustomArrayFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional) const;
	void AddCustomArrayFieldReadFromModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, quint16 hIndents = 1) const;
};


} // namespace imtsdlgencpp

