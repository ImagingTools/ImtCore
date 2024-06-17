import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0

ComboBox {
    id: root;

    property alias gqlCommandId: dataProvider.commandId;
    property alias fields: dataProvider.fields;

    Component.onCompleted: {
        dataProvider.updateModel();
    }

    function getAdditionalInputParams(){
        return {};
    }

    CollectionDataProvider {
        id: dataProvider;

        onCollectionModelChanged: {
            console.log("onCollectionModelChanged")

            root.model = dataProvider.collectionModel;
        }

        function getAdditionalInputParams(){
            return root.getAdditionalInputParams();
        }
    }
}
