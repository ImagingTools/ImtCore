#include <imtmdbx/Test/CMdbxTest.h>

#include <QtTest>

#include "CMask.h"
#include "CMaskContainer.h"

#include <iostream>
#include <QThread>
#include <QCoreApplication>


template <typename T>
T bit_count2 (T input) {
	T res = 0;
	//	static const unsigned shift_size = ((sizeof(T) * 8)-1);
	static const unsigned shift_size = 31;
	if(input < 0) {
		res ++;
		input &= ( (1 << shift_size) - 1 );
	}
	while( input ) {
		res += (input & 1);
		input >>= 1;
	}
	return res;
}

static const int lut[32] = {0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
							8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31};

static unsigned int bit_count(unsigned int v){
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;

	return lut[(unsigned int)(v * 0x07C4ACDDU) >> 27] + 1;
}


int BitsSetTable256[256];

// Function to initialise the lookup table
void initialize()
{

	// To initially generate the
	// table algorithmically
	BitsSetTable256[0] = 0;
	for (int i = 0; i < 256; i++)
	{
		BitsSetTable256[i] = (i & 1) +
				BitsSetTable256[i / 2];
	}
}

// Function to return the count
// of set bits in n
int bit_count3(int n)
{
	return (BitsSetTable256[n & 0xff] +
			BitsSetTable256[(n >> 8) & 0xff] +
			BitsSetTable256[(n >> 16) & 0xff] +
			BitsSetTable256[n >> 24]);
}

int bit_count4(qint64 n)
{
	return (BitsSetTable256[n & 0xff] +
			BitsSetTable256[(n >> 8) & 0xff] +
			BitsSetTable256[(n >> 16) & 0xff] +
			BitsSetTable256[(n >> 24) & 0xff] +
			BitsSetTable256[(n >> 32) & 0xff] +
			BitsSetTable256[(n >> 40) & 0xff] +
			BitsSetTable256[(n >> 48) & 0xff] +
			BitsSetTable256[n >> 56]);
}


CMdbxTest::CMdbxTest()
{
	std::string duration("3600000"); // 3600 seconds -> 1 h //120 - 20 min
	QByteArray timeoutDuration(duration.c_str(), static_cast<int>(duration.length()));
	qputenv("QTEST_FUNCTION_TIMEOUT", timeoutDuration);
	{ // just for checking ..
		auto result = qgetenv("QTEST_FUNCTION_TIMEOUT");
		qDebug() << "timeout set to:" << result << "ms";
	}
}

CMdbxTest::~CMdbxTest()
{

}

void CMdbxTest::initTestCase()
{
	qDebug("Called before everything else.");
}

void CMdbxTest::cleanupTestCase()
{
	qDebug("Called after myFirstTest and mySecondTest.");
}

void CMdbxTest::test_int()
{
	return;

	int k[64];
	int b[64];

	int n = bit_count(3);

	for (int cursor = 0; cursor < 64; cursor++){
		k[cursor] = 0;
		b[cursor] = 0xffffffff;
	}

	initialize();

	QBENCHMARK_ONCE {
		for (int i = 0; i < 1000000; i++){
			for (int cursor = 0; cursor < 64; cursor++){
				//				k[cursor] = k[cursor] & b[cursor];
				k[cursor] = k[cursor] + bit_count3(b[cursor]);
			}
		}
	}
}

void CMdbxTest::test_int64()
{
	return;

	qint64 k[64];
	qint64 b[64];
	QElapsedTimer time;
	time.start();
	QBENCHMARK_ONCE {
		for (int i = 0; i < 1000000; i++){
			for (int cursor = 0; cursor < 64; cursor++){
				k[cursor] = k[cursor] & bit_count4(b[cursor]);
			}
		}
	}
	qDebug() << "Time elapsed:" << time.elapsed();

}

void CMdbxTest::test_char()
{
	return;

	char k[256];
	char b[256];
	QBENCHMARK_ONCE {
		for (int i = 0; i < 1000000; i++){
			for (int cursor = 0; cursor < 256; cursor++){
				k[cursor] = k[cursor] & b[cursor];
			}
		}
	}
}

void CMdbxTest::test_mdbxfind()
{
	return;

	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "example.mdb";
	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write(false);
	QElapsedTimer time;

	mdbx::map_handle testHandle = txn.create_map("fap1", mdbx::key_mode::ordinal, mdbx::value_mode::single);
	qint64 key = 5;
	mdbx::slice keySlice(&key, 8);
	std::string value("value5");
	mdbx::slice valueSlice(value.data());
	txn.put(testHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
	key = 3;
	keySlice = mdbx::slice(&key, 8);
	value = std::string("value3");
	valueSlice = mdbx::slice(value.data());
	txn.put(testHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
	key = 1;
	keySlice = mdbx::slice(&key, 8);
	value = std::string("value1");
	valueSlice = mdbx::slice(value.data());
	txn.put(testHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
	key = 7;
	keySlice = mdbx::slice(&key, 8);
	value = std::string("value7");
	valueSlice = mdbx::slice(value.data());
	txn.put(testHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
	value = std::string("value7_");
	valueSlice = mdbx::slice(value);
	txn.put(testHandle, keySlice, valueSlice, mdbx::put_mode::upsert);

	txn.commit();
	txn = env.start_read();

	key = 3;
	keySlice = mdbx::slice(&key, 8);
	mdbx::cursor_managed cursor = txn.open_cursor(testHandle);
	mdbx::cursor::move_result result = cursor.find(keySlice);
	key = result.key.as_int64();
	result = cursor.to_next(false);
	key = result.key.as_int64();
	value = result.value.as_string();
	result = cursor.to_next(false);
	if (result.done){
		key = result.key.as_int64();
		value = result.value.as_string();
	}
	result = cursor.to_next(false);
	if (result.done){
		key = result.key.as_int64();
		value = result.value.as_string();
	}
	result = cursor.to_previous(false);
	if (result.done){
		key = result.key.as_int64();
		value = result.value.as_string();
	}
}


void CMdbxTest::test_mdbxcursor()
{
	return;

	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "example.mdb";
	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write(false);
	QElapsedTimer time;

	mdbx::map_handle testHandle = txn.create_map("fap1", mdbx::key_mode::ordinal, mdbx::value_mode::single);
	mdbx::cursor cursor = txn.open_cursor(testHandle);
	qint64 key = 5;
	mdbx::slice keySlice(&key, 8);
	std::string value("value5");
	mdbx::slice valueSlice(value.data());

	cursor.upsert(keySlice, valueSlice);
	key = 3;
	keySlice = mdbx::slice(&key, 8);
	value = std::string("value3");
	valueSlice = mdbx::slice(value.data());
	cursor.upsert(keySlice, valueSlice);
	key = 1;
	keySlice = mdbx::slice(&key, 8);
	value = std::string("value1");
	valueSlice = mdbx::slice(value.data());
	cursor.upsert(keySlice, valueSlice);
	key = 7;
	keySlice = mdbx::slice(&key, 8);
	value = std::string("value7");
	valueSlice = mdbx::slice(value.data());
	cursor.upsert(keySlice, valueSlice);
	value = std::string("value7_");
	valueSlice = mdbx::slice(value);
	cursor.upsert(keySlice, valueSlice);

	txn.commit();
	txn = env.start_read();
	cursor = txn.open_cursor(testHandle);

	key = 3;
	keySlice = mdbx::slice(&key, 8);
	mdbx::cursor::move_result result = cursor.find(keySlice);
	key = result.key.as_int64();
	result = cursor.to_next(false);
	key = result.key.as_int64();
	value = result.value.as_string();
	result = cursor.to_next(false);
	if (result.done){
		key = result.key.as_int64();
		value = result.value.as_string();
	}
	result = cursor.to_next(false);
	if (result.done){
		key = result.key.as_int64();
		value = result.value.as_string();
	}
	result = cursor.to_previous(false);
	if (result.done){
		key = result.key.as_int64();
		value = result.value.as_string();
	}
}


void CMdbxTest::test_mdbxmap()
{
	return;

	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "example.mdb";

	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write(true);
	QElapsedTimer time;

	mdbx::map_handle testHandle = txn.create_map("map", mdbx::key_mode::ordinal, mdbx::value_mode::single);
	time.start();
	for (qint64 i = 1; i < 10000; i++){

		//		if (i%100000 == 0){
		//			std::string mapName = "map" + QString::number(i/100000).toStdString();
		//			txn.commit();
		//			txn = env.start_write(false);
		//			env.close_map(testHandle);
		//			testHandle = txn.create_map(mapName, mdbx::key_mode::ordinal, mdbx::value_mode::single);

		//			std::cout << QString::number(i).toStdString()  << " " << "for 100 tho" << " " << time.elapsed() << std::endl;
		//		}
		if (i%1000000 == 0){
			std::cout << QString::number(i/1000000).toStdString()  << " " << "for mln" << " " << time.elapsed() << std::endl;
		}

		std::string key("key");
		key += std::to_string(i);
		std::string value("value");
		value += std::to_string(i);
		//		mdbx::slice keySlice(key.data(), key.size());
		mdbx::slice keySlice(&i, 8);
		//		char testMask[16];
		//		mdbx::slice valueSlice(value.data());
		//		mdbx::slice valueSlice(testMask, 16);
		//		value += "_1";
		mdbx::slice valueSlice(value.data());
		txn.put(testHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
		//		txn.commit();
		//		txn = env.start_write(false);
		//		env.close_map(testHandle);
		//		testHandle = txn.create_map("fap1", mdbx::key_mode::ordinal, mdbx::value_mode::single);

		//		txn.insert(testHandle, keySlice, valueSlice);
		//		txn.erase(testHandle, keySlice);
		//		txn.upsert(testHandle, keySlice, valueSlice_1);
		if(i%1 == 0){
			std::string mapName = "map" + QString::number(i/1).toStdString();
			std::cout << mapName  << " " << time.elapsed() << std::endl;
			time.restart();

			//            uint64_t id = txn.id();
			//MDBX_txn_info info = txn.get_info();
			mdbx::txn_managed writeTxn;

			//            id = writeTxn.id();

			txn.commit();

			//info = txn.get_info();

			txn = env.start_write(false);
			env.close_map(testHandle);
			testHandle = txn.create_map(mapName, mdbx::key_mode::ordinal, mdbx::value_mode::single);
		}
	}
	txn.commit();

	qDebug() << "map finish";
	qDebug() << "Time make elapsed:" << time.elapsed();
	txn.abort();
	env.close();
}


void CMdbxTest::test_mdbx()
{
	return;

	//	/* Create and open the LMDB environment: */
	//	mdbx::env env;
	////	env.set set_mapsize(1UL * 1024UL * 1024UL * 1024UL); /* 1 GiB */
	//	env.open("./example.mdb", 0, 0664);

	// for simplify the set<pair<key,value>>
	// is used instead of multimap<key,value>
	//	using SET = std::set<Item, ItemCompare>;
	//		QBENCHMARK_ONCE {

	mdbx::env::operate_parameters operateParameters(10,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "example.mdb";
	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write(false);
	mdbx::map_handle testHandle = txn.create_map("test", mdbx::key_mode::reverse, mdbx::value_mode::single);
	QElapsedTimer time;
	time.start();
	for (qint64 i = 1000000; i < 1000000; i++){
		std::string key("key");
		key += std::to_string(i);
		std::string value("value");
		value += std::to_string(i*1000);
		//		mdbx::slice keySlice(key.data(), key.size());
		mdbx::slice keySlice(&i, 8);
		char testMask[256];
		//		mdbx::slice valueSlice(value.data());
		mdbx::slice valueSlice(testMask, 256);
		//		value += "_1";
		//		mdbx::slice valueSlice_1(value.data());
		txn.put(testHandle, keySlice, valueSlice, mdbx::put_mode::upsert);
		//		txn.insert(testHandle, keySlice, valueSlice);
		//		txn.erase(testHandle, keySlice);
		//		txn.upsert(testHandle, keySlice, valueSlice_1);
		if(i%100000 == 0){
			qDebug() << QString::number(i) << time.elapsed();
			time.restart();
			txn.commit();
			txn = env.start_write(false);
			testHandle = txn.create_map("test", mdbx::key_mode::reverse, mdbx::value_mode::single);
		}
	}

	//	for (qint64 i = 0; i < 1000000; i++){
	//		mdbx::slice keySlice(&i, 8);
	//		txn.erase(testHandle, keySlice);
	//		if(i%100000 == 0){
	//			qDebug() << QString::number(i) << time.elapsed();
	//			time.restart();
	//			txn.commit();
	//			txn = env.start_write(false);
	//			testHandle = txn.create_map("test", mdbx::key_mode::reverse, mdbx::value_mode::multi);
	//		}
	//	}
	txn.commit();
	txn = env.start_read();

	mdbx::cursor_managed cursor = txn.open_cursor(testHandle);
	qint64 key = 1000;
	mdbx::slice keySlice(&key, 8);
	mdbx::slice valueSlice;
	//	if (cursor.seek(keySlice)){
	////		cursor.move(mdbx::cursor::multi_currentkey_firstvalue, keySlice, valueSlice);
	////		valueSlice = cursor.txn().get(testHandle, keySlice);

	//	}
	//	mdbx::cursor::move_result result = cursor.find(keySlice);
	//	qDebug() << result.value.as_string().data();
	//	result = cursor.to_current_next_multi();
	//	qDebug() << result.value.as_string().data();
	qDebug() << "Start read masks";
	time.restart();
	qint64 b[32];
	for (qint64 i = 0; i < 48000; i++){
		mdbx::slice keySlice(&i, 8);
		//		cursor.seek(keySlice);
		//		mdbx::slice result = cursor.txn().get(testHandle, keySlice);
		//		qint64* k = static_cast<qint64*>(result.data());

		mdbx::cursor::move_result result = cursor.find(keySlice);
		qint64* k = static_cast<qint64*>(result.value.data());
		for (int cursor = 0; cursor < 32; cursor++){
			k[cursor] = k[cursor] & b[cursor];
		}
		//		if(i%100000 == 0){
		//			qDebug() << QString::number(i) << time.elapsed();
		//			time.restart();
		//		}
	}
	qDebug() << "Time make elapsed:" << time.elapsed();
	cursor.close();
	txn.abort();
	env.close();
	// }
}


/************************************************************/

void CMdbxTest::test_read_all_units(){

	return;

	std::cout << "TEST_read_all_units!!!";

	bool isIndex = false;

	QString tableNameBase = "Units10";
	const QString tableName = !isIndex ? tableNameBase : tableNameBase + "Index";
	QString dataBaseName = tableNameBase + ".mdb";

	QElapsedTimer time;

	time.start();

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine(tableNameBase + ".mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_read();

	mdbx::map_handle mapHandleRead = txn.open_map(tableName.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);
	mdbx::cursor_managed cursorRead = txn.open_cursor(mapHandleRead);
	mdbx::cursor::move_result result = cursorRead.to_first();
	int count = 0;
	while(1){
		qint64 keyRead;
		std::string valueRead;
		std::string keyReadIndex;
		qint64 valueReadIndex;

		if (result.done){
			if(!isIndex){
				keyRead = result.key.as_int64();
				valueRead = result.value.as_string();

				if(count%1 == 0){
					qDebug() << "Итерация " << count << " READING: " << keyRead << " " << valueRead << "\n";
				}
			}
			else {
				keyReadIndex = result.key.as_string();
				valueReadIndex = result.value.as_int64();
				if(count%1 == 0){
					qDebug() << "Итерация " << count << " READING:INDEX " << keyReadIndex << " " << valueReadIndex << "\n";
				}
			}
		}
		bool onLast = cursorRead.on_last();
		if(onLast){
			break;
		}
		if(!onLast){
			result = cursorRead.to_next();
		}

		count++;
	}

	qDebug() << "Time read elapsed:" << time.elapsed();
}


void CMdbxTest::test_write_units(){

	return; //!!!!!!!!!!!!!!!!!!!!!!!!!

	std::cout << "TEST_write_units!!!";

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine("Units.mdb");

	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();
	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable("Units", txn);

	QElapsedTimer time;

	time.start();

//	documentTable.StartWriteProcess();

	for (qint64 i = 1; i < 1000000; i++){
		QString data = "unit";
		data += QString::number(i);
		documentTable.AddDocument(data.toLocal8Bit());
		if(i%1000 == 0){
			qDebug() << "Итерация " << i << "\n";
		}
	}
	txn.commit();
//	documentTable.EndWriteProcess();

	qDebug() << "map finish";
	qDebug() << "Time make elapsed:" << time.elapsed();

}

void CMdbxTest::test_write_10units(){

	return;

	std::cout << "TEST_write_10units!!!";

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine("Units10.mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable("Units10", txn);


//		documentTable.StartWriteProcess();

	for (qint64 i = 1; i < 11; i++){
		QString data = "unit";
		data += QString::number(i);
		documentTable.AddDocument(data.toLocal8Bit());

	}
	txn.commit();
//		documentTable.EndWriteProcess();

}

void CMdbxTest::test_write_10unitsInt(){

	return;

	std::cout << "TEST_write_10unitsInt!!!";

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine("Units10Int.mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable("Units10Int", txn);

	//	documentTable.StartWriteProcess();

	for (qint64 i = 1; i < 11; i++){
		documentTable.AddDocument(i);
	}
	txn.commit();
	//	documentTable.EndWriteProcess();

}

void CMdbxTest::test_create_index(){

	return;

	std::cout << "TEST_create_index!!!";

	QString tableName = "Units10";

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, txn);
	bool ok = documentTable.CreateIndex();
	qDebug() << "CreateIndex :: " << ok;
	txn.commit();

}

void CMdbxTest::test_get_doc(){

	return;

	std::cout << "TEST_get_doc!!!";

	QString tableName = "Units10";
	QString dataBaseName = tableName + ".mdb";

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_read();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, txn);

	qint64 key = 2;
	QByteArray result  = documentTable.GetDocument(key);

	qDebug() << "RESULT:: " << QString(result);

}

void CMdbxTest::test_get_key(){

	return;

	std::cout << "TEST_get_key!!!";

	QString tableName = "Units10";

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_read();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, txn);

	QByteArray value = "unit6";
	qint64 key  = documentTable.GetKey(value);

	qDebug() << "KEY:: " << key;

}

void CMdbxTest::test_update_doc(){

	return;

	qDebug() << "test_update_doc";

	QString tableName = "Units10";
	qint64 index = 9;

	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, txn);
	documentTable.UpdateDocument(index, "unitNEW_9");
	txn.commit();

}



class ThreadTest: public QObject
{
	Q_OBJECT

public Q_SLOTS:
	void on_WriteRequest() {

		qDebug() << "ThreadTest";

		QString tableName = "Units10";

		qint64 key = 3;
		std::string data = "NEW_VALUE!!!";
		mdbx::slice keySlice(&key, 8);
		mdbx::slice valueSlice(data.data(), data.length());

		imtmdbx::CDatabaseEngine engineWrite = imtmdbx::CDatabaseEngine(tableName + ".mdb");
		mdbx::env_managed envWrite = engineWrite.GetEnv();
		mdbx::txn_managed txnWrite = envWrite.start_write();
		mdbx::map_handle mapHandleWrite = txnWrite.open_map(tableName.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);
		//mdbx::cursor_managed cursor = txnWrite.open_cursor(mapHandleWrite);
		//mdbx::cursor::move_result result = cursor.to_last(false);

		//txnWrite.put(mapHandleWrite, keySlice, valueSlice, mdbx::put_mode::upsert);
		txnWrite.update(mapHandleWrite, keySlice, valueSlice);
		txnWrite.commit();
	}
};

class ThreadController: public QObject
{
	Q_OBJECT
	QThread thread;
public:
	ThreadController()
	{
		ThreadTest *testObject = new ThreadTest;
		connect(&thread, &QThread::finished, testObject, &QObject::deleteLater);
		connect(this, &ThreadController::write, testObject, &ThreadTest::on_WriteRequest);

		testObject->moveToThread(&thread);
		thread.start();
	}

signals:
	void write();

};

void CMdbxTest::test_read_write(){

	return;

	std::cout << "TEST_TEST!!!";

	/*************************************************************/
	QString tableName = "Units10";

	imtmdbx::CDatabaseEngine engineRead = imtmdbx::CDatabaseEngine(tableName + ".mdb");
	//imtmdbx::CDatabaseEngine engineWrite = imtmdbx::CDatabaseEngine(tableName + ".mdb");

	mdbx::env_managed envRead = engineRead.GetEnv();
	mdbx::txn_managed txnRead = envRead.start_read();


	//thread
	int argc = 0; char *argv = {};
	QCoreApplication app(argc, &argv);

	ThreadController controller;
	emit controller.write();

	//waiting
	QDateTime currTime = QDateTime::currentDateTime();
	QDateTime time = QDateTime::currentDateTime();
	QDateTime add30Sec = currTime;
	add30Sec = add30Sec.addSecs(5);

	while(time < add30Sec){
		time = QDateTime::currentDateTime();
	}
	qDebug() << "Time:: " << time.toString();

	//reading
	qint64 key = 3;
	mdbx::slice keySlice(&key, 8);
	std::string data = "newNew";

	mdbx::map_handle mapHandleRead = txnRead.open_map(tableName.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);
	mdbx::cursor_managed cursorRead = txnRead.open_cursor(mapHandleRead);

	mdbx::cursor::move_result resultRead = cursorRead.find(keySlice);
	std::string value = resultRead.value.as_string();

	qDebug() << "VALUE:: " << value;



	/*************************************************************/
//		imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine(tableName + ".mdb");
//		imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, engine, false);

//		documentTable.UpdateDocument(3, "unitNEW");
//		documentTable.SetIsRead(true);
//		QByteArray newVal = documentTable.GetDocument(3);
//		qDebug() << "NEW_VALUE:: " << newVal;

}

class TreadMask: public QThread
{
	Q_OBJECT
public:
	TreadMask(imtmdbx::CDatabaseEngine& engine):
		m_engine(engine)
	{
		start();
	}

protected:
	void run() override
	{

	}

	imtmdbx::CDatabaseEngine& m_engine;
};

void CMdbxTest::test_write_masks(){

	//return;

	std::cout << "TEST_WRITE_MASKS!!!";

	/*************************************************************/
	QString tableName = "Masks";
	imtmdbx::CDatabaseEngine engine = imtmdbx::CDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();
	QString maskNameMln = "Mask1mln";
	QString maskName1000 = "Mask1000";

	{
		imtmdbx::CMask mask1mln(maskNameMln, txn);
		QElapsedTimer time;
		time.start();
		for(int i = 0; i < 1000000; i++){
//			if (i%10 != 0){
				mask1mln.SetUnit(i,true);
//			}
			if (i%10000 == 0){
				std::cout << "Write i: " << i / 10000 << " " << time.elapsed() << std::endl;
			}
		}
		std::cout << "Write 1mln: " << time.elapsed() << std::endl;
	}

	{
		imtmdbx::CMask mask1000(maskName1000, txn);
		QElapsedTimer time;
		time.start();
		for(int i = 0; i < 1000000; i++){
			if (i%1000 == 0){
				mask1000.SetUnit(i,true);
			}
			if (i%10000 == 0){
				std::cout << "Write i: " << i / 10000 << " " << time.elapsed() << std::endl;
			}
		}
		std::cout << "Write 1000: " << time.elapsed() << std::endl;
	}
	txn.commit();
	txn = env.start_read();

	{
		imtmdbx::CMask mask1mln(maskNameMln, txn);
		imtmdbx::CMask mask1000(maskName1000, txn);
//		quint64 unit =  mask1mln.GetUnit(2);

		imtmdbx::CMaskContainer container(imtmdbx::CMaskContainer::OT_AND);
		container.AddMask(&mask1mln, false);
		//container.AddMask(&mask1000, false);

		QElapsedTimer time;
		time.start();
		int count = container.GetUnitCount();
		QList<quint64> result;
		result = container.GetUnitPositions(0,1000000);

		std::cout << "Unit count: " << count << " result count:" << result.count() << " time:" << time.elapsed() << std::endl;
	}


}


I_ADD_TEST(CMdbxTest);
