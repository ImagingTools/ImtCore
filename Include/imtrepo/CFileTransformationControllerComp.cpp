#include <imtrepo/CFileTransformationControllerComp.h>


// ImtCore includes
#include <imtrepo/IFileObjectCollection.h>


namespace imtrepo
{


// public methods

// reimplemented (IRepositoryTransformationController)

bool CFileTransformationControllerComp::TransformRepository(IFileObjectCollection& repository, int fromRevision, int toRevision) const
{
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

	for (const TransformationStep& step : transformations){
		imtbase::ICollectionInfo::Ids fileIds = repository.GetElementIds();
		for (const QByteArray& fileId : fileIds){
			QString filePath = repository.GetFileInfo(fileId).filePath;

			// TODO: prepare backup of the file structure!

			if (!step.transformationPtr->TransformFile(IRepositoryFileTransformaton::RFT_CONTENT, filePath, step.from, step.to)){
				SendErrorMessage(0, QString("Repository file '%1' could not be transformed").arg(filePath));

				// TODO: execute rollback of the whole folder structure!

				return false;
			}
		}
	}

	return true;
}


} // namespace imtrepo


