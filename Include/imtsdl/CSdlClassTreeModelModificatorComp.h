#pragma once


// ImtCore includes
#include <imtsdl/CSdlClassModificatorBaseComp.h>


namespace imtsdl
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to TreeItemModel
	Extends generated classes by add methods, allows to save and load it from/to \c imtbase::CTreeItemModel
 */
class CSdlClassTreeModelModificatorComp: public CSdlClassModificatorBaseComp
{

public:
	typedef CSdlClassModificatorBaseComp BaseClass;

	I_BEGIN_COMPONENT(CSdlClassTreeModelModificatorComp)
	I_END_COMPONENT;

protected:
	// reimplemented (CSdlClassModificatorBaseComp)
	virtual bool ProcessHeaderClassFile(const CSdlType& sdlType) override;
	virtual bool ProcessSourceClassFile(const CSdlType& sdlType) override;

private:
	/// \todo add error handling for it
	void AddFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddCustomFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddCustomFieldWriteToModelImplCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType, quint16 hIndents = 1);
	void AddCustomFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddCustomFieldReadFromModelImplCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType, quint16 hIndents = 1);
	void AddPrimitiveArrayFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddPrimitiveArrayFieldWriteToModelImplCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType, quint16 hIndents = 1);
	void AddPrimitiveArrayFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddPrimitiveArrayFieldReadFromModelImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);
	void AddCustomArrayFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddCustomArrayFieldWriteToModelImplCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType, quint16 hIndents = 1);
	void AddCustomArrayFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType);
	void AddCustomArrayFieldReadFromModelImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);

private:
};


} // namespace imtsdl

