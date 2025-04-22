#pragma once


// ACF includes
#include <istd/IInformationProvider.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IMultiStatusProvider.h>

// Acula includes
#include <imthype/ITaskMetaInfo.h>


namespace imthype
{


/**
	Container of the task results.
*/
class ITaskResultCollection:
			virtual public imtbase::IObjectCollection,
			virtual public imtbase::IMultiStatusProvider,
			virtual public imthype::ITaskMetaInfo
{
public:
	virtual void SetArtifactType(const QByteArray& artifactId, ArtifactType artifactType) = 0;
	virtual void SetArtifactStatus(const QByteArray& artifactId, istd::IInformationProvider::InformationCategory status) = 0;

	/**
		Insert a new result data object into the container.
	*/
	virtual QByteArray InsertNewResult(
				ArtifactType artifactType,
				const QByteArray& typeId,
				const QString& name,
				const QString& description,
				const istd::IInformationProvider* statusProviderPtr = nullptr,
				const istd::IChangeable* defaultValuePtr = nullptr) = 0;
};


} // namespace imthype


