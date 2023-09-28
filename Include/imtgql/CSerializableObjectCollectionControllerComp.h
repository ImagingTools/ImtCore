#pragma once


// ACF includes
#include <iser/ISerializable.h>
#include <iser/IArchive.h>

// ImtCore includes
#include <imtgql/CObjectCollectionControllerCompBase.h>


namespace imtgql
{


class CSerializableObjectCollectionControllerComp: public imtgql::CObjectCollectionControllerCompBase
{
public:
    typedef imtgql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSerializableObjectCollectionControllerComp);
		I_ASSIGN(m_objectFactCompPtr, "ObjectFactory", "Factory used for creation of the new Object instance", true, "ObjectFactory");
    I_END_COMPONENT;

protected:
    virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
    virtual istd::IChangeable* CreateObject(const QList<imtgql::CGqlObject>& inputParams, QByteArray &objectId, QString &name, QString &description, QString& errorMessage) const override;

private:
	I_FACT(iser::ISerializable, m_objectFactCompPtr);
};


} // namespace imtgql





