#ifndef STORAGEHELPER_H
#define STORAGEHELPER_H

#include "ejdocument.h"
#include <QDataStream>
#include "QIODevice"


template<typename T>
void write(QDataStream &os, T* value)
{
	os << *value;
}

template<typename T>
void write(QDataStream &os, T value)
{
	os << value;
}


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
	static QByteArray getPatch(EjDocument* oldDoc, EjDocument* newDoc, quint16 newVer, QString user);

	static void loadData(EjDocument *doc, QList<QByteArray *> &lData, bool isSaveHistory = false);
	static void updateDoc(EjDocument *doc, quint16 patchVer, quint32 patchTime, bool isSetKey = true);
};


#define PATCH_VERSION quint8(0x02)


template<typename T>
bool equals(T* a, T* b)
{
	return *a == *b;
}


template<typename T>
bool equals(const T &a, const T &b)
{
	return a == b;
}


template<typename T>
class QListRef
{
public:
	explicit QListRef(const QList<T> *ba) :
		ba(ba),
		p(0),
		s(ba->size())
	{}

	QListRef left(int n) const
	{
		return QListRef(ba, p, n);
	}

	QListRef right(int n) const
	{
		return QListRef(ba, p+s-n, n);
	}

	QListRef mid(int n, int m) const
	{
		return QListRef(ba, p+n, s-m-n);
	}

	T at(int pos) const
	{
		return ba->at(p+pos);
	}

	int length() const
	{
		return s;
	}

	bool isEmpty() const
	{
		return s == 0;
	}

	bool contains(T c) const
	{
		typename QList<T>::const_iterator it = begin();
		typename QList<T>::const_iterator e = end();
		while(it != e && !equals(*it, c)) {
			it++;
		}
		return it != e;
	}

	QList<T> toList() const
	{
		return ba->mid(p,s);
	}

	typename QList<T>::const_iterator begin() const
	{
		return ba->constBegin() + p;
	}

	typename QList<T>::const_iterator end() const
	{
		return ba->constBegin() + p + s;
	}

	int pos() const
	{
		return p;
	}

	// 0 1[2 3 4 5]6 7 8 9
	// 9 8 7 6[5 4 3 2]1 0
	int invPos() const
	{
		return ba->size() - p - s;
	}

private:
	const QList<T> *ba;
	int p;
	int s;

	QListRef(const QList<T> *ba, int p, int s) :
		ba(ba),
		p(p),
		s(s)
	{
	}
};


template<typename T>
void algb(const QListRef<T> &a, const QListRef<T> &b, int*k1, int*k0, int d = 1)
{
	const int m = a.length();
	const int n = b.length();

	//    memSet(k0, 0, (n+1)*sizeof(int));
	//    memSet(k1, 0, (n+1)*sizeof(int));
	memset(k0, 0, (n+1)*sizeof(int));
	memset(k1, 0, (n+1)*sizeof(int));

	int *out = k1;

	typename QList<T>::const_iterator i1 = d > 0 ? a.begin() : a.end()-1;

	for(int i = 1; i <= m; i++) {

		qSwap(k0, k1);

		typename QList<T>::const_iterator j1 = d > 0 ? b.begin() : b.end()-1;

		for(int j = 1; j <= n; j++) {
			if(equals(*i1, *j1)) {
				k1[j] = k0[j-1] + 1;
			}
			else {
				k1[j] = k1[j-1] > k0[j] ? k1[j-1] : k0[j];
			}
			j1 += d;
		}
		i1 += d;
	}

	if(out == k0) {
		//    memCopy(k0, k1, (n+1)*sizeof(int));
		memcpy(k0, k1, (n+1)*sizeof(int));
	}
}


template<typename T>
QList<T> lcsr(const QListRef<T> &a, const QListRef<T> &b, int *buff[])
{
	if(a.isEmpty() || b.isEmpty()) {
		return QList<T>();
	}

	const int m = a.length();
	const int n = b.length();
	if(m == 1) {
		if(b.contains(a.at(0))) {
			return a.toList();
		}
		return QList<T>();
	}

	int i = m / 2;

	int *L1 = buff[0];
	int *L2 = buff[1];
	int *t = buff[2];

	algb(a.left(i),    b, L1, t);
	algb(a.right(m-i), b, L2, t, -1);

	int M = L1[0] + L2[n];
	int k = 0;
	for(int j = 1; j <= n; j++) {
		if(L1[j] + L2[n-j] > M) {
			M = L1[j] + L2[n-j];
			k = j;
		}
	}

	return lcsr(a.left(i), b.left(k), buff) + lcsr(a.right(m-i), b.right(n-k), buff);
}


template<typename T>
QList<T> lcs(const QList<T> &a, const QList<T> &b, int cmn_prev = 0, int cmn_suff = 0)
{
	// Optimization: preallocate vectors to avoid memory relocation
	QListRef<T> a_ref(&a);
	QListRef<T> b_ref(&b);
	a_ref = a_ref.mid(cmn_prev,cmn_suff);
	b_ref = b_ref.mid(cmn_prev,cmn_suff);
	QVector<int> L1(b_ref.length()+1);
	QVector<int> L2(b_ref.length()+1);
	QVector<int> t(b_ref.length()+1);

	int *buff[] = {L1.data(), L2.data(), t.data()};
	return lcsr(a_ref, b_ref, buff);

}

/**
 * @return Difference patch between two lists
 */
template<typename T>
QByteArray diff(const QList<T> &a, const QList<T> &b, bool reversible = true)
{
	QByteArray result;
	QDataStream stream(&result, QIODevice::WriteOnly);
	stream.setVersion(QDataStream::Qt_4_5);

	stream << PATCH_VERSION;
	stream << reversible;


	typename QList<T>::const_iterator i1 = a.constBegin();
	typename QList<T>::const_iterator i2 = a.constBegin();
	typename QList<T>::const_iterator j1 = b.constBegin();
	typename QList<T>::const_iterator j2 = b.constBegin();
	typename QList<T>::const_iterator aEnd = a.constEnd();
	typename QList<T>::const_iterator bEnd = b.constEnd();

	int cmn_prev = 0;
	int cmn_suff = 0;
	if(a.count() > 0 && b.count() > 0)
	{
		while(i2 != aEnd && j2 != bEnd && equals(*i2, *j2))
		{
			i2++; j2++;
			cmn_prev++;
		}
		if(i2 != aEnd && j2 != bEnd)
		{
			while(i2 != aEnd-1 && j2 != bEnd-1 && equals(*(aEnd-1), *(bEnd-1)))
			{
				aEnd--; bEnd--;
				cmn_suff++;
			}
		}
	}
	QList<T> t = lcs(a, b, cmn_prev, cmn_suff);


	typename QList<T>::const_iterator tEnd = t.constEnd();

	for(typename QList<T>::const_iterator pk = t.constBegin(); pk != tEnd; pk++) {

		i1 = i2;
		while(i2 != aEnd && !equals(*i2, *pk)) {
			i2++;
		}

		j1 = j2;
		while(j2 != bEnd && !equals(*j2, *pk)) {
			j2++;
		}

		if(i1 != i2 || j1 != j2) {
			quint32 t = i1 - a.constBegin();
			IntVarLen2 n(t);
			QList<T> ba = a.mid(i1 - a.constBegin(), i2-i1);
			ByteArrVarLen<T> bal(ba, reversible);
			stream << n << bal;

			t = j1 - b.constBegin();
			IntVarLen2 n2(t);
			QList<T> ba2 = b.mid(j1 - b.constBegin(), j2-j1);
			ByteArrVarLen<T> bal2(ba2);
			stream << n2 << bal2;
		}

		if(i2 != aEnd) i2++;
		if(j2 != bEnd) j2++;

	}

	i1 = i2;
	i2 = aEnd;
	j1 = j2;
	j2 = bEnd;

	if(i1 != i2 || j1 != j2) {
		quint32 t = i1 - a.constBegin();
		IntVarLen2 n(t);
		QList<T> ba = a.mid(i1 - a.constBegin(), i2-i1);
		ByteArrVarLen<T> bal(ba, reversible);
		stream << n << bal;

		t = j1 - b.constBegin();
		IntVarLen2 n2(t);
		QList<T> ba2 = b.mid(j1 - b.constBegin(), j2-j1);
		ByteArrVarLen<T> bal2(ba2);
		stream << n2 << bal2;
	}

	return result;
}

#endif // STORAGEHELPER_H
