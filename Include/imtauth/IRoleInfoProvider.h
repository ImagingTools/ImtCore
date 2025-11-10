#pragma once


// ACF includes
#include <iprm/IParamsSet.h>
#include <istd/IChangeable.h>
#include <idoc/IDocumentMetaInfo.h>


namespace imtbase
{
class ICollectionInfo;
}


namespace imtauth
{


class IRole;
typedef istd::TUniqueInterfacePtr<IRole> IRoleUniquePtr;


/**
	Interface for a collection of roles.
	\ingroup Role
*/
class IRoleInfoProvider: virtual public istd::IChangeable
{
public:
	enum MetaInfoTypes
	{
		/**
			List of role infos given as QStringList.
		*/
		MIT_ROLE_INFO_LIST = idoc::IDocumentMetaInfo::MIT_USER + 1
	};

	/**
		Get list of all available roles.
	*/
	virtual const imtbase::ICollectionInfo& GetRoleList() const = 0;

	/**
		Get information for role by Object-ID.
	*/
	virtual const imtauth::IRoleUniquePtr GetRole(const QByteArray& objectId, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
};


} // namespace imtauth


