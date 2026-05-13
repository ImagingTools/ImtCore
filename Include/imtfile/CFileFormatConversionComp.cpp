// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CFileFormatConversionComp.h>


namespace imtfile
{


int CFileFormatConversionComp::ConvertFiles(
			const QString& inputPath,
			QString& outputPath,
			const iprm::IParamsSet* /*paramsPtr*/,
			ibase::IProgressManager* progressManagerPtr) const
{
	if (m_persistenceCompPtr.IsValid() && m_workingObjectCompPtr.IsValid()){
		if (m_workingObjectCompPtr->ResetData()){
			int loadState = m_persistenceCompPtr->LoadFromFile(*m_workingObjectCompPtr, inputPath, progressManagerPtr);
			if (loadState == ifile::IFilePersistence::OS_OK){
				int saveState = m_persistenceCompPtr->SaveToFile(*m_workingObjectCompPtr, outputPath, progressManagerPtr);
				if (saveState == ifile::IFilePersistence::OS_OK){
					return iproc::IProcessor::TS_OK;
				}
			}
		}
	}

	return iproc::IProcessor::TS_INVALID;
}


} // namespace imtfile


