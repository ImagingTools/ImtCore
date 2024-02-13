import QtQuick 2.12

import Acf 1.0

import imtcontrols 1.0
import imtcolgui 1.0

SchemeView{
    id: scheme;

    Component.onCompleted: {
        //TEST
        //links for test
        let index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "02", index_link);
        index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "03", index_link);
        index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "04", index_link);
        index_link = linkModel.InsertNewItem();
        linkModel.SetData("ObjectId", "05", index_link);

        let index_link2 = linkModel2.InsertNewItem();
        linkModel2.SetData("ObjectId", "02", index_link2);
        index_link2 = linkModel2.InsertNewItem();
        linkModel2.SetData("ObjectId", "03", index_link2);

        let index_link3 = linkModel3.InsertNewItem();
        linkModel3.SetData("ObjectId", "02", index_link3);
        index_link3 = linkModel3.InsertNewItem();
        linkModel3.SetData("ObjectId", "03", index_link3);
        //

        let index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "01", index);
        objectModel.SetData("X", 300, index);
        objectModel.SetData("Y", 300, index);
        objectModel.SetData("MainText", "Main text Main text Main text", index);
        objectModel.SetData("SecondText", "Second text", index);
        objectModel.SetExternTreeModel("Links", linkModel, index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "02", index);
        objectModel.SetData("X", 600, index);
        objectModel.SetData("Y", 600, index);
        objectModel.SetData("MainText", "Main text 2", index);
        objectModel.SetData("SecondText", "Second text 2", index);
        objectModel.SetData("HasError", true, index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "03", index);
        objectModel.SetData("X", 100, index);
        objectModel.SetData("Y", 100, index);
        objectModel.SetData("MainText", "Main text 3", index);
        objectModel.SetData("SecondText", "Second text 3", index);
        objectModel.SetData("IsComposite", true, index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "04", index);
        objectModel.SetData("X", 600, index);
        objectModel.SetData("Y", 100, index);
        objectModel.SetData("MainText", "Main text 4", index);
        objectModel.SetData("SecondText", "Second text 4", index);
        objectModel.SetExternTreeModel("Links", linkModel3, index);

        index = objectModel.InsertNewItem();
        objectModel.SetData("Id", "05", index);
        objectModel.SetData("X", 100, index);
        objectModel.SetData("Y", 600, index);
        objectModel.SetData("MainText", "Main text 5", index);
        objectModel.SetData("SecondText", "Second text 5", index);
        objectModel.SetExternTreeModel("Links", linkModel2, index);

        scheme.requestPaint();
        //TEST
    }

    TreeItemModel {id: linkModel;/*for test*/}
    TreeItemModel {id: linkModel2;/*for test*/}
    TreeItemModel {id: linkModel3;/*for test*/}

    onCopySignal: {
        if(scheme.selectedIndex >=0){
            scheme.copyObjectFunc(scheme.selectedIndex);
        }
    }
    onPasteSignal: {
        if(scheme.selectedIndex >=0){
            scheme.pasteObjectFunc();
        }
    }
    onDeleteSignal: {
        if(scheme.selectedIndex >=0){
            scheme.deleteObjectFunc(scheme.selectedIndex);
        }
    }
}


