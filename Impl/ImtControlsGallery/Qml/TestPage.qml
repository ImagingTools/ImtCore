import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import QtQuick.Window 2.2
import imtgui 1.0
import imtdocgui 1.0

Rectangle {
    id: testPage;

    anchors.fill: parent;
    clip: true;

    Component.onCompleted: {
        let index = rootModel.insertNewItem();
        rootModel.setData("Id", "Hardware", index)
        rootModel.setData("Name", "Hardware", index)

        let itemsModel = rootModel.addTreeModel("Items")

//        for (let i = 0; i < 100; i++){
//            let index = itemsModel.insertNewItem();
//            itemsModel.setData("Id", i, index)
//            itemsModel.setData("Name", "", index)
//            itemsModel.setData("Description", "", index)
//        }

        for (let i = 0; i < 300; i++){
            let index = itemsModel.insertNewItem();
            itemsModel.setData("Id", i, index)

            if (i < 100){
                itemsModel.setData("Name", "Hardware" + i, index)
                itemsModel.setData("Description", "Hardware"  + i, index)
            }
            else if (i < 200){
                itemsModel.setData("Name", "", index)
                itemsModel.setData("Description", "", index)
            }
            else{
                itemsModel.setData("Name", "Hardware" + i, index)
                itemsModel.setData("Description", "", index)
            }
        }

//        index = itemsModel.insertNewItem();
//        itemsModel.setData("Id", index, index)
//        itemsModel.setData("Name", "Hardware" + index, index)
//        itemsModel.setData("Description", "Hardware"  + index, index)

//        index = itemsModel.insertNewItem();
//        itemsModel.setData("Id", index, index)
//        itemsModel.setData("Name", "", index)
//        itemsModel.setData("Description", "", index)

//        index = itemsModel.insertNewItem();
//        itemsModel.setData("Id", index, index)
//        itemsModel.setData("Name", "Hardware" + index, index)
//        itemsModel.setData("Description", "", index)

        searchPage.model = rootModel;

        searchPage.doUpdateGui();
    }

    TreeItemModel {
        id: rootModel;
    }

    SearchPage {
        id: searchPage;
        anchors.fill: parent;
    }
}
