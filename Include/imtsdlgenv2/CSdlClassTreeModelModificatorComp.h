#pragma once


// ImtCore includes
#include <imtsdlgen/CSdlClassModificatorBaseComp.h>
#include <imtsdlgenv2/CSdlGenTools.h>


namespace imtsdlgenv2
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to TreeItemModel
	Extends generated classes by add methods, allows to save and load it from/to \c imtbase::CTreeItemModel
 */
class CSdlClassTreeModelModificatorComp: public imtsdlgen::CSdlClassModificatorBaseComp, protected CSdlGenTools
{

public:
	typedef imtsdlgen::CSdlClassModificatorBaseComp BaseClass;

	I_BEGIN_COMPONENT(CSdlClassTreeModelModificatorComp)
	I_END_COMPONENT;

protected:
	// reimplemented (CSdlClassModificatorBaseComp)
	virtual bool ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType) override;
	virtual bool ProcessSourceClassFile(const imtsdl::CSdlType& sdlType) override;

private:
	/// \todo add error handling for it
	void AddFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddCustomFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddCustomFieldWriteToModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, quint16 hIndents = 1);
	void AddCustomFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddCustomFieldReadFromModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, quint16 hIndents = 1);
	void AddPrimitiveArrayFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddPrimitiveArrayFieldWriteToModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, quint16 hIndents = 1);
	void AddPrimitiveArrayFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddPrimitiveArrayFieldReadFromModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);
	void AddCustomArrayFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddCustomArrayFieldWriteToModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, quint16 hIndents = 1);
	void AddCustomArrayFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType);
	void AddCustomArrayFieldReadFromModelImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);

private:
};


} // namespace imtsdlgenv2

