#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtgql/CGqlObject.h>

// GmgCore includes
#include <imtgql/IGqlObjectCollectionDelegate.h>


namespace imtgql
{


class CGqlObjectCollectionDelegateCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IGqlObjectCollectionDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlObjectCollectionDelegateCompBase);
		I_REGISTER_INTERFACE(IGqlObjectCollectionDelegate);
		I_ASSIGN_MULTI_0(m_objectTypeIdsAttrPtr, "ObjectTypeIds", "ID of the supported object type for this delegate", true);
		I_ASSIGN(m_fileTransferCompPtr, "FileTransfer", "Transferring files by url", true, "HttpFileTransfer");
	I_END_COMPONENT;

	// reimplemented (IGqlObjectCollectionDelegate)
	virtual QByteArrayList GetSupportedObjectTypeIds() const override;
	virtual imtcom::IFileTransfer* GetFileTransfer() const override;

protected:
	I_MULTIATTR(QByteArray, m_objectTypeIdsAttrPtr);
	I_REF(imtcom::IFileTransfer, m_fileTransferCompPtr);
};


} // namespace gmgaws


