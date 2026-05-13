// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdesign/CDesignTokenSplitProcessorComp.h>


namespace imtdesign
{


QByteArray CDesignTokenSplitProcessorComp::GetHelpString() const
{
	QString helpText = "Invalid arguments \n \t\tAnd other help text\n\n";
	QByteArray retval = helpText.toLocal8Bit().constData();

	return retval;
}


int CDesignTokenSplitProcessorComp::Exec()
{
	if(m_argumentParserCompPtr->IsSplitModeRequired())
	{
		m_designTokenFileParserCompPtr->SetFile(m_argumentParserCompPtr->GetDesignTokenFilePath());

		if (!m_designTokenFileParserCompPtr->SplitFile(m_argumentParserCompPtr->GetOutputDirectoryPath(), m_argumentParserCompPtr->GetProjectName())){
			return -1;
		}
	}

	return 0;
}


} // namespace imtdesign


