#include <imtstyle/CDesignTokenSplitProcessorComp.h>

// Acf includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>



namespace imtstyle
{


QByteArray CDesignTokenSplitProcessorComp::GetHelpString() const
{
	QByteArray retval;
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	retval = helpText.toLocal8Bit().constData();
	return retval;
}


int CDesignTokenSplitProcessorComp::Exec()
{
	if(m_argumentParserAttrPtr->IsSplitModeRequired())
	{
		m_designTokenFileParserAttrPtr->SetFile(m_argumentParserAttrPtr->GetDesignTokenFilePath());
		return bool(int(!m_designTokenFileParserAttrPtr->SplitFile(m_argumentParserAttrPtr->GetOutputDirectoryPath(), m_argumentParserAttrPtr->GetProjectName())));
	}

	return 0;
}


} // namespace imtstyle


