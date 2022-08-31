#include <imtauth/CUserInfoMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtauth
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CUserInfoMetaInfoCreatorComp::CreateMetaInfo(
            const istd::IChangeable* dataPtr,
            const QByteArray& typeId,
            idoc::MetaInfoPtr& metaInfoPtr) const
{
    return true;
}


// public methods of embedded class MetaInfo

QString CUserInfoMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
    return QString();
}


} // namespace imtauth


