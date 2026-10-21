pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

QtObject {
    id: root;

    /**
        Whether a permission list holds a permission.

        A permission is the full path of its feature, e.g. "/Administration/Users/Edit/Add".
        A group of the tree is held when a permission inside it is held; a feature id
        alone names no permission.

        Mirrors imtauth::HasPermission, which decides the same question on the server.
    */
    function hasPermission(permissions, permissionPath){
        if (!permissionPath || permissionPath.indexOf('/') !== 0){
            return false;
        }

        if (permissions.indexOf(permissionPath) >= 0){
            return true;
        }

        let groupPrefix = permissionPath + '/';
        for (let i = 0; i < permissions.length; ++i){
            if (permissions[i].indexOf(groupPrefix) === 0){
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
