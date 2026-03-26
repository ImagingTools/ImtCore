import QtQuick

Item {
    id: root; width: 300; height: 200

    Rectangle {
       id : placeholder // Занимает место всегда
       color:"gray"
       anchors.fill : root 
   }
   
   Loader {
       id : loader 
       sourceComponent : Rectangle { 
           color:"blue"
           width : root.width 
           height : root.height 
       }
       
       Component.onCompleted :{
           console.log("Loader загружен. visible:", loader.item.visible);
           console.log("Геометрия placeholder не изменилась при загрузке.");
           loader.item.visible = false;
           console.log("После скрытия item в Loader:");
           console.log("visible:", loader.item.visible);
           // Геометрия placeholder не изменится, так как Loader резервирует место.
       }
   }
}