// Qt.md5(data): QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Md5).toHex() - the hex digest of the
// UTF-8 bytes of the string (RFC 1321)

function utf8(text){
    let bytes = []
    for(let i = 0; i < text.length; i++){
        let code = text.charCodeAt(i)
        // a surrogate pair is one code point; a lone surrogate is U+FFFD, as QString::toUtf8
        if(code >= 0xd800 && code <= 0xdbff && i + 1 < text.length){
            let low = text.charCodeAt(i + 1)
            if(low >= 0xdc00 && low <= 0xdfff){
                code = 0x10000 + ((code - 0xd800) << 10) + (low - 0xdc00)
                i++
            } else code = 0xfffd
        } else if(code >= 0xd800 && code <= 0xdfff) code = 0xfffd
        if(code < 0x80) bytes.push(code)
        else if(code < 0x800) bytes.push(0xc0 | code >> 6, 0x80 | code & 63)
        else if(code < 0x10000) bytes.push(0xe0 | code >> 12, 0x80 | code >> 6 & 63, 0x80 | code & 63)
        else bytes.push(0xf0 | code >> 18, 0x80 | code >> 12 & 63, 0x80 | code >> 6 & 63, 0x80 | code & 63)
    }
    return bytes
}

const S = [7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21]
const K = Array.from({ length: 64 }, (_, i) => Math.floor(Math.abs(Math.sin(i + 1)) * 0x100000000) >>> 0)

function md5(data){
    let bytes = utf8(String(data))
    let length = bytes.length
    bytes.push(0x80)
    while(bytes.length % 64 !== 56) bytes.push(0)
    let bits = length * 8
    for(let i = 0; i < 8; i++){
        bytes.push(bits % 256)
        bits = Math.floor(bits / 256)
    }
    let a0 = 0x67452301, b0 = 0xefcdab89, c0 = 0x98badcfe, d0 = 0x10325476
    let m = new Array(16)
    for(let offset = 0; offset < bytes.length; offset += 64){
        for(let i = 0; i < 16; i++){
            let j = offset + i * 4
            m[i] = bytes[j] | bytes[j + 1] << 8 | bytes[j + 2] << 16 | bytes[j + 3] << 24
        }
        let a = a0, b = b0, c = c0, d = d0
        for(let i = 0; i < 64; i++){
            let f, g
            if(i < 16){ f = b & c | ~b & d; g = i }
            else if(i < 32){ f = d & b | ~d & c; g = (5 * i + 1) % 16 }
            else if(i < 48){ f = b ^ c ^ d; g = (3 * i + 5) % 16 }
            else { f = c ^ (b | ~d); g = 7 * i % 16 }
            let sum = (a + f + K[i] + m[g]) | 0
            a = d
            d = c
            c = b
            b = (b + (sum << S[i] | sum >>> (32 - S[i]))) | 0
        }
        a0 = (a0 + a) | 0
        b0 = (b0 + b) | 0
        c0 = (c0 + c) | 0
        d0 = (d0 + d) | 0
    }
    let hex = ''
    for(let word of [a0, b0, c0, d0]){
        for(let i = 0; i < 4; i++) hex += ((word >>> (i * 8)) & 255).toString(16).padStart(2, '0')
    }
    return hex
}

module.exports = { md5 }
