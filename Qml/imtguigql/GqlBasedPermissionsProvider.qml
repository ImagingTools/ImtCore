// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * GqlBasedPermissionsProvider
 *
 * GQL implementation of the abstract PermissionsProvider contract.
 * Sends a ProductPermissions query and populates the permissionsModel.
 */
PermissionsProvider {
    id: gqlPermissionsProvider;

    function updateModel(){
        if (gqlPermissionsProvider.productId == ""){
            console.error("Unable to update model for permissions. Error: Product-ID is empty")
            return;
        }

        permissionModel.send();
    }

    property GqlRequestSender permissionModel: GqlRequestSender {
        requestType: 0; // Query
        gqlCommandId: "ProductPermissions";

        function createQueryParams(query){
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("productId", gqlPermissionsProvider.productId);
            query.AddParam(inputParams);
        }

        function onResult(data){
            gqlPermissionsProvider.permissionsModel = data;
        }

        function getHeaders(){
            return gqlPermissionsProvider.getHeaders();
        }
    }
}
