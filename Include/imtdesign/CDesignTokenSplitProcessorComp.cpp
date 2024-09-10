#include <imtdesign/CDesignTokenSplitProcessorComp.h>

// ACF includes
#include <iprm/IParamsSet.h>
#include <iprm/IEnableableParam.h>
#include <iprm/IIdParam.h>
#include <ibase/IApplication.h>
#include <ifile/IFileNameParam.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>



namespace imtdesign
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
	if(m_argumentParserCompPtr->IsSplitModeRequired())
	{
		m_designTokenFileParserCompPtr->SetFile(m_argumentParserCompPtr->GetDesignTokenFilePath());
		return bool(int(!m_designTokenFileParserCompPtr->SplitFile(m_argumentParserCompPtr->GetOutputDirectoryPath(), m_argumentParserCompPtr->GetProjectName())));
	}

	return 0;
}


} // namespace imtdesign


