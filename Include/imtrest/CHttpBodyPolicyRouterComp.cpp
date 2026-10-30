// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CHttpBodyPolicyRouterComp.h>


// ImtCore includes
#include <imtrest/CHttpRequest.h>
#include <imtrest/IProtocolEngine.h>


namespace imtrest
{


namespace
{


// The prefixes and the paths are compared without the slashes at the beginning and at the end
QByteArray NormalizePath(QByteArray path)
{
	if (path.startsWith('/')){
		path.remove(0, 1);
	}

	if (path.endsWith('/')){
		path.chop(1);
	}

	return path;
}


} // namespace


// public methods

int CHttpBodyPolicyRouterComp::FindPrefixIndex(const QByteArrayList& pathPrefixes, const QByteArray& path)
{
	const QByteArray normalizedPath = NormalizePath(path);

	int bestIndex = -1;
	qsizetype bestLength = -1;

	for (int i = 0; i < pathPrefixes.size(); ++i){
		const QByteArray prefix = NormalizePath(pathPrefixes[i]);

		const bool isMatch = prefix.isEmpty() || (normalizedPath == prefix) || normalizedPath.startsWith(prefix + '/');
		if (isMatch && (prefix.size() > bestLength)){
			bestLength = prefix.size();
			bestIndex = i;
		}
	}

	return bestIndex;
}


// reimplemented (IHttpRequestBodyPolicy)

IHttpRequestBodyPolicy::BodyDecision CHttpBodyPolicyRouterComp::DecideBody(const CHttpRequest& requestHead) const
{
	const int index = FindPrefixIndex(m_pathPrefixes, requestHead.GetCommandId());
	if (index >= 0){
		const IHttpRequestBodyPolicy* policyPtr = m_policiesCompPtr[index];
		if (policyPtr != nullptr){
			return policyPtr->DecideBody(requestHead);
		}
	}

	if (m_defaultPolicyCompPtr.IsValid()){
		return m_defaultPolicyCompPtr->DecideBody(requestHead);
	}

	// The default policy is obligatory, so there is no way to receive the body that nobody has chosen the policy for: it is a mistake of the settings of the server
	BodyDecision decision;
	decision.mode = BM_REJECT;
	decision.rejectStatusCode = IProtocolEngine::SC_INTERNAL_SERVER_ERROR;

	return decision;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CHttpBodyPolicyRouterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_defaultPolicyCompPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("The default policy is not set, the requests with a body whose path does not match any of the prefixes will be refused"));
	}

	if (m_pathPrefixesAttrPtr.GetCount() != m_policiesCompPtr.GetCount()){
		SendWarningMessage(0, QStringLiteral("The number of the path prefixes and of the policies is different, the extra ones are ignored"));
	}

	const int count = qMin(m_pathPrefixesAttrPtr.GetCount(), m_policiesCompPtr.GetCount());
	for (int i = 0; i < count; ++i){
		m_pathPrefixes.append(m_pathPrefixesAttrPtr[i]);
	}
}


} // namespace imtrest


