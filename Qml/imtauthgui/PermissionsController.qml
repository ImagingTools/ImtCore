pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

QtObject {
    id: root;

    function checkPermission(permissionId){
        if (AuthorizationController.loggedUserIsSuperuser()){
            return true;
        }

        let permissions = AuthorizationController.getPermissions();

        if (permissions.includes(permissionId)){
            return true;
        }

        return false;
    }

    function getPermissions(){
        return AuthorizationController.getPermissions();
    }
}
