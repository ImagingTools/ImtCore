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

    function setLoginData(refreshToken, accessToken, userId, login, systemId, permissions){
		container.refreshToken = refreshToken
		container.accessToken = accessToken
		container.userId = userId
		container.login = login
		container.systemId = systemId
		container.permissions = permissions

        accepted()
	}

    property GqlSdlRequestSender request : GqlSdlRequestSender {
        gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_authorization;
        sdlObjectComp:
            Component {
            AuthorizationPayload {
                onFinished: {
                    container.setLoginData(m_refreshToken, m_token, m_userId, m_username, m_systemId, m_permissions.split(';'))
                }
            }
        }

        function getHeaders(){
            return container.getHeaders();
        }
    }

    property GqlModel authorizationGqlModel: GqlModel {}
}
