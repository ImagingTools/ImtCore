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
	/// \todo add error handling for it
	void AddFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field);
	/// \todo add complex and array values processing
};


} // namespace imtsdl

