import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: treeViewPage;

    anchors.fill: parent;
    clip: true;
	color: Style.baseColor

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
                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Ленина", index);
				testInsertModelLevel0.setData("typeId__", "Node", index);
				testInsertModelLevel0.setData("hasChildren__", true, index);

                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Маркса", index);
				testInsertModelLevel0.setData("typeId__", "Node", index);
				testInsertModelLevel0.setData("hasChildren__", true, index);

                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Менделеева", index);
				testInsertModelLevel0.setData("typeId__", "Node", index);
				testInsertModelLevel0.setData("hasChildren__", true, index);

                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Королёва", index);
				testInsertModelLevel0.setData("typeId__", "Node", index);
				testInsertModelLevel0.setData("hasChildren__", true, index);

                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Гагарина", index);
				testInsertModelLevel0.setData("typeId__", "Node", index);
				testInsertModelLevel0.setData("hasChildren__", false, index);

                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Мира", index);
				testInsertModelLevel0.setData("typeId__", "Doc", index);
				testInsertModelLevel0.setData("hasChildren__", false, index);


            }

            treeView.insertTree(-1, testInsertModelLevel0);//!!!

            index = testInsertModel.insertNewItem();
            testInsertModel.setData("name", "дом 1" , index);
			testInsertModel.setData("typeId__", "Node", index);
			testInsertModel.setData("hasChildren__", true, index);

            index = testInsertModel.insertNewItem();
            testInsertModel.setData("name","дом 2" , index);
			testInsertModel.setData("typeId__", "Node", index);
			testInsertModel.setData("hasChildren__", true, index);

            index = testInsertModel.insertNewItem();
            testInsertModel.setData("name", "дом 3" , index);
			testInsertModel.setData("typeId__", "Doc", index);
			testInsertModel.setData("hasChildren__", false, index);

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
