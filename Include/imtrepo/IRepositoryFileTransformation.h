#pragma once


// ACF includes
#include <istd/IPolymorphic.h>
#include <istd/TRange.h>


namespace imtrepo
{


class IFileObjectCollection;


/**
	\ingroup Collection
*/
class IRepositoryFileTransformaton: public istd::IPolymorphic
{
public:
	enum RepositoryFileType
	{
		/**
			File with the object content.
		*/
		RFT_CONTENT,

		/**
			Meta-info file.
		*/
		RFT_META_INFO,

		/**
			File with the strutural information about repository item.
		*/
		RFT_ITEM_INFO
	};

	/**
		Get the revision range supported by the file transformation.
		\note Please note that the upgrade range should be provided.
	*/
	virtual istd::CIntRange GetSupportedRevisionRange() const = 0;

	/**
		Execute the file transformation for the given revision range.
	*/
	virtual bool TransformFile(RepositoryFileType fileType, const QString& filePath, int fromRevision, int toRevision) const = 0;
};


} // namespace imtrepo


