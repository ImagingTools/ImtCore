import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthAuthorizationSdl 1.0

QtObject {
    id: container;

	property string accessToken: "";
	property string refreshToken: "";
    property string login: "";
    property string userId: "";
    property string systemId: "";
    property string productId: "";
    property var permissions: [];
    property bool isTokenGlobal: true

    signal accepted();
    signal failed(string message);

    property string modelState: container.authorizationGqlModel.state;

    function getHeaders(){
        return {}
    }

    function authorization(loginF, passwordF){
        request.addInputParam(AuthorizationInputTypeMetaInfo.s_login, loginF);
        request.addInputParam(AuthorizationInputTypeMetaInfo.s_password, passwordF);
        request.addInputParam(AuthorizationInputTypeMetaInfo.s_productId, container.productId);

        request.send();
    }

    property GqlSdlRequestSender request : GqlSdlRequestSender {
        gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_authorization;
        sdlObjectComp:
            Component {
            AuthorizationPayload {
                onFinished: {
					container.refreshToken = m_refreshToken;
					container.accessToken = m_token;

                    container.userId = m_userId;
                    container.login = m_username;
                    container.systemId = m_systemId;
                    if (m_permissions){
                        container.permissions = m_permissions.split(';')
                    }
                    container.accepted();
                }
            }
        }

        function getHeaders(){
            return container.getHeaders();
        }
    }

    property GqlModel authorizationGqlModel: GqlModel {}
}
