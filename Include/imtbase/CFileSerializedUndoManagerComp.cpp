// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CFileSerializedUndoManagerComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>


namespace imtbase
{


QString CFileSerializedUndoManagerComp::GetStorageDirectory() const
{
	if (m_fileNameParamCompPtr.IsValid()){
		return m_fileNameParamCompPtr->GetPath();
	}

	return QString();
}


QString CFileSerializedUndoManagerComp::GetStepFilePath(int stepId) const
{
	QString directory = GetStorageDirectory();
	if (directory.isEmpty() || (stepId < 0)){
		return QString();
	}

	return QDir(directory).absoluteFilePath(QString("step_%1.bin").arg(stepId));
}


QString CFileSerializedUndoManagerComp::GetCompareFilePath() const
{
	QString directory = GetStorageDirectory();
	if (directory.isEmpty()){
		return QString();
	}

	return QDir(directory).absoluteFilePath("step_compare.bin");
}


bool CFileSerializedUndoManagerComp::SaveObjectToFile(iser::ISerializable& object, const QString& filePath) const
{
	if (filePath.isEmpty() || !m_filePersistenceCompPtr.IsValid()){
		return false;
	}

	QDir().mkpath(QFileInfo(filePath).absolutePath());

	return m_filePersistenceCompPtr->SaveToFile(object, filePath) == ifile::IFilePersistence::OS_OK;
}


bool CFileSerializedUndoManagerComp::LoadObjectFromFile(iser::ISerializable& object, const QString& filePath) const
{
	if (filePath.isEmpty() || !m_filePersistenceCompPtr.IsValid()){
		return false;
	}

	return m_filePersistenceCompPtr->LoadFromFile(object, filePath) == ifile::IFilePersistence::OS_OK;
}


bool CFileSerializedUndoManagerComp::AreFilesEqual(const QString& firstFilePath, const QString& secondFilePath)
{
	QFile firstFile(firstFilePath);
	QFile secondFile(secondFilePath);

	if (!firstFile.open(QIODevice::ReadOnly) || !secondFile.open(QIODevice::ReadOnly)){
		return false;
	}

	if (firstFile.size() != secondFile.size()){
		return false;
	}

	static const qint64 blockSize = 64 * 1024;
	while (!firstFile.atEnd()){
		if (firstFile.read(blockSize) != secondFile.read(blockSize)){
			return false;
		}
	}

	return true;
}


// reimplemented (imtbase::CSerializedUndoManagerCompBase)

int CFileSerializedUndoManagerComp::WriteStep(iser::ISerializable& object)
{
	int stepId = m_nextStepId;

	QString filePath = GetStepFilePath(stepId);
	if (filePath.isEmpty()){
		return -1;
	}

	if (!SaveObjectToFile(object, filePath)){
		QFile::remove(filePath);

		return -1;
	}

	++m_nextStepId;

	return stepId;
}


bool CFileSerializedUndoManagerComp::ReadStep(int stepId, iser::ISerializable& object) const
{
	return LoadObjectFromFile(object, GetStepFilePath(stepId));
}


bool CFileSerializedUndoManagerComp::IsStepEqualToState(int stepId, iser::ISerializable& object) const
{
	QString storedFilePath = GetStepFilePath(stepId);
	QString compareFilePath = GetCompareFilePath();

	if (storedFilePath.isEmpty() || compareFilePath.isEmpty()){
		return false;
	}

	bool result = false;
	if (SaveObjectToFile(object, compareFilePath)){
		result = AreFilesEqual(storedFilePath, compareFilePath);
	}

	QFile::remove(compareFilePath);

	return result;
}


void CFileSerializedUndoManagerComp::ReleaseStep(int stepId)
{
	QString filePath = GetStepFilePath(stepId);
	if (!filePath.isEmpty()){
		QFile::remove(filePath);
	}
}


qint64 CFileSerializedUndoManagerComp::GetStepSize(int stepId) const
{
	QString filePath = GetStepFilePath(stepId);
	if (filePath.isEmpty()){
		return 0;
	}

	return QFileInfo(filePath).size();
}


void CFileSerializedUndoManagerComp::OnCurrentStepChanged(int currentStep)
{
	if (m_currentStepParamCompPtr.IsValid()){
		m_currentStepParamCompPtr->SetSelectedOptionIndex(currentStep);
	}
}


} // namespace imtbase
