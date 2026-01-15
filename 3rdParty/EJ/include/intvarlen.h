/**
 * @brief The IntVarLen class writes integer values as a variable-length
 * sequences.
 */
class IntVarLen
{
public:
    IntVarLen(quint32 &v) :
        value(v)
    {}

    friend QDataStream& operator>>(QDataStream &is, IntVarLen &v)
    {
        v.value = 0;
        quint8 c;
        do {
            is >> c;
            v.value = (v.value << 7) | (c & 0x7f);
        }while(c & 0x80);

        return is;
    }

    friend QDataStream& operator<<(QDataStream &os, IntVarLen &v)
    {
        quint32 value = v.value;

        quint64 buf = value & 0x7f;
        value = value >> 7;

        while(value) {
            buf = (buf << 8) | quint8((value & 0x7f) | 0x80);
            value = value >> 7;
        };

        while(buf & 0x80) {
            os << quint8(buf & 0xff);
            buf = buf >> 8;
        };
        os << quint8(buf & 0xff);

        return os;
    }
    int getValue() { return value; }

private:
    quint32 &value;
};
