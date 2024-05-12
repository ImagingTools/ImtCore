#pragma once


// imtsdl includes
#include <imtsdl/CSdlClassModificatorBaseComp.h>


namespace imtsdl
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
	virtual bool ProcessHeaderClassFile(const CSdlType& sdlType) override;
	virtual bool ProcessSourceClassFile(const CSdlType& sdlType) override;

private:
	void AddIncludeDerective(QTextStream& stream, bool addRemark);
	void AddFieldWriteToJsonCode(QTextStream& stream, const CSdlField& field);
	void AddFieldReadFromJsonCode(QTextStream& stream, const CSdlField& field);
	void AddCustomFieldWriteToJsonCode(QTextStream& stream, const CSdlField& field);
	void AddCustomFieldWriteToJsonImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);
	void AddArrayFieldWriteToJsonCode(QTextStream& stream, const CSdlField& field);
	void AddArrayFieldWriteToJsonImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);
	void AddCustomFieldReadFromJsonCode(QTextStream& stream, const CSdlField& field);
	void AddCustomFieldReadFromJsonImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);
	void AddArrayFieldReadFromJsonCode(QTextStream& stream, const CSdlField& field);
	void AddArrayFieldReadFromJsonImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);
	void AddCustomArrayFieldWriteToJsonCode(QTextStream& stream, const CSdlField& field);
	void AddCustomArrayFieldWriteToJsonImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);
	void AddCustomArrayFieldReadFromJsonCode(QTextStream& stream, const CSdlField& field);
	void AddCustomArrayFieldReadToJsonImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents = 1);
	void AddJsonValueCheckAndConditionBegin(QTextStream& stream, const CSdlField& field, bool expected = true, quint16 hIndents = 1);
};


} // namespace imtsdl

