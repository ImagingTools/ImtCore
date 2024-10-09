import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtgui 1.0
import imtappApplicationSdl 1.0

QtObject {
    id: root;

    property ApplicationInfo serverApplicationInfo: null;
    signal updated();

    onServerApplicationInfoChanged: {
        root.updated();
    }

    function updateModel(){
        request.send();
    }

    property GqlSdlRequestSender request : GqlSdlRequestSender {
        gqlCommandId: ImtappApplicationSdlCommandIds.s_getApplicationInfo;
        sdlObjectComp: Component { ApplicationInfo{} }
        onSdlObjectChanged: {
            root.serverApplicationInfo = sdlObject;
        }
    }
}
