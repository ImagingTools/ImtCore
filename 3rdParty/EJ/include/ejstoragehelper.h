#ifndef STORAGEHELPER_H
#define STORAGEHELPER_H

#include "ejdocument.h"
#include <QDataStream>


template<typename T>
struct Creator
{
	static T create()
	{
		return T();
	}

	static T create(QDataStream &is)
	{
		T t;
		is >> t;
		return t;
	}
};


template<typename T>
struct StreamReader {};

template<typename T>
struct Creator<T*>
{
	static T* create()
	{
		return new T();
	}
	static T* create(QDataStream &is)
	{
		return StreamReader<T>::read(is);
	}
};


class IntVarLen2
{
public:
	IntVarLen2(quint32 &v) :
		value(v)
	{}

	friend QDataStream& operator>>(QDataStream &is, IntVarLen2 &v)
	{
		v.value = 0;
		quint8 c;
		do {
			is >> c;
			v.value = (v.value << 7) | (c & 0x7f);
		}while(c & 0x80);

		return is;
	}

	friend QDataStream& operator<<(QDataStream &os, IntVarLen2 &v)
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


template<typename T>
class ByteArrVarLen
{
public:
	ByteArrVarLen(QList<T> &v, bool writeData = true) :
		value(v),
		wd(writeData)
	{}

	friend QDataStream& operator>>(QDataStream &is, ByteArrVarLen &v)
	{
		quint32 len;
		IntVarLen2 t(len);
		is >> t;
		v.value.clear();

		if(len > 10000000)
			throw 1;

		if(v.wd) {
			for(quint32 i = 0; i < len; i++) {
				v.value << Creator<T>::create(is);
			}
		}
		else {
			for(quint32 i = 0; i < len; i++) {
				v.value << Creator<T>::create();
			}
		}

		return is;
	}

	friend QDataStream& operator<<(QDataStream &os, ByteArrVarLen &v)
	{
		quint32 len = v.value.size();
		IntVarLen2 t(len);
		os << t;

		if(v.wd) {
			foreach (const T& item, v.value) {
				write(os, item);
			}
		}

		return os;
	}


private:
	QList<T> &value;
	bool wd;
};


template<typename T>
void readPatchRecord(QDataStream &stream, quint32 &i1, QList<T> &s1, quint32 &i2, QList<T> &s2, bool rev = true)
{
	IntVarLen2 n2(i2);
	IntVarLen2 n1(i1);
	ByteArrVarLen<T> bal1(s1, rev);
	ByteArrVarLen<T> bal2(s2);
	stream >> n1 >> bal1 >> n2 >> bal2;
}


template<typename T>
QList<T> patch2(const QList<T> &orig, QList<T> &unused, const QByteArray &diff, bool invert = false)
{
	QList<T> result;

	quint8 version;
	bool reversible;

	//    quint32 len = 0;
	quint32 p = 0;
	quint32 p_bak = 0;
	quint32 i1, i2;
	QList<T> s1,s2;
	int lenOrig = orig.length();

	QDataStream stream(diff);
	stream.setVersion(QDataStream::Qt_4_5);
	stream >> version >> reversible;
	p = 0;
	while(!stream.atEnd()) {
		if(invert) {
			readPatchRecord(stream, i2, s2, i1, s1);
		}
		else {
			readPatchRecord(stream, i1, s1, i2, s2, reversible);
		}

		result.append(orig.mid(p, i1-p));
		result.append(s2);
		//        len += i1-p;
		//        len += s2.length();
		//        fordel.append(orig.mid(i1, s1.length()));
		if(lenOrig > p_bak + p - p_bak)
			unused.append(orig.mid(p_bak, p - p_bak));
		p_bak = i1;
		p = i1 + s1.length();
	}
	//    len += orig.length() - p;

	result.append(orig.mid(p));
	qDeleteAll(s1);
	s1.clear();
	//    result.r;

	return result;
}


class COMMONSHARED_EXPORT EjStorageHelper
{
public:
	EjStorageHelper();
	static EjStorageHelper *getInstance();
    static void loadSmallImage(QImage *image, QByteArray &name, bool isSmall = true);
	static void addImage(QString path, QByteArray &name);
	static void addImage(QImage &image, QByteArray &name);
    static QString pathPic();

	static void loadData(EjDocument *doc, QList<QByteArray *> &lData, bool isSaveHistory = false);
	static void updateDoc(EjDocument *doc, quint16 patchVer, quint32 patchTime, bool isSetKey = true);
};
#endif // STORAGEHELPER_H
