module.exports = {
    qsTr: function(str){return str},
    binding: function(func, queue){
        func.bound = true
        func.queue = queue
        return func
    }
}