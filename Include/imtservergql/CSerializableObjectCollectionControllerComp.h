// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonObject>

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
	I_END_COMPONENT;

protected:
	virtual void CustomProcessObject(const imtgql::CGqlRequest& gqlRequest, iser::ISerializable& object) const;

	bool SerializeObject(const istd::IPolymorphic* object, QByteArray& objectData) const;
	bool DeSerializeObject(istd::IPolymorphic* object, const QByteArray& objectData) const;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual QJsonObject GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QJsonObject GetInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QJsonObject GetDataMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual QJsonObject GetObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString& errorMessage) const override;
	virtual bool UpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const override;
	virtual QJsonObject GetObjectListFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtservergql





