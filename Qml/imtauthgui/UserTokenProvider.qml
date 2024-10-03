import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthAuthorizationSdl 1.0

QtObject {
    id: container;

    property string token: "";
    property string login: "";
    property string userId: "";
    property string systemId: "";
    property string productId: "";
    property var permissions: [];
    property bool isTokenGlobal: true

    signal accepted();
    signal failed(string message);

    property string modelState: container.authorizationGqlModel.state;

    Component.onCompleted: {
        Events.subscribeEvent("GetToken", container.getToken);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("GetToken", container.getToken);
    }

    function getToken(callback){
        if (callback){
            callback(token);
        }
    }

    function getAdditionalInputParams(){
        return {}
    }

    function authorization(loginF, passwordF){
        request.addInputParam(AuthorizationInputTypeMetaInfo.s_login, loginF);
        request.addInputParam(AuthorizationInputTypeMetaInfo.s_password, passwordF);
        request.addInputParam(AuthorizationInputTypeMetaInfo.s_productId, container.productId);

        let additionInputParams = container.getAdditionalInputParams();
        if (Object.keys(additionInputParams).length > 0){
            let additionParams = Gql.GqlObject("addition");
            for (let key in additionInputParams){
                additionParams.InsertField(key, additionInputParams[key]);
            }
            request.inputParams.InsertFieldObject(additionParams);
        }

        request.send();
    }

    property GqlSdlRequestSender request : GqlSdlRequestSender {
        gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_authorization;
        sdlObjectComp:
            Component {
            AuthorizationPayload {
                onFinished: {
                    container.token = m_token;
                    if (container.isTokenGlobal){
                        container.authorizationGqlModel.SetGlobalAccessToken(m_token);
                    }

                    container.userId = m_userId;
                    container.login = m_username;
                    container.systemId = m_systemId;
                    container.permissions = m_permissions.split(';')

                    container.accepted();
                }
            }
        }
    }

    property GqlModel authorizationGqlModel: GqlModel {}
}
