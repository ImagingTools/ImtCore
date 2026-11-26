const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

// ---- FileIO (ImtCore): write() downloads on the web ----------------------------------------------

class FileIO extends QtObject {
    read(){ return '' }

    write(data){
        if(typeof document === 'undefined') return false
        let bytes = typeof data === 'string' ? Uint8Array.from(data, ch => ch.charCodeAt(0) & 0xff) : data
        let url = URL.createObjectURL(new Blob([bytes]))
        let a = document.createElement('a')
        a.href = url
        a.download = String(this.$v[FileIO.SOURCE]).split(/[\\/]/).pop()
        a.click()
        setTimeout(() => URL.revokeObjectURL(url), 0)
        return true
    }

    setSource(source){ this.source = source }
}
defineType(FileIO, { props: { source: 'string' }, signals: ['error(msg)'] })

module.exports = { FileIO }
