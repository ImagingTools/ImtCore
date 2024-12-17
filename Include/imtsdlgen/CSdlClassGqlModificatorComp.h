#pragma once


// ImtCore includes
#include <imtsdlgen/CSdlClassModificatorBaseComp.h>


namespace imtsdlgen
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to/from  GraphQL object
	Extends generated classes by add methods, allows to save and load it from/to \c imtgql::CGqlObject
 */
class CSdlClassGqlModificatorComp: public CSdlClassModificatorBaseComp
{
public:
	typedef CSdlClassModificatorBaseComp BaseClass;

	I_BEGIN_COMPONENT(CSdlClassGqlModificatorComp)
	I_END_COMPONENT;

protected:
	// reimplemented (IIncludeDirectivesProvider)
	[[nodiscard]] virtual QList<imtsdl::IncludeDirective> GetIncludeDirectives() const override;

	// reimplemented (CSdlClassModificatorBaseComp)
	virtual bool ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType) override;
	virtual bool ProcessSourceClassFile(const imtsdl::CSdlType& sdlType) override;

private:
	// read methods
	void AddFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional);
	void AddScalarFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional);
	void AddScalarListFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional);
	void AddCustomFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional);
	void AddCustomListFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional);

	// write methods
	void AddFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional);
	void AddScalarFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents);
	void AddScalarListFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents);
	void AddCustomFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents);
	void AddCustomListFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents);

	// general help methods for scalar
	void AddExtractValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddDataCheckRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddSetValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field);

	// general help methods for custom
	void AddExtractCustomValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	/// \todo add error handling for it.
	void AddCheckCustomRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddSetCustomValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents = 1);

	// general help methods for custom list
	/// \todo add error handling for it. do not forget about arrays and arrays.isEmpty
	void AddCheckCustomListRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddSetCustomListValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents = 1);

	// general help methods for scalar list
	/// \todo add error handling for it. do not forget about arrays and arrays.isEmpty
	void AddCheckScalarListRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
	void AddSetScalarListValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents = 1);
};


} // namespace imtsdlgen


