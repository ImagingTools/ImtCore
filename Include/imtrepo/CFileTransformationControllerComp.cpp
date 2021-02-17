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

// reimplemented (IRepositoryFileTransformationStepsProvider)

IRepositoryFileTransformationStepsProvider::TransformationSteps CFileTransformationControllerComp::GetTransformationSteps(int fromRevision, int toRevision) const
{
	TransformationSteps steps;

	istd::CIntRange range(fromRevision, toRevision);
	range.Validate();

	istd::CIntRange availableRange(0, 0);

	for (int i = 0; i < m_transformationsCompPtr.GetCount(); ++i){
		const IRepositoryFileTransformation* transformationPtr = m_transformationsCompPtr[i];
		if (transformationPtr != nullptr){
			istd::CIntRange transformationRange = transformationPtr->GetSupportedRevisionRange();

			availableRange.Unite(transformationRange);
		}
	}

	if (!availableRange.Contains(range)){
		return steps;
	}

	bool isUpgrade = (fromRevision < toRevision);

	for (int i = 0; i < m_transformationsCompPtr.GetCount(); ++i){
		const IRepositoryFileTransformation* transformationPtr = m_transformationsCompPtr[i];

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
						steps.push_back(step);
					}
					else{
						steps.push_front(step);
					}

					range.SetMinValue(checkRange.GetMaxValue());

					break;
				}

				stopRevision--;
			}
		}
	}

	if (steps.first().from != fromRevision || steps.last().to != toRevision){
		steps.clear();
	}

	return steps;
}


// reimplemented (IRepositoryTransformationController)

bool CFileTransformationControllerComp::TransformRepository(IFileObjectCollection& repository, int fromRevision, int toRevision) const
{
	if (fromRevision == toRevision){
		return true;
	}

	IRepositoryItemInfoProvider* itemInfoProviderPtr = dynamic_cast<IRepositoryItemInfoProvider*>(&repository);
	if (itemInfoProviderPtr == nullptr){
		return false;
	}

	imtbase::ICollectionInfo::Ids repositoryItemIds = GetRepositoryItemIds(repository);
	if (repositoryItemIds.isEmpty()){
		return true;
	}

	bool isOk;
	TransformationState transformationState = GetTransformationState(repository, isOk);

	if (transformationState == TS_REPLACING){
		if (ReplaceWithTransformedItems(repository)){
			if (SetTransformationState(repository, TS_IDLE)){
				return true;
			}

			return false;
		}

		return false;
	}

	if (!CleanupTrasformation(repository)){
		return false;
	}

	TransformationSteps transformations = GetTransformationSteps(fromRevision, toRevision);

	if (transformations.isEmpty()){
		return false;
	}

	if (transformations.first().from != fromRevision || transformations.last().to != toRevision){
		return false;
	}

	if (!SetTransformationState(repository, TS_IN_PROGRESS)){
		return false;
	}

	bool isFailed = false;

	for (int itemIndex = 0; itemIndex < repositoryItemIds.count() && !isFailed; itemIndex++){
		QFileInfo itemInfo;

		const IRepositoryItemInfo* repositoryItemInfoPtr = GetRepositoryItemInfo(repository, repositoryItemIds[itemIndex]);

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
		CleanupTrasformation(repository);
		SetTransformationState(repository, TS_IDLE);

		return false;
	}

	if (!SetTransformationState(repository, TS_REPLACING)){
		return false;
	}

	if (ReplaceWithTransformedItems(repository)){
		CleanupTrasformation(repository);
		if (SetTransformationState(repository, TS_IDLE)){
			return true;
		}
	}

	return false;
}


// protected methods

imtbase::ICollectionInfo::Ids CFileTransformationControllerComp::GetRepositoryItemIds(IFileObjectCollection& repository) const
{
	IRepositoryItemInfoProvider* itemInfoProviderPtr = dynamic_cast<IRepositoryItemInfoProvider*>(&repository);
	if (itemInfoProviderPtr != nullptr){
		return itemInfoProviderPtr->GetRepositoryItems().GetElementIds();
	}

	return imtbase::ICollectionInfo::Ids();
}


const IRepositoryItemInfo* CFileTransformationControllerComp::GetRepositoryItemInfo(IFileObjectCollection& repository, const QByteArray& itemId) const
{
	IRepositoryItemInfoProvider* itemInfoProviderPtr = dynamic_cast<IRepositoryItemInfoProvider*>(&repository);
	if (itemInfoProviderPtr != nullptr){
		return itemInfoProviderPtr->GetRepositoryItemInfo(itemId);
	}

	return nullptr;
}


CFileTransformationControllerComp::TransformationState CFileTransformationControllerComp::GetTransformationState(IFileObjectCollection& repository, bool &isOk) const
{
	QFile stateFile(repository.GetCollectionRootFolder() + "/TransformationState");
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


bool CFileTransformationControllerComp::SetTransformationState(IFileObjectCollection& repository, TransformationState state) const
{
	QFile stateFile(repository.GetCollectionRootFolder() + "/TransformationState");
	QTextStream textStream(&stateFile);

	if (stateFile.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate)){
		textStream << (int)state;
		stateFile.close();

		return true;
	}

	return false;
}


bool CFileTransformationControllerComp::ReplaceWithTransformedItems(IFileObjectCollection& repository) const
{
	QFileInfoList fileList;
	ifile::CFileListProviderComp::CreateFileList(repository.GetCollectionRootFolder(), 0, 1, {"*.new"}, QDir::NoSort, fileList);

	for (const QFileInfo& file : fileList){
		QString filePath = file.filePath();
		QString newFilePath = filePath.chopped(4);
		if (!istd::CSystem::FileCopy(filePath, newFilePath, true)){
			return false;
		}

		if (!QFile(filePath).remove()){
			return false;
		}
	}

	return true;
}


bool CFileTransformationControllerComp::CleanupTrasformation(IFileObjectCollection& repository) const
{
	bool retVal = true;

	imtbase::ICollectionInfo::Ids itemIds = GetRepositoryItemIds(repository);

	for (const QByteArray& itemId : itemIds){
		const IRepositoryItemInfo* itemInfoPtr = GetRepositoryItemInfo(repository, itemId);
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


