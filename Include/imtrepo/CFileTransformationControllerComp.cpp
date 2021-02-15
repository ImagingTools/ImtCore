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

CFileTransformationControllerComp::CFileTransformationControllerComp()
	:m_itemInfoProvider(nullptr)
{
}


// reimplemented (IRepositoryTransformationController)

bool CFileTransformationControllerComp::TransformRepository(IFileObjectCollection& repository, int fromRevision, int toRevision) const
{
	m_itemInfoProvider = dynamic_cast<IRepositoryItemInfoProvider*>(&repository);
	if (m_itemInfoProvider == nullptr){
		return false;
	}

	bool isOk;
	TransformationState transformationState = GetTransformationState(repository, isOk);

	if (transformationState == TS_REPLACING){
		if (ApplyNewRevision(repository)){
			if (SetTransformationState(repository, TS_IDLE)){
				return true;
			}

			return false;
		}

		return false;
	}

	m_repositoryItemIds = m_itemInfoProvider->GetRepositoryItemIds();
	if (!CleanupTrasformation(repository)){
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

	if (SetTransformationState(repository, TS_IN_PROGRESS)){
		return false;
	}

	bool isFailed = false;

	for (int itemIndex = 0; itemIndex < m_repositoryItemIds.count() && !isFailed; itemIndex++){
		QFileInfo itemInfo;

		for (int stepIndex = 0; stepIndex < transformations.count() && !isFailed; stepIndex++){
			if (stepIndex == 0){
				IRepositoryItemInfoProvider::FileIds fileIds = m_itemInfoProvider->GetRepositoryItemFileIds(m_repositoryItemIds[itemIndex]);

				for (FileId fileId : fileIds){
					QString filePath = m_itemInfoProvider->GetRepositoryItemFilePath(m_repositoryItemIds[itemIndex], fileId);
					if (!filePath.isEmpty()){
						if (!istd::CSystem::FileCopy(filePath, filePath + ".new", true)){
							isFailed = true;
							break;
						}

						if (fileId == FI_ITEM_INFO){
							itemInfo = filePath;
						}
					}
				}
			}

			const TransformationStep& step = transformations[stepIndex];

			if (!step.transformationPtr->TransformFile(*this, m_repositoryItemIds[itemIndex], step.from, step.to)){
				SendErrorMessage(0, QString("Repository file '%1' could not be transformed").arg(itemInfo.absoluteFilePath()));

				isFailed = true;
				break;
			}
		}
	}

	if (isFailed){
		CleanupTrasformation(repository);
		SetTransformationState(repository, TS_IDLE);

		return false;
	}

	if (SetTransformationState(repository, TS_REPLACING)){
		return false;
	}

	if (ApplyNewRevision(repository)){
		CleanupTrasformation(repository);
		if (SetTransformationState(repository, TS_IDLE)){
			return true;
		}
	}

	return false;
}


// protected methods

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


bool CFileTransformationControllerComp::ApplyNewRevision(IFileObjectCollection& repository) const
{
	QFileInfoList fileList;
	ifile::CFileListProviderComp::CreateFileList(repository.GetCollectionRootFolder(), 0, 1, {"*.new"}, QDir::NoSort, fileList);

	for (const QFileInfo& file : fileList){
		QString filePath = file.filePath();
		QString newFilePath = filePath.chopped(4);
		if (!istd::CSystem::FileMove(filePath, newFilePath, true)){
			return false;
		}
	}

	return true;
}


bool CFileTransformationControllerComp::CleanupTrasformation(IFileObjectCollection& repository) const
{
	IRepositoryItemInfoProvider* infoProvider = dynamic_cast<IRepositoryItemInfoProvider*>(&repository);
	if (infoProvider == nullptr){
		return false;
	}

	bool retVal = true;
		
	for (const QByteArray& itemId : m_repositoryItemIds){
		IRepositoryItemInfoProvider::FileIds fileIds = infoProvider->GetRepositoryItemFileIds(itemId);
		for (IRepositoryItemInfoProvider::FileId fileId : fileIds){
			QString filePath = infoProvider->GetRepositoryItemFilePath(itemId, fileId) + ".new";
			QFile file(filePath);

			if (file.exists()){
				if (!file.remove()){
					retVal = false;
				}
			}

		}
	}

	return retVal;
}


// reimplemented (IRepositoryItemInfoProvider)

IRepositoryItemInfoProvider::ItemIds CFileTransformationControllerComp::GetRepositoryItemIds() const
{
	return m_repositoryItemIds;
}


IRepositoryItemInfoProvider::FileIds CFileTransformationControllerComp::GetRepositoryItemFileIds(const QByteArray& itemId) const
{
	if (m_itemInfoProvider != nullptr){
		return m_itemInfoProvider->GetRepositoryItemFileIds(itemId);
	}

	return FileIds();
}


QString CFileTransformationControllerComp::GetRepositoryItemFilePath(const QByteArray& itemId, IRepositoryItemInfoProvider::FileId fileId) const
{
	if (m_itemInfoProvider != nullptr){
		QString filePath = m_itemInfoProvider->GetRepositoryItemFilePath(itemId, fileId);
		if (!filePath.isEmpty()){
			return filePath + ".new";
		}
	}

	return QString();
}


} // namespace imtrepo


