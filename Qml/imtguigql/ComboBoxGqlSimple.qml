import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0

ComboBox {
    id: root;

    property alias gqlCommandId: dataProvider.commandId;
    property alias subscriptionCommandId: dataProvider.subscriptionCommandId;
    property alias fields: dataProvider.fields;
    property alias filter: dataProvider.filter;

    Component.onCompleted: {
        dataProvider.updateModel();
    }

    function updateModel(){
        dataProvider.updateModel();
    }

    function getHeaders(){
        return {};
    }

    CollectionDataProvider {
        id: dataProvider;

        onCollectionModelChanged: {
            root.model = dataProvider.collectionModel;
        }

        function getHeaders(){
            return root.getHeaders();
        }
    }
}
