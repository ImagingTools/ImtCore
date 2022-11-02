import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

TreeViewControllerBase {
    id: container;

    property ModelIndex selectedIndex: collectionView.selectedIndex;

    property Item documentItem;

    onSelectedIndexChanged: {
        console.log("PackageTreeView onSelectedIndexChanged", container.selectedIndex);

        if (container.selectedIndex){
            let rootkey = container.selectedIndex.itemData.Id;
            treeView.visible = itemId != "" && rootkey != null && rootkey != "";

           // commandsDelegate.disconnectModelChanged();
           // treeView.itemStateChanged.disconnect(itemStateChanged);
            console.log("resetProperties");
            resetProperties();

            console.log("hideCurrentItem");
            hideCurrentItem();

            let model = documentModel.GetData("Items");

            //Get path of indexes for the selected item.
            let indexes = selectedIndex.getIndexes();
            console.log("indexes", indexes);

            //Последний индекс наш item(ChildModel может не быть у него)
            let itemIndex = indexes.pop();

            for (let index of indexes){
                model = model.GetData('ChildModel', index)
            }

            console.log("model 2", model.toJSON());

            let dependenciesModel = model.GetData('DependenciesModel', itemIndex);
            if (!dependenciesModel){
                dependenciesModel = model.AddTreeModel('DependenciesModel', itemIndex);
            }

            let parentFeatures = []
            getParentFeatures(dependenciesModel, parentFeatures);

            let selectedFeatures = []

            for (let parentId of parentFeatures){
                getFeaturesLeaves(dependenciesModel, parentId, selectedFeatures);
            }

            for (let featureId of selectedFeatures){
                console.log("selectFeature start", featureId);
                selectFeature(featureId);
                console.log("selectFeature end", featureId);

                let itemData = treeView.getItemDataById(featureId);

                if (itemData){
                    treeView.itemsStateChanged(itemData);
                }

                let childrenDepends = []
                featureDependenciesModel.getAllChildrenDependsFeatures(featureId, childrenDepends);
                setValueToProperty("Active", false, childrenDepends);
            }

            let parentDepends = []
            featureDependenciesModel.getAllParentsDependsFeatures(rootkey, parentDepends);
            setValueToProperty("Visible", false, parentDepends);

            //commandsDelegate.connectModelChanged();
           // treeView.itemStateChanged.connect(itemStateChanged);
            //undoRedoManager.model.modelChanged.connect(undoRedoManager.modelUpdated);
        }
        else{
            treeView.visible = false;
        }
    }

    function itemStateChanged(itemData){
        console.log("PackageTreeView itemStateChanged", itemData.Id, itemData.State);
        let rootkey = container.selectedIndex.itemData.Id;

        let itemId = itemData.Id;
        let itemName = itemData.Name;
        let itemState = itemData.State;
        let itemOptional = itemData.Optional;

        if (rootkey == itemId){
            return;
        }

        let itemParentId = null;

        if (itemData.Parent && itemData.Parent.Level >= 1){
            itemParentId = itemData.Parent.Id;
        }

        let model = documentModel.GetData("Items");

        //Get path of indexes for the selected item.
        let indexes = selectedIndex.getIndexes();

        //Последний индекс на item(ChildModel может не быть у него)
        let itemIndex = indexes.pop()

        for (let index of indexes){
            model = model.GetData('ChildModel', index)
        }

        let dependenciesModel = model.GetData('DependenciesModel', itemIndex);

        if (!dependenciesModel){
            dependenciesModel = model.AddTreeModel('DependenciesModel', itemIndex);
        }

        let childrenDepends = []
        featureDependenciesModel.getAllChildrenDependsFeatures(itemId, childrenDepends);

        if (itemState == Qt.Checked || itemState == Qt.PartiallyChecked){
            let itemIndex = -1;
            for (let i = 0; i < dependenciesModel.GetItemsCount(); i++){
                let curItemId = dependenciesModel.GetData("Id", i);

                if (curItemId == itemId){
                    itemIndex = i;
                    break;
                }
            }

            if (itemIndex < 0){
                itemIndex = dependenciesModel.InsertNewItem();
            }

            if (dependenciesModel.ContainsKey("State", itemIndex)){
                let curState = dependenciesModel.GetData("State", itemIndex);
                if (curState == itemState){
                    return;
                }
            }

            dependenciesModel.SetData("Id", itemId, itemIndex);
            dependenciesModel.SetData("Name", itemName, itemIndex);
            dependenciesModel.SetData("State", itemState, itemIndex);
            dependenciesModel.SetData("Optional", itemOptional, itemIndex);
            dependenciesModel.SetData("ParentId", itemParentId, itemIndex);

            setValueToProperty("Active", false, childrenDepends);
        }
        else{
            let itemIndex = -1;
            for (let i = 0; i < dependenciesModel.GetItemsCount(); i++){
                let featureId = dependenciesModel.GetData("Id", i);

                if (featureId == itemId){
                    itemIndex = i;
                    break;
                }
            }

            if (itemIndex >= 0){
                console.log("dependenciesModel RemoveItem", model.toJSON());
                dependenciesModel.RemoveItem(itemIndex);
            }

            setValueToProperty("Active", true, childrenDepends);
        }

        synchronise();
    }

    function synchronise(){
        let rootFeatureId = container.selectedIndex.itemData.Id;

        let model = documentModel.GetData("Items");

        //Get path of indexes for the selected item.
        let indexes = selectedIndex.getIndexes();

        //Последний индекс наш item(ChildModel может не быть у него)
        let itemIndex = indexes.pop()

        for (let index of indexes){
            model = model.GetData('ChildModel', index)
        }

        let dependenciesModel = model.GetData('DependenciesModel', itemIndex);
        featureDependenciesModel.model.SetData(rootFeatureId, dependenciesModel);

        console.log("featureDependenciesModel", featureDependenciesModel.model.toJSON());
    }

    function  hideCurrentItem(){
        undoRedoManager.model.modelChanged.disconnect(undoRedoManager.modelUpdated);

        let packageId = documentItem.itemId;
        let featureId = selectedIndex.itemData.Id;

        for (let i = 0; i < treeView.model.GetItemsCount(); i++){
            let c_packageId = treeView.model.GetData("Id", i);
            if (c_packageId === packageId){
                let featuresModel = treeView.model.GetData("ChildModel", i);
                _updateVisibleElements(featuresModel, featureId);

                break;
            }
        }

        undoRedoManager.model.modelChanged.connect(undoRedoManager.modelUpdated);
    }

    function _updateVisibleElements(model, itemId){
        console.log("PackageTreeView updateVisibleElements");

        for (let i = 0; i < model.GetItemsCount(); i++){
            let c_itemId = model.GetData("Id", i);
            console.log("c_itemId", c_itemId);
            if (c_itemId == itemId){
                console.log(c_itemId, "Visible false");
                model.SetData("Visible", false, i);

                return;
            }
            else{
                let childModel = model.GetData("ChildModel", i);
                if (childModel){
                    _updateVisibleElements(childModel, itemId);
                }
            }
        }
    }
}
