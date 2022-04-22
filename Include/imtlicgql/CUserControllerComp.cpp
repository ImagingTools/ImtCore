#include <imtlicgql/CUserControllerComp.h>


// ImtCore includes
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <imtlic/CFeatureInfo.h>
#include <imtlic/IFeaturePackage.h>
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtlic/CFeaturePackage.h>



namespace imtlicgql
{


imtbase::CTreeItemModel* CUserControllerComp::ListObjects(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			QString& errorMessage) const
{
	return nullptr;
}


istd::IChangeable* CUserControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	return nullptr;
}


} // namespace imtlicgql


