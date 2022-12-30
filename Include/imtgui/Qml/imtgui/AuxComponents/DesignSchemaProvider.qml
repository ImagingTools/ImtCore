import QtQuick 2.15
import Acf 1.0
import imtqml 1.0

Item {
    id: container;

    property TreeItemModel model: TreeItemModel {}

    function getDesignSchema(){
        for (let i = 0; i < model.GetItemsCount(); i++){
            let pageModel = model.GetModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId == "General"){
                    let elements = pageModel.GetData("Elements");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId == "DesignSchema"){
                            let elementValue = elements.GetData("Value", j);
                            let parameters = elements.GetData("Parameters", j);

                            let scheme = parameters.GetData("Id", elementValue);

                            return scheme;
                        }
                    }
                }
            }
        }

        return null;
    }
}
