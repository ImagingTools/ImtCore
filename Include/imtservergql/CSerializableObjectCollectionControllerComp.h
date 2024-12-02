#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>


namespace imtservergql
{


class CSerializableObjectCollectionControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSerializableObjectCollectionControllerComp);
		I_ASSIGN(m_collectionIdAttrPtr, "CollectionId", "ID for according to which GQL commands are generated", true, "");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", false, "VersionInfo");
	I_END_COMPONENT;

protected:
	virtual void CustomProcessObject(const imtgql::CGqlRequest& gqlRequest, iser::ISerializable& object) const;

	bool SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const;
	bool DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	imtbase::CTreeItemModel* GetInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	imtbase::CTreeItemModel* GetDataMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* SetObjectDescription(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetElementIds(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_collectionIdAttrPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);

	QByteArrayList m_baseCommandIds;
};


} // namespace imtservergql





