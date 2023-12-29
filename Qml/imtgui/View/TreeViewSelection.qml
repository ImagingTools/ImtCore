import QtQuick 2.12

QtObject {
    id: root;

    property int count: items.length;

    property var items: [];

    signal selectionChanged();

    function resetSelection(){
        root.items = []

        root.selectionChanged();
    }

    function singleSelect(item){
        console.log("singleSelect", item);

        root.items = []
        root.items.push(item)

        root.selectionChanged();
    }

    function isSelected(item){
        return root.items.includes(item);
    }

   function select(item){
       if (!contains(item)){
           root.items.push(item)
           root.selectionChanged();
       }
   }

   function deselect(item){
       let pos = root.items.indexOf(item)

       if (pos >= 0){
           root.items.splice(pos, 1)
       }

       root.selectionChanged();
   }

    function contains(item){
        return root.items.includes(item);
    }


    function updateSelection(){   // deprecated
        root.items = [];
    }
}
