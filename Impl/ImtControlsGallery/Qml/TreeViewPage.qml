import QtQuick 2.12

import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: treeViewPage;

    anchors.fill: parent;

    TreeViewGql{
        id: treeView;

        anchors.centerIn: parent;

        width: parent.width - 200;
        height: parent.height - 200;
        hasSelection: true;

        Component.onCompleted: {//TEST

            let date = new Date();
            let val = date.valueOf();
            let index;

            for(let i = 0; i < 10; i++){
                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Ленина", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Маркса", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Менделеева", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Королёва", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Гагарина", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", false, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Мира", index);
                testInsertModelLevel0.SetData("TypeId", "Doc", index);
                testInsertModelLevel0.SetData("HasChildren", false, index);


            }

            treeView.insertTree(-1, testInsertModelLevel0);//!!!

            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name", "дом 1" , index);
            testInsertModel.SetData("TypeId", "Node", index);
            testInsertModel.SetData("HasChildren", true, index);

            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name","дом 2" , index);
            testInsertModel.SetData("TypeId", "Node", index);
            testInsertModel.SetData("HasChildren", true, index);

            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name", "дом 3" , index);
            testInsertModel.SetData("TypeId", "Doc", index);
            testInsertModel.SetData("HasChildren", false, index);

        }

        onRequestSignal: {
            treeView.insertTree(index, testInsertModel);
        }
    }

    TreeItemModel{
        id: testInsertModel;
    }

    TreeItemModel{
        id: testInsertModelLevel0;
    }
}
