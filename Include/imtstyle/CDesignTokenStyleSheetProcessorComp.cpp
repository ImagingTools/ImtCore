#include <imtstyle/CDesignTokenStyleSheetProcessorComp.h>

// Acf includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>



namespace imtstyle
{


// reimplemented (IDesignTokenProcessor)

QByteArray CDesignTokenStylesheetProcessorComp::GetHelpString() const
{
	QByteArray retval;
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	retval = helpText.toLocal8Bit().constData();
	return retval;
}


int CDesignTokenStylesheetProcessorComp::Exec()
{

	return 0;
}



} // namespace imtstyle


