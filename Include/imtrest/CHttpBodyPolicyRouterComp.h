// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArrayList>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtrest/IHttpRequestBodyPolicy.h>


namespace imtrest
{


/**
	Policy of the body that chooses another policy by the path of the request. The policy with the longest matching
	prefix decides. A prefix matches whole segments of the path: \c /files matches \c /files and \c /files/a, but not
	\c /files2. The empty prefix (or \c /) matches all paths and is used if no other prefix matches.
	If no prefix matches, the body is received with the limit of the server.
*/
class CHttpBodyPolicyRouterComp:
			public ilog::CLoggerComponentBase,
			virtual public IHttpRequestBodyPolicy
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CHttpBodyPolicyRouterComp);
		I_REGISTER_INTERFACE(IHttpRequestBodyPolicy);
		I_ASSIGN_MULTI_0(m_pathPrefixesAttrPtr, "PathPrefixes", "Path prefixes of the requests, the policy with the same index is used for the requests of the prefix", false);
		I_ASSIGN_MULTI_0(m_policiesCompPtr, "Policies", "Policies of the prefixes, in the order of the prefixes", false);
	I_END_COMPONENT

	/**
		Find the prefix for the path.
		\return Index of the prefix with the longest match or -1 if no prefix matches. If several equal prefixes match,
				the first of them is used.
	*/
	static int FindPrefixIndex(const QByteArrayList& pathPrefixes, const QByteArray& path);

	// reimplemented (IHttpRequestBodyPolicy)
	virtual BodyDecision DecideBody(const CHttpRequest& requestHead) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_MULTIATTR(QByteArray, m_pathPrefixesAttrPtr);
	I_MULTIREF(IHttpRequestBodyPolicy, m_policiesCompPtr);

	// Set when the component is created, the policy is used from many threads and does not change after that.
	QByteArrayList m_pathPrefixes;
};


} // namespace imtrest


