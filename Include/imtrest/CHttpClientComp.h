#pragma once

// Qt includes
#include <QtCore/QObject>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <imod/TSingleModelObserverBase.h>
#include <istd/IChangeable.h>

#include <imtbase/IObjectCollection.h>
#include <imtbase/CObjectCollectionBase.h>
#include <imtrest/IHttpClientRequest.h>


namespace imtrest
{

/*!
	\brief The CHttpClientComp is a manager of generated http requests created by other modules of THIS application. It's is an observer of requests queue.
	\warning Do not forget to create instance of this component!
*/
class CHttpClientComp: public ilog::CLoggerComponentBase,
		public imod::TSingleModelObserverBase<imtbase::IObjectCollection>
{

public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpClientComp)
		I_ASSIGN(m_urlAttrPtr, "URL", "The Url, or it's parts that shud be placed inrequest if not exists\nWarning overwrites values:\n\t UrlFragment\n\t UrlHost\n\t UrlPassword\n\t UrlPort\n\t UrlPostPath\n\t UrlPrePath\n\t UrlScheme\n\t UrlUserName", false, "");
		I_ASSIGN(m_urlFragmentAttrPtr, "UrlFragment", "The ID of the 'anchor' with the preceding # character. The anchor can be a title inside the document or an element id attribute. Using this link, the browser opens the page and moves the window to the specified element.", false, "");
		I_ASSIGN(m_urlHostAttrPtr, "UrlHost", "Fully spelled out domain name of the host in the DNS or IP address of the host in the form of four groups of decimal numbers separated by dots; the integers are in the range from 0 to 255.", false, "");
		I_ASSIGN(m_urlPasswordAttrPtr, "UrlPassword", "The password of the specified user", false, "");
		I_ASSIGN(m_urlPortAttrPtr, "UrlPort", "The host port for connecting", false, -1);
		I_ASSIGN(m_urlPostPathAttrPtr, "UrlPostPath", "The clarifying information about the location of the resource; depends on the protocol. places AT END of the path of the request", false, "");
		I_ASSIGN(m_urlPrePathAttrPtr, "UrlPrePath", "The clarifying information about the location of the resource; depends on the protocol. places BEFORE the path of the request", false, "");
		I_ASSIGN(m_urlSchemeAttrPtr, "UrlScheme", "The scheme of circulation of a resource; in most cases this refers to the network Protocol", false, "");
		I_ASSIGN(m_urlUserNameAttrPtr, "UrlUserName", "The user name used to access the resource", false, "");

		I_ASSIGN(m_objectCollectionCompPtr, "HttpRequestsCollection", "Comment", true, "ObjectCollection");
		I_ASSIGN_TO(m_objectCollectionModelCompPtr, m_objectCollectionCompPtr, true);

	I_END_COMPONENT


protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;


	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet);


private:
	I_ATTR(QString, m_urlAttrPtr);
	I_ATTR(QString, m_urlFragmentAttrPtr);
	I_ATTR(QString, m_urlHostAttrPtr);
	I_ATTR(QString, m_urlPasswordAttrPtr);
	I_ATTR(int,		m_urlPortAttrPtr);
	I_ATTR(QString, m_urlPostPathAttrPtr);
	I_ATTR(QString, m_urlPrePathAttrPtr);
	I_ATTR(QString, m_urlSchemeAttrPtr);
	I_ATTR(QString, m_urlUserNameAttrPtr);

	I_REF(imtbase::IObjectCollection, m_objectCollectionCompPtr);
	I_REF(imod::IModel, m_objectCollectionModelCompPtr);

	QNetworkAccessManager m_networkAccessManager;
	QUrl m_baseUrl;



};


} // namespace imtrest


