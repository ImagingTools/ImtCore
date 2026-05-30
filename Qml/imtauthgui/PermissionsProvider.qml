import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0

/**
 * PermissionsProvider
 *
 * Abstract contract for loading product permissions.
 * Concrete implementations (e.g. GqlBasedPermissionsProvider) provide the actual transport.
 *
 * This file MUST NOT import imtguigql or any GQL module.
 */
QtObject {
    id: permissionsProvider;

    property string productId: "";
    property TreeItemModel permissionsModel: TreeItemModel {};

    function updateModel(){
        if (permissionsProvider.productId == ""){
            console.error("Unable to update model for permissions. Error: Product-ID is empty")
            return;
        }
    }

    function getHeaders(){
        return {};
    }
}
