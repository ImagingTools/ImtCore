pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
    id: root;

    /**
        Whether a permission list holds a permission.

        A permission is identified by the full path of its feature, e.g.
        "/Administration/EditUser/AddUser", and that is the form it is held in.
        Either side may be a bare feature id instead, and then only the last segment
        is compared: shared code cannot spell out a path that differs from one
        product to the next, and accounts filled before the paths existed hold bare
        ids to this day. Two paths, however, name two places and never stand in for
        each other.

        Mirrors imtauth::HasPermission, which decides the same question on the server.
    */
    function hasPermission(permissions, permissionPath){
        if (!permissionPath || permissionPath === ""){
            return false;
        }

        if (permissions.indexOf(permissionPath) >= 0){
            return true;
        }

        let isPath = permissionPath.indexOf('/') === 0;
        let featureId = permissionPath.substring(permissionPath.lastIndexOf('/') + 1);

        for (let i = 0; i < permissions.length; ++i){
            let permission = permissions[i];
            if (isPath && permission.indexOf('/') === 0){
                continue;
            }

            if (permission.substring(permission.lastIndexOf('/') + 1) === featureId){
                return true;
            }
        }

        return false;
    }

    function checkPermission(permissionPath){
        if (AuthorizationController.loggedUserIsSuperuser()){
            return true;
        }

        return root.hasPermission(AuthorizationController.getPermissions(), permissionPath);
    }

    function getPermissions(){
        return AuthorizationController.getPermissions();
    }
}
