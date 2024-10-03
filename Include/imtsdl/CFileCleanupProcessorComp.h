#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iproc/TSyncProcessorCompBase.h>


namespace imtsdl
{


/**
	Simple file joiner. Joins text files into a single
*/
class CFileCleanupProcessorComp:
			virtual public iproc::CSyncProcessorCompBase
{
public:
	static const QByteArray s_sourceFilePathParamId;
	static const QByteArray s_targetFilePathParamId;

	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CFileCleanupProcessorComp)
	I_END_COMPONENT

	// reimplemented (iproc::IProcessor)

	/**
		\brief Cleanups files by removing duplicates of lines
		\param paramsPtr - (required)
				- (required) [\c s_sourceFilePathParamId]	\c ifile::IFileNameParam	- original file to be cleaned up
				- (oprional) [\c s_targetFilePathParamId]	\c ifile::IFileNameParam	- file path where new file will be created \warning it not set, the original file will be overrided
		\param inputPtr (required)
				- list of regukar expressions for removing lines. as \c iprm::IOptionsList
					- [id] - unused
					- [name] - Rerular expression list
		\param outputPtr - unused
		\param progressManagerPtr - unused
	*/
	virtual int DoProcessing(
				const iprm::IParamsSet* paramsPtr,
				const istd::IPolymorphic* inputPtr,
				istd::IChangeable* outputPtr,
				ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	static const QString s_filePathVariable;
};


} // namespace imtsdl


