// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtauthUsersSdl 1.0
import imtauthgui 1.0

/**
 * GqlBasedPasswordPolicyProvider
 *
 * SDL/GQL implementation of the PasswordPolicyController contract: sends
 * GetPasswordPolicy and feeds the answer back through applyPolicy(). This is the
 * only place that knows how the policy reaches the client, so imtauthgui stays
 * free of GQL.
 */
PasswordPolicyController {
    id: root

    property string context: ""

    property bool __requestSent: false

    // reimplemented (PasswordPolicyController)
    function load() {
        if (root.loaded || root.__requestSent) {
            return
        }

        root.__requestSent = true
        root.__policyRequestSender.send()
    }

    property GqlSdlRequestSender __policyRequestSender: GqlSdlRequestSender {
        context: root.context
        requestType: 1
        gqlCommandId: ImtauthUsersSdlCommandIds.s_getPasswordPolicy

        inputObjectComp: Component {
            PasswordPolicyInput {}
        }

        sdlObjectComp: Component {
            PasswordPolicyPayload {
                onFinished: {
                    root.applyPolicy({
                        "minLength": m_minLength,
                        "maxLength": m_maxLength,
                        "requireLowercase": m_requireLowercase,
                        "requireUppercase": m_requireUppercase,
                        "requireDigit": m_requireDigit,
                        "requireSpecialChar": m_requireSpecialChar,
                        "rejectLoginAsPassword": m_rejectLoginAsPassword,
                        "blocklistUsed": m_blocklistUsed,
                        "historyDepth": m_historyDepth
                    })
                }
            }
        }

        function onError(message, type) {
            console.warn("PasswordPolicy: request error:", message, type)
            // Allow a later view to retry instead of staying silently unconfigured.
            root.__requestSent = false
        }
    }
}
