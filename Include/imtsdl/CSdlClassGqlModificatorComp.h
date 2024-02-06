#pragma once


// imtsdl includes
#include <imtsdl/CSdlClassModificatorBaseComp.h>


namespace imtsdl
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to TreeItemModel
	Extends generated classes by add methods, allows to save and load it from/to \c imtbase::CTreeItemModel
 */
class CSdlClassGqlModificatorComp: public CSdlClassModificatorBaseComp
{

public:
	typedef CSdlClassModificatorBaseComp BaseClass;

	I_BEGIN_COMPONENT(CSdlClassGqlModificatorComp)
	I_END_COMPONENT;

protected:
	// reimplemented (CSdlClassModificatorBaseComp)
	virtual bool ProcessHeaderClassFile(const CSdlType& sdlType) override;
	virtual bool ProcessSourceClassFile(const CSdlType& sdlType) override;

private:
	void AddFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);
	void AddScalarFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);
	void AddScalarListFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field) throw();
	void AddCustomFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);
	void AddCustomListFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);

	// general help methods for scalar
	void AddExtractValueFromRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	void AddSetValueToObjectCode(QTextStream& stream, const CSdlField& field);

	// general help methods for custom
	void AddExtractCustomValueFromRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddCheckCustomRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	void AddSetCustomValueToObjectCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);

	// general help methods for custom list
	/// \todo add error handling for it. do not forget about arrays and arrays.isEmpty
	void AddCheckCustomListRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);
	void AddSetCustomListValueToObjectCode(QTextStream& stream, const CSdlField& field, uint hIndents = 1);



};


} // namespace imtsdl

