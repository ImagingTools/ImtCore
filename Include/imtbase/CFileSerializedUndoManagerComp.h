// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <ifile/IFilePersistence.h>
#include <ifile/IFileNameParam.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtbase/CSerializedUndoManagerCompBase.h>


namespace imtbase
{


/**
	Implements a multi-level UNDO mechanism which stores the complete document state of each step
	in a separate file.

	This component reuses the UNDO/REDO bookkeeping of \ref CSerializedUndoManagerCompBase and stores
	every serialized document state in its own file instead of an in-memory buffer. It is intended for
	documents whose serialized state is too large to be kept in memory for many undo levels.

	\par How it works
	- Each step is serialized with the referenced \ref ifile::IFilePersistence into a file named
	  \c step_%1.bin, where \c %1 is a unique step number.
	- The directory used for saving and loading the step files is taken from the referenced
	  \ref ifile::IFileNameParam.
	- The current step (the number of available undo steps) is published to the referenced
	  \ref iprm::ISelectionParam so that an external component can track the position within the history.
	- When the redo list is cleared (a new change is made after an undo, the undo buffer overflows,
	  or the history is reset) the corresponding step files are deleted.

	\par Component Attributes
	- \b MaxBufferSize - Maximum total size of the undo step files in megabytes (default: 100 MB, inherited)

	\sa CSerializedUndoManagerCompBase, idoc::IUndoManager
*/
class CFileSerializedUndoManagerComp: public CSerializedUndoManagerCompBase
{
public:
	typedef CSerializedUndoManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFileSerializedUndoManagerComp);
		I_ASSIGN(m_filePersistenceCompPtr, "FilePersistence", "Persistence used to serialize document steps to files", true, "FilePersistence");
		I_ASSIGN(m_fileNameParamCompPtr, "StoragePath", "Directory used to save and load the step files", true, "StoragePath");
		I_ASSIGN(m_currentStepParamCompPtr, "CurrentStep", "External selection parameter storing the current step", false, "CurrentStep");
	I_END_COMPONENT;

protected:
	QString GetStorageDirectory() const;
	QString GetStepFilePath(int stepId) const;
	QString GetCompareFilePath() const;

	bool SaveObjectToFile(iser::ISerializable& object, const QString& filePath) const;
	bool LoadObjectFromFile(iser::ISerializable& object, const QString& filePath) const;
	static bool AreFilesEqual(const QString& firstFilePath, const QString& secondFilePath);

	// reimplemented (imtbase::CSerializedUndoManagerCompBase)
	virtual int WriteStep(iser::ISerializable& object) override;
	virtual bool ReadStep(int stepId, iser::ISerializable& object) const override;
	virtual bool IsStepEqualToState(int stepId, iser::ISerializable& object) const override;
	virtual void ReleaseStep(int stepId) override;
	virtual qint64 GetStepSize(int stepId) const override;
	virtual void OnCurrentStepChanged(int currentStep) override;

private:
	int m_nextStepId = 0;

	I_REF(ifile::IFilePersistence, m_filePersistenceCompPtr);
	I_REF(ifile::IFileNameParam, m_fileNameParamCompPtr);
	I_REF(iprm::ISelectionParam, m_currentStepParamCompPtr);
};


} // namespace imtbase
