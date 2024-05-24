pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root;

    Component.onCompleted: {
        Events.subscribeEvent("SetPermissions", private_.setPermissions)
        Events.subscribeEvent("Logout", private_.onLogout)
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("SetPermissions", private_.setPermissions)
        Events.unSubscribeEvent("Logout", private_.onLogout)
    }

    QtObject {
        id: private_;

        property var permissions: []

        function setPermissions(permissions){
            private_.permissions = permissions;
        }

        function onLogout(){
            private_.setPermissions([]);
        }
    }

    function checkPermission(permissionId){
        if (AuthorizationController.loggedUserIsSuperuser()){
            return true;
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
