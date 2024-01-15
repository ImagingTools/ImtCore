import QtQuick 2.12

import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: treeViewPage;

    anchors.fill: parent;
    clip: true;

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
                testInsertModelLevel0.SetData("TypeId__", "Node", index);
                testInsertModelLevel0.SetData("HasChildren__", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Маркса", index);
                testInsertModelLevel0.SetData("TypeId__", "Node", index);
                testInsertModelLevel0.SetData("HasChildren__", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Менделеева", index);
                testInsertModelLevel0.SetData("TypeId__", "Node", index);
                testInsertModelLevel0.SetData("HasChildren__", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Королёва", index);
                testInsertModelLevel0.SetData("TypeId__", "Node", index);
                testInsertModelLevel0.SetData("HasChildren__", true, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Гагарина", index);
                testInsertModelLevel0.SetData("TypeId__", "Node", index);
                testInsertModelLevel0.SetData("HasChildren__", false, index);

                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Мира", index);
                testInsertModelLevel0.SetData("TypeId__", "Doc", index);
                testInsertModelLevel0.SetData("HasChildren__", false, index);


            }

            treeView.insertTree(-1, testInsertModelLevel0);//!!!

            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name", "дом 1" , index);
            testInsertModel.SetData("TypeId__", "Node", index);
            testInsertModel.SetData("HasChildren__", true, index);

            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name","дом 2" , index);
            testInsertModel.SetData("TypeId__", "Node", index);
            testInsertModel.SetData("HasChildren__", true, index);

            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name", "дом 3" , index);
            testInsertModel.SetData("TypeId__", "Doc", index);
            testInsertModel.SetData("HasChildren__", false, index);

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
