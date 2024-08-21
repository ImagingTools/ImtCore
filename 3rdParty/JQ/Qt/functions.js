module.exports = {
    qsTr: function(str){return str},
    binding: function(func, queue){
        func.bound = true
        func.queue = queue
        return func
    },
    locale: function(){
        return {
            decimalPoint: (1.1).toLocaleString().substring(1, 2)
        }
    }
}