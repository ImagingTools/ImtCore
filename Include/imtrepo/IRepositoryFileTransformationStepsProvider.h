// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>

// ImtCore includes
#include <imtrepo/IRepositoryFileTransformation.h>


namespace imtrepo
{


/**
	Interface defining the transformation of a file inside of the repository structure.
	\ingroup Collection
	\ingroup FileRepository
*/
class IRepositoryFileTransformationStepsProvider: public istd::IPolymorphic
{
public:
	struct TransformationStep
	{
		int from = -1;
		int to = -1;
		const IRepositoryFileTransformation* transformationPtr = nullptr;
	};

	typedef QVector<TransformationStep> TransformationSteps;

	virtual TransformationSteps GetTransformationSteps(int fromRevision, int toRevision) const = 0;
};


} // namespace imtrepo


