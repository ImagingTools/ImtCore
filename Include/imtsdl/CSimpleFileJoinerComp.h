// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ITextParam.h>
#include <iproc/TSyncProcessorCompBase.h>


namespace imtsdl
{


/**
	Simple file joiner. Joins text files into a single
*/
class CSimpleFileJoinerComp:
			virtual public iproc::CSyncProcessorCompBase
{
public:
	/// \todo move it to \file imtsdl.h
	static const QByteArray s_sourceDirPathParamId;
	static const QByteArray s_targetFilePathParamId;
	static const QByteArray s_appendModeParamId;

	typedef iproc::CSyncProcessorCompBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleFileJoinerComp)
		I_ASSIGN(m_emptyStringsAtEndAttrPtr, "EmptyStringsAtEnd", "Defines how many lines should be added at end of a file's part", true, 3);
		I_ASSIGN(m_filePartSeparatorTextCompPtr, "FilePartSeparatorText", "Defines a text at begin of each file.\n$(FilePath) variable is supported - will be replacet to file name.", false, "FilePartSeparatorText");
	I_END_COMPONENT

	// reimplemented (iproc::IProcessor)

	/**
		\brief Joins text files into a single
		\param paramsPtr - (required)
				- (required) [\c s_sourceDirPathParamId]	\c ifile::IFileNameParam	- dir fo files search
				- (required) [\c s_targetFilePathParamId]	\c ifile::IFileNameParam	- file path where file will be created
				- (optional) [\c s_appendModeParamId]		\c iprm::IEnableableParam	- if enabled file data will be appended
		\param inputPtr (required)
				- list of file filters as \c iprm::IOptionsList \note order will be preserved
					- [id] - unused
					- [name] - file path related to the source dir
		\param outputPtr - unused
		\param progressManagerPtr - unused
	*/
	virtual TaskState DoProcessing(
		const iprm::IParamsSet* paramsPtr,
		const istd::IPolymorphic* inputPtr,
		istd::IChangeable* outputPtr,
		ibase::IProgressManager* progressManagerPtr = NULL) override;

private:
	static const QString s_filePathVariable;

private:
	I_ATTR(int, m_emptyStringsAtEndAttrPtr);
	I_REF(iprm::ITextParam, m_filePartSeparatorTextCompPtr);
};


} // namespace imtsdl


