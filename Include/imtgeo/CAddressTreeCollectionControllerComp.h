// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CLegacyObjectCollectionControllerCompBase.h>
#include "imtdb/IDatabaseEngine.h"

namespace imtgeo
{


class CAddressTreeCollectionControllerComp: public imtservergql::CLegacyObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CLegacyObjectCollectionControllerCompBase BaseClass;

    I_BEGIN_COMPONENT(CAddressTreeCollectionControllerComp);
        I_ASSIGN(m_addressTypeCollectionPtr, "AddressTypeComponent", "The string address type collection", true, "AddressTypeCollection");
        I_ASSIGN(m_engineCompChr3, "DatabaseEngineChr3", "Database engine used for low level SQL quering", true, "DatabaseEngineChr3");
    I_END_COMPONENT;

protected:
    I_REF(imtbase::IObjectCollection, m_addressTypeCollectionPtr);
    I_REF(imtdb::IDatabaseEngine, m_engineCompChr3);


	// reimplemented (imtservergql::CLegacyObjectCollectionControllerCompBase)
    virtual QVariant GetObjectInformation(const QByteArray& informationId, const QByteArray& objectId) const override;
    virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
    virtual imtbase::CTreeItemModel* ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
    bool checkHasChildren(const QString& id) const;
};


} // namespace imtgeo



