#include <imtrepo/CFileTransformationControllerComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

// ACF includes
#include <ifile/CFileListProviderComp.h>
#include <istd/CSystem.h>

// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>


namespace imtrepo
{


// public methods

// reimplemented (IRepositoryTransformationController)

bool CFileTransformationControllerComp::TransformRepository(int fromRevision, int toRevision) const
{
	if (!m_repositoryItemInfoProviderCompPtr.IsValid() || !m_repositoryCompPtr.IsValid()){
		return false;
	}

	bool isOk;
	TransformationState transformationState = GetTransformationState(isOk);

	if (transformationState == TS_REPLACING){
		if (ApplyNewRevision()){
			if (SetTransformationState(TS_IDLE)){
				return true;
			}

			return false;
		}

		return false;
	}

	if (!CleanupTrasformation()){
		return false;
	}

	if (fromRevision == toRevision){
		return true;
	}

	istd::CIntRange range(fromRevision, toRevision);
	range.Validate();

	istd::CIntRange availableRange(0, 0);

	for (int i = 0; i < m_transformationsCompPtr.GetCount(); ++i){
		const IRepositoryFileTransformaton* transformationPtr = m_transformationsCompPtr[i];
		if (transformationPtr != nullptr){
			istd::CIntRange transformationRange = transformationPtr->GetSupportedRevisionRange();

			availableRange.Unite(transformationRange);
		}
	}

	if (!availableRange.Contains(range)){
		return false;
	}

	bool isUpgrade = (fromRevision < toRevision);

	struct TransformationStep
	{
		int from = -1;
		int to = -1;
		const IRepositoryFileTransformaton* transformationPtr = nullptr;
	};

	QVector<TransformationStep> transformations;

	for (int i = 0; i < m_transformationsCompPtr.GetCount(); ++i){
		const IRepositoryFileTransformaton* transformationPtr = m_transformationsCompPtr[i];

		if (transformationPtr != nullptr){
			istd::CIntRange transformationRange = transformationPtr->GetSupportedRevisionRange();

			int startRevision = range.GetMinValue();
			int stopRevision = range.GetMaxValue();

			while (startRevision != stopRevision){
				istd::CIntRange checkRange(startRevision, stopRevision);

				if (transformationRange.Contains(checkRange)){
					TransformationStep step;
					step.from = isUpgrade ? checkRange.GetMinValue() : checkRange.GetMaxValue();
					step.to = isUpgrade ? checkRange.GetMaxValue() : checkRange.GetMinValue();
					step.transformationPtr = transformationPtr;

					if (isUpgrade){
						transformations.push_back(step);
					}
					else{
						transformations.push_front(step);
					}

					range.SetMinValue(checkRange.GetMaxValue());

					break;
				}

				stopRevision--;
			}
		}
	}

	if (SetTransformationState(TS_IN_PROGRESS)){
		return false;
	}

	bool isFailed = false;

	imtbase::ICollectionInfo::Ids repositoryItemIds = m_repositoryItemInfoProviderCompPtr->GetRepositoryItems().GetElementIds();

	for (int itemIndex = 0; itemIndex < repositoryItemIds.count() && !isFailed; itemIndex++){
		QFileInfo itemInfo;

		const IRepositoryItemInfo* repositoryItemInfoPtr = m_repositoryItemInfoProviderCompPtr->GetRepositoryItemInfo(repositoryItemIds[itemIndex]);

		if (repositoryItemInfoPtr != nullptr){
			IRepositoryItemInfo::RepositoryFileTypes fileIds = repositoryItemInfoPtr->GetRepositoryItemFileTypes();

			RepositoryItemInfo repositoryItemInfo;

			for (IRepositoryItemInfo::RepositoryFileType fileId : fileIds){
				QString filePath = repositoryItemInfoPtr->GetRepositoryItemFilePath(fileId);
				if (!filePath.isEmpty()){
					if (!istd::CSystem::FileCopy(filePath, filePath + ".new", true)){
						isFailed = true;
						break;
					}

					repositoryItemInfo.m_files[fileId] = filePath + ".new";

					if (fileId == IRepositoryItemInfo::RFT_INFO){
						itemInfo = filePath;
					}
				}
			}


			for (int stepIndex = 0; stepIndex < transformations.count() && !isFailed; stepIndex++){
				const TransformationStep& step = transformations[stepIndex];

				if (!step.transformationPtr->TransformFile(repositoryItemInfo, step.from, step.to)){
					SendErrorMessage(0, QString("Repository file '%1' could not be transformed").arg(itemInfo.absoluteFilePath()));

					isFailed = true;
					break;
				}
			}
		}
		else{
			isFailed = true;
			break;
		}
	}

	if (isFailed){
		CleanupTrasformation();
		SetTransformationState(TS_IDLE);

		return false;
	}

	if (SetTransformationState(TS_REPLACING)){
		return false;
	}

	if (ApplyNewRevision()){
		CleanupTrasformation();
		if (SetTransformationState(TS_IDLE)){
			return true;
		}
	}

	return false;
}


// protected methods

CFileTransformationControllerComp::TransformationState CFileTransformationControllerComp::GetTransformationState(bool &isOk) const
{
	QString rootFolder = m_repositoryCompPtr->GetCollectionRootFolder();
	QFile stateFile(rootFolder + "/TransformationState");
	QTextStream textStream(&stateFile);

	if (stateFile.exists()){
		if (stateFile.open(QIODevice::Text | QIODevice::ReadOnly)){
			int state;

			textStream >> state;
			stateFile.close();

			isOk = true;
			return (TransformationState)state;
		}
	}

	return TS_IDLE;
	isOk = false;
}


bool CFileTransformationControllerComp::SetTransformationState(TransformationState state) const
{
	QString rootFolder = m_repositoryCompPtr->GetCollectionRootFolder();
	QFile stateFile(rootFolder + "/TransformationState");
	QTextStream textStream(&stateFile);

	if (stateFile.exists()){
		if (!stateFile.remove()){
			return false;
		}
	}

	if (stateFile.open(QIODevice::Text | QIODevice::WriteOnly)){
		textStream << (int)state;
		stateFile.close();

		return true;
	}

	return false;
}


bool CFileTransformationControllerComp::ApplyNewRevision() const
{
	QString rootFolder = m_repositoryCompPtr->GetCollectionRootFolder();

	QFileInfoList fileList;
	ifile::CFileListProviderComp::CreateFileList(rootFolder, 0, 1, {"*.new"}, QDir::NoSort, fileList);

	for (const QFileInfo& file : fileList){
		QString filePath = file.filePath();
		QString newFilePath = filePath.chopped(4);
		if (!istd::CSystem::FileMove(filePath, newFilePath, true)){
			return false;
		}
	}

	return true;
}


bool CFileTransformationControllerComp::CleanupTrasformation() const
{
	bool retVal = true;

	imtbase::ICollectionInfo::Ids itemIds = m_repositoryItemInfoProviderCompPtr->GetRepositoryItems().GetElementIds();

	for (const QByteArray& itemId : itemIds){
		const IRepositoryItemInfo* itemInfoPtr = m_repositoryItemInfoProviderCompPtr->GetRepositoryItemInfo(itemId);
		if (itemInfoPtr != nullptr){
			IRepositoryItemInfo::RepositoryFileTypes fileIds = itemInfoPtr->GetRepositoryItemFileTypes();
			for (IRepositoryItemInfo::RepositoryFileType fileId : fileIds){
				QString filePath = itemInfoPtr->GetRepositoryItemFilePath(fileId) + ".new";
				QFile file(filePath);

				if (file.exists()){
					if (!file.remove()){
						retVal = false;
					}
				}
			}
		}
		else{
			retVal = false;
		}
	}

	return retVal;
}


// public methods of the embedded class RepositoryItemInfo

// reimplemented (IRepositoryItemInfo)

IRepositoryItemInfo::RepositoryFileTypes CFileTransformationControllerComp::RepositoryItemInfo::GetRepositoryItemFileTypes() const
{
	IRepositoryItemInfo::RepositoryFileTypes types;

	for (int type : m_files.keys()){
		types.insert((RepositoryFileType)type);
	}

	return types;
}


QString CFileTransformationControllerComp::RepositoryItemInfo::GetRepositoryItemFilePath(RepositoryFileType fileId) const
{
	if (m_files.contains(fileId)){
		return m_files[fileId];
	}

	return QString();
}


} // namespace imtrepo


