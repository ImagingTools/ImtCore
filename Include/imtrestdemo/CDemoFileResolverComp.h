// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtrest/IHttpFileResolver.h>


namespace imtrestdemo
{


/**
	Resolver of the files of the demo server (see imtrest::CHttpFileServletComp): the path \c /files/<path> is the file
	<root>/<path>.

	Rules that the demo shows:
	- a path with \c .. , a hidden file (the name begins with a dot) and a file that is outside of the root
	  after the links are resolved are refused with 403: the resolver protects from the paths outside of the directory;
	- a file in the directory \c private is given only with the header \c Authorization: Bearer <token>, otherwise the answer
	  is 401 with \c WWW-Authenticate;
	- a file that does not exist is 404;
	- \c Content-Type is chosen by the extension; the query \c download=1 adds \c Content-Disposition: attachment.
*/
class CDemoFileResolverComp:
			public icomp::CComponentBase,
			virtual public imtrest::IHttpFileResolver
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDemoFileResolverComp);
		I_REGISTER_INTERFACE(imtrest::IHttpFileResolver);
		I_ASSIGN(m_rootDirectoryAttrPtr, "RootDirectory", "Directory of the files that are given by the server", true, "");
		I_ASSIGN(m_privateTokenAttrPtr, "PrivateToken", "Token that gives the access to the directory 'private'", true, "demo-token");
	I_END_COMPONENT;

	// reimplemented (imtrest::IHttpFileResolver)
	virtual imtrest::IProtocolEngine::StatusCode ResolveFile(const imtrest::CHttpRequest& request, FileDescription& file) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QString, m_rootDirectoryAttrPtr);
	I_ATTR(QByteArray, m_privateTokenAttrPtr);

	// Set when the component is created, the resolver is used from many threads and does not change after that
	QString m_canonicalRoot;
};


} // namespace imtrestdemo


