#include <imtlicgql/CUserControllerComp.h>


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


