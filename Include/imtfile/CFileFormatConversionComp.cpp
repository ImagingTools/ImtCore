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
		int loadState = m_persistenceCompPtr->LoadFromFile(*m_workingObjectCompPtr, inputPath, progressManagerPtr);
		if (loadState == ifile::IFilePersistence::OS_OK){
			int saveState = m_persistenceCompPtr->SaveToFile(*m_workingObjectCompPtr, outputPath, progressManagerPtr);
			if (saveState == ifile::IFilePersistence::OS_OK){
				return iproc::IProcessor::TS_OK;
			}
		}
	}

	return iproc::IProcessor::TS_INVALID;
}


} // namespace imtfile


