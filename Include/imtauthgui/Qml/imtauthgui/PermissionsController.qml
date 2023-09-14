pragma Singleton

import QtQuick 2.12
import Acf 1.0

Item {
    id: root;

    property AuthorizationPage authorizationPage: null;

    Component.onCompleted: {
        Events.subscribeEvent("SetPermissions", private_.setPermissions)
        Events.subscribeEvent("OnLogout", private_.onLogout)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SetPermissions", private_.setPermissions)
        Events.unSubscribeEvent("OnLogout", private_.onLogout)
    }

    QtObject {
        id: private_;

        property var permissions: {[]}

        function setPermissions(permissions){
            console.log("setPermissions", permissions);

            private_.permissions = permissions;
        }

        function onLogout(){
            private_.setPermissions([]);
        }
    }

    function checkPermission(permissionId){
        if (root.authorizationPage != null){
            if (root.authorizationPage.loggedUserIsSuperuser()){
                return true;
            }
        }

        if (private_.permissions.includes(permissionId)){
            return true;
        }

        return false;
    }

    function getPermissions(){
        return private_.permissions;
    }
}
