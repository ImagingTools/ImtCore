#pragma once


// ImtCore includes
#include <imtsdlgen/CSdlClassModificatorBaseComp.h>


namespace imtsdlgen
{


/**
	The C++ class modificator, adds saving and loading generated SDL types to Json
	Extends generated classes by add methods, allows to save and load it from/to \c QJsonObject
 */
class CSdlClassJsonModificatorComp: public CSdlClassModificatorBaseComp
{

public:
	typedef CSdlClassModificatorBaseComp BaseClass;

	I_BEGIN_COMPONENT(CSdlClassJsonModificatorComp)
	I_END_COMPONENT;

protected:
	// reimplemented (CSdlClassModificatorBaseComp)
	virtual bool ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType) override;
	virtual bool ProcessSourceClassFile(const imtsdl::CSdlType& sdlType) override;

private:
	void AddIncludeDirective(QTextStream& stream, bool addRemark);
	void AddFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddCustomFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddCustomFieldWriteToJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);
	void AddArrayFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddArrayFieldWriteToJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);
	void AddCustomFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddCustomFieldReadFromJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);
	void AddArrayFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddArrayFieldReadFromJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);
	void AddCustomArrayFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddCustomArrayFieldWriteToJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);
	void AddCustomArrayFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field);
	void AddCustomArrayFieldReadToJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents = 1);
	void AddJsonValueCheckAndConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected = true, quint16 hIndents = 1);
};


} // namespace imtsdlgen

