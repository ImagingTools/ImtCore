import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthAuthorizationSdl 1.0

QtObject {
    id: container;

    property string userMode;

    signal updated();

    onUserModeChanged: {
        Events.sendEvent("UserModeChanged", container.userMode);
    }

    function isNoUserManagement(){
        return userMode === UserModeTypeMetaInfo.s_nO_USER_MANAGEMENT;
    }

    function updateModel(){
        userMode = "";
        request.send();
    }

    property GqlSdlRequestSender request : GqlSdlRequestSender {
        gqlCommandId: ImtauthAuthorizationSdlCommandIds.s_getUserMode;
        sdlObjectComp:
            Component {
            UserManagementPayload {
                onFinished: {
                    container.userMode = m_userMode;
                    container.updated();
                }
            }
        }
    }
}
