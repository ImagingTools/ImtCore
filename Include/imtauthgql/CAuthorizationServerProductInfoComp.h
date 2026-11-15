// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <memory>

// Qt includes
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

// ACF includes
#include <ibase/IApplicationInfo.h>
#include <ilog/TLoggerCompWrap.h>
#include <imod/CSingleModelObserverBase.h>
#include <imod/IModel.h>

// ImtCore includes
#include <imtclientgql/IServerCredentialProvider.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtlic/CProductInfo.h>
#include <imtlic/IProductInfo.h>


namespace imtauthgql
{


/**
	The permission tree of this product with the permissions of the authorization server, for the roles of the product:
	those permissions belong to the authorization server, which gives them out.
	At the start and after each reconnection this server publishes its own tree there, untranslated, and fetches the
	permissions of the authorization server; a failed step is repeated. A fetched tree replaces the served one when it
	changes, and every tree is kept until the end: a request may still read the one it got.
	A server without a token only fetches: an authorization server that takes products without tokens gives its permissions to it.
*/
class CAuthorizationServerProductInfoComp:
			public QObject,
			public imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase>,
			protected imod::CSingleModelObserverBase,
			virtual public imtlic::IProductInfo
{
public:
	typedef imtclientgql::TClientRequestManagerCompWrap<ilog::CLoggerComponentBase> BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationServerProductInfoComp);
		I_REGISTER_INTERFACE(imtlic::IProductInfo);
		I_REGISTER_INTERFACE(iprm::INameParam);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_productInfoCompPtr, "ProductInfo", "Permission tree of this product", true, "ProductInfo");
		I_ASSIGN(m_serverCredentialCompPtr, "ServerCredentialProvider", "Token of this server: without it the tree of the product is not published", false, "ServerCredentialProvider");
		I_ASSIGN(m_connectionStatusModelCompPtr, "ConnectionStatus", "Connection to the authorization server: a reconnection publishes and fetches again", false, "ConnectionStatus");
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Version of the product, published with its tree", false, "ApplicationInfo");
	I_END_COMPONENT;

	// reimplemented (imtlic::IProductInfo)
	virtual const IProductInfo* GetProductFamilyInfo() const override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QString GetProductDescription() const override;
	virtual void SetProductDescription(const QString& description) override;
	virtual QByteArray GetCategoryId() const override;
	virtual void SetCategoryId(const QByteArray& categoryId) override;
	virtual imtbase::IObjectCollection* GetFeatures() override;
	virtual bool AddFeature(const QByteArray& featureId, const imtlic::IFeatureInfo& featureInfo) override;
	virtual bool RemoveFeature(const QByteArray& featureId) override;
	virtual OptionalFeatureInfos GetOptionalFeatures() const override;
	virtual void SetOptionalFeatures(const OptionalFeatureInfos& optionalFeatures) override;

	// reimplemented (iprm::INameParam)
	virtual const QString& GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual bool IsNameFixed() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

private:
	typedef std::shared_ptr<imtlic::CProductInfo> ProductInfoPtr;
	typedef QList<ProductInfoPtr> ProductInfoList;

	/**
		The served tree; built from the tree of the product on the first use, after the product filled it.
	*/
	imtlic::CProductInfo& GetCurrent() const;
	ProductInfoPtr CreateTree() const;
	void Invalidate();
	bool Publish() const;
	QString GetProductVersion() const;
	bool Fetch();
	void Run();

private:
	I_REF(imtlic::IProductInfo, m_productInfoCompPtr);
	I_REF(imtclientgql::IServerCredentialProvider, m_serverCredentialCompPtr);
	I_REF(imod::IModel, m_connectionStatusModelCompPtr);
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);

	mutable QMutex m_treeMutex;
	mutable ProductInfoList m_trees;
	mutable imtlic::CProductInfo* m_currentTreePtr = nullptr;
	ProductInfoPtr m_importedTreePtr;
	QByteArray m_importedHash;

	QThread* m_threadPtr = nullptr;
	QMutex m_runMutex;
	QWaitCondition m_runCondition;
	bool m_isStopping = false;
	bool m_isPublishNeeded = true;
	// Only with a token: an authorization server takes a tree from the servers of products only
	bool m_isPublishEnabled = true;
	bool m_isFetchNeeded = true;
	bool m_isReconnected = false;
};


} // namespace imtauthgql


