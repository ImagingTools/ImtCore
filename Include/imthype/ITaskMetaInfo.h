// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollectionInfo.h>


namespace imthype
{


/**
	Interface for access the meta-info of an inspection task.
	\sa iinsp::IInspectionTask
*/
class ITaskMetaInfo: virtual public imtbase::IObjectCollectionInfo
{
public:
	/**
		Possible types of an artifact in an inspection task.
	*/
	enum ArtifactType
	{
		/**
			Artifact type is undefined.
		*/
		AT_NONE,

		/**
			Artifact is an input of the task.
		*/
		AT_INPUT,

		/**
			Artifact is an output of the task.
		*/
		AT_OUTPUT,

		/**
			Artifact is an intermediate result of the task.
		*/
		AT_INTERMEDIATE
	};

	/**
		Get the type of the artifact for the given object-ID.
	*/
	virtual ArtifactType GetArtifactType(const QByteArray& artifactId) const = 0;

	/**
		Get the type-ID of the artifact data.
	*/
	virtual QByteArray GetArtifactDataTypeId(const QByteArray& artifactId) const = 0;
};


} // namespace imthype


