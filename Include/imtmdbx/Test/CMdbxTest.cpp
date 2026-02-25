// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/Test/CMdbxTest.h>

#include <QtTest>

// ImtCore includes
#include <imtmdbx/CMask.h>
#include <imtmdbx/CMaskContainer.h>
#include <imtmdbx/CDoubleMask.h>
#include <imtmdbx/CMdbxDatabaseEngine.h>

#include <iostream>
//#include <QThread>
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

int bit_count4(quint64 n)
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


void CMdbxTest::test_delete_append()
{
	qDebug() << "TEST_DELETE_APPEND";
	std::string path = "TEST_DELETE_APPEND.mdb";

	//removing test mdbx database file
	QString filePath = QString::fromStdString(path);
	QFile file(filePath);
	if(file.exists()){
		file.remove();
	}

	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;

	mdbx::env_managed env(path, createParameters, operateParameters);

	//MAIN PARAMS
	int multiCoeff = 3;//DEFAULT: = 1
	int count = 1000;
	int maxLength = QString::number(count).length();
	int updateInterval = 100;//10
	bool commitEveryTime = false;
	bool commitAfterEveryDeletion = false;

	//WRITING:
	if(1){
		mdbx::txn_managed txn = env.start_write();
		imtmdbx::CDocumentTable testTable("Test",
										  txn,
										  mdbx::key_mode::usual,
										  mdbx::value_mode::multi,
										  true);

		int writeCount = 0;
		for(int i = 0; i < count; i++){
			QString str = QString::number(i+1);

			for(int j = 0; j < multiCoeff; j++){//for multi value
				int length = str.length();
				for(int k = 0; k < maxLength - length; k++){
					str = str.prepend("0");
				}
				QString key = str;
				QString doc = QString("Doc_") + str;
				if(j > 0){
					QString add = "_" + QString::number(j);
					doc = doc.append(add);
				}
				testTable.AddDocument(key.toLocal8Bit(), doc.toLocal8Bit());
				writeCount++;
			}
		}
		txn.commit();
		qDebug() << "WRITE_COUNT:: " << writeCount;
	}

	//UPDATING:
	if(!commitAfterEveryDeletion){//do not commit after deletion
		mdbx::txn_managed txn = env.start_write();
		imtmdbx::CDocumentTable* testTable = new imtmdbx::CDocumentTable("Test",
																		 txn,
																		 mdbx::key_mode::usual,
																		 mdbx::value_mode::multi,
																		 true);

		for(int i = 0; i < count; i++){

			if((i+1) % updateInterval == 0){
				QString str = QString::number(i+1);

				int length = str.length();
				for(int j = 0; j < maxLength - length; j++){
					str = str.prepend("0");
				}
				QString doc = QString("Doc_") + str;
				if(multiCoeff > 1){//for multi value
					doc = doc.append("_1");
				}

				QString new_key = QString("_") + str;

				testTable->MoveToValue(doc.toLocal8Bit());
				testTable->RemoveDocument();
				testTable->AddDocument(new_key.toLocal8Bit(), doc.toLocal8Bit());

				if(commitEveryTime){
					delete testTable;
					txn.commit();
					txn = env.start_write();
					testTable = new imtmdbx::CDocumentTable("Test",
															txn,
															mdbx::key_mode::usual,
															mdbx::value_mode::multi,
															true);

				}

			}
		}
		delete testTable;
		txn.commit();
	}

	//UPDATING: 2
	if(commitAfterEveryDeletion){//commit after deletion
		mdbx::txn_managed txn = env.start_write();
		imtmdbx::CDocumentTable* testTable = new imtmdbx::CDocumentTable("Test",
																		 txn,
																		 mdbx::key_mode::usual,
																		 mdbx::value_mode::multi,
																		 true);

		for(int i = 0; i < count; i++){//deletion
			if((i+1) % updateInterval == 0){
				QString str = QString::number(i+1);

				int length = str.length();
				for(int j = 0; j < maxLength - length; j++){
					str = str.prepend("0");
				}
				QString doc = QString("Doc_") + str;
				if(multiCoeff > 1){//for multi value
					doc = doc.append("_1");
				}

				testTable->MoveToValue(doc.toLocal8Bit());
				testTable->RemoveDocument();

				delete testTable;
				txn.commit();
				txn = env.start_write();
				testTable = new imtmdbx::CDocumentTable("Test",
														txn,
														mdbx::key_mode::usual,
														mdbx::value_mode::multi,
														true);

			}
		}//deletion

		for(int i = 0; i < count; i++){//add document
			if((i+1) % updateInterval == 0){
				QString str = QString::number(i+1);

				int length = str.length();
				for(int j = 0; j < maxLength - length; j++){
					str = str.prepend("0");
				}
				QString doc = QString("Doc_") + str;
				if(multiCoeff > 1){//for multi value
					doc = doc.append("_1");
				}

				QString new_key = str.prepend("_");
				testTable->AddDocument(new_key.toLocal8Bit(), doc.toLocal8Bit());
			}
		}//add document

		delete testTable;
		txn.commit();
	}

	//READING:
	if(1){
		int readCount = 0;
		mdbx::txn_managed txn = env.start_write();
		imtmdbx::CDocumentTable testTable("Test",
										  txn,
										  mdbx::key_mode::usual,
										  mdbx::value_mode::multi,
										  true);

		bool isReady = testTable.MoveToFirst();
		while(isReady){
			readCount++;
			QByteArray key = testTable.GetKeyBA();
			QByteArray doc = testTable.GetDocument();
			//qDebug() << "TEST_:: " << "Key:: " << key << "doc::" << doc;
			std::cout << "TEST_:: " << " Key:: " << key.data() << " doc::" << doc.data() << "\n";
			isReady = testTable.MoveToNext();
		}
		std::cout << "READ_COUNT:: " << readCount << "\n";
	}


	std::cout << "TEST_DELETE_APPEND::FINISHED!";
}


void CMdbxTest::initTestCase()
{
	//qDebug("Called before everything else.");
}

void CMdbxTest::cleanupTestCase()
{
	//qDebug("Called after myFirstTest and mySecondTest.");
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

	quint64 k[64];
	quint64 b[64];
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

	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "example.mdb";
	{
		mdbx::env_managed env2(path, createParameters, operateParameters);
		mdbx::txn_managed txn2 = env2.start_write(false);
		mdbx::map_handle testHandle2 = txn2.create_map("fap1", mdbx::key_mode::reverse, mdbx::value_mode::single);
		txn2.commit();
	}
	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write(false);
	mdbx::map_handle testHandle = txn.create_map("fap1", mdbx::key_mode::usual, mdbx::value_mode::single);


	//	return;

	quint64 key = 5;
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
	key = result.key.as_uint64();
	result = cursor.to_next(false);
	key = result.key.as_uint64();
	value = result.value.as_string();
	result = cursor.to_next(false);
	if (result.done){
		key = result.key.as_uint64();
		value = result.value.as_string();
	}
	result = cursor.to_next(false);
	if (result.done){
		key = result.key.as_uint64();
		value = result.value.as_string();
	}
	result = cursor.to_previous(false);
	if (result.done){
		key = result.key.as_uint64();
		value = result.value.as_string();
	}
}


void CMdbxTest::test_mdbxcursor()
{
	// return;

	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "example.mdb";
	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write(false);
	QElapsedTimer time;

	mdbx::map_handle testHandle = txn.create_map("fap1", mdbx::key_mode::usual, mdbx::value_mode::single);
	mdbx::cursor cursor = txn.open_cursor(testHandle);
	QString name("Иван");
	QByteArray data;
	QDataStream ds(&data, QIODeviceBase::WriteOnly);
	ds << (quint8)0;
	data += name.toUtf8();

	mdbx::slice keySlice(data.data(), data.size());
	// quint64 key = 5;
	// mdbx::slice keySlice(&key, 8);

	std::string value("value0");

	mdbx::slice valueSlice(value.data());

	cursor.upsert(keySlice, valueSlice);
	// key = 3;
	// keySlice = mdbx::slice(&key, 8);
	name = QString("Сергей");
	data.resize(1);
	data += name.toUtf8();
	keySlice = mdbx::slice(data.data(), data.size());

	value = std::string("value3");
	valueSlice = mdbx::slice(value.data());

	cursor.upsert(keySlice, valueSlice);
	// key = 1;
	// keySlice = mdbx::slice(&key, 8);
	name = QString("Петр");
	data.resize(1);
	data += name.toUtf8();
	keySlice = mdbx::slice(data.data(), data.size());
	value = std::string("value1");
	valueSlice = mdbx::slice(value.data());
	cursor.upsert(keySlice, valueSlice);

	name = QString("Антон");
	data.resize(1);
	data += name.toUtf8();
	keySlice = mdbx::slice(data.data(), data.size());

	// key = 7;
	// keySlice = mdbx::slice(&key, 8);
	value = std::string("value7");
	valueSlice = mdbx::slice(value.data());
	cursor.upsert(keySlice, valueSlice);
	value = std::string("value7_");
	valueSlice = mdbx::slice(value);
	cursor.upsert(keySlice, valueSlice);

	txn.commit();
	txn = env.start_read();
	cursor = txn.open_cursor(testHandle);

	mdbx::cursor::move_result result = cursor.to_first();
	std::string resKey;
	resKey = result.key.as_string();

	result = cursor.to_next();
	resKey = result.key.as_string();

	// key = 3;
	// keySlice = mdbx::slice(&key, 8);
	// mdbx::cursor::move_result result = cursor.find(keySlice);
	// key = result.key.as_uint64();
	// result = cursor.to_next(false);
	// key = result.key.as_uint64();
	// value = result.value.as_string();
	// result = cursor.to_next(false);
	// if (result.done){
	// 	key = result.key.as_uint64();
	// 	value = result.value.as_string();
	// }
	// result = cursor.to_next(false);
	// if (result.done){
	// 	key = result.key.as_uint64();
	// 	value = result.value.as_string();
	// }
	// result = cursor.to_previous(false);
	// if (result.done){
	// 	key = result.key.as_uint64();
	// 	value = result.value.as_string();
	// }
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
	for (quint64 i = 1; i < 10000; i++){

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
	for (quint64 i = 1000000; i < 1000000; i++){
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

	//	for (quint64 i = 0; i < 1000000; i++){
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
	quint64 key = 1000;
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
	quint64 b[32];
	for (quint64 i = 0; i < 48000; i++){
		mdbx::slice keySlice(&i, 8);
		//		cursor.seek(keySlice);
		//		mdbx::slice result = cursor.txn().get(testHandle, keySlice);
		//		quint64* k = static_cast<quint64*>(result.data());

		mdbx::cursor::move_result result = cursor.find(keySlice);
		quint64* k = static_cast<quint64*>(result.value.data());
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

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine(tableNameBase + ".mdb");
	mdbx::env_managed& env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_read();

	mdbx::map_handle mapHandleRead = txn.open_map(tableName.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);
	mdbx::cursor_managed cursorRead = txn.open_cursor(mapHandleRead);
	mdbx::cursor::move_result result = cursorRead.to_first();
	int count = 0;
	while(1){
		quint64 keyRead;
		std::string valueRead;
		std::string keyReadIndex;
		quint64 valueReadIndex;

		if (result.done){
			if(!isIndex){
				keyRead = result.key.as_uint64();
				valueRead = result.value.as_string();

				if(count%1 == 0){
					qDebug() << "Итерация " << count << " READING: " << keyRead << " " << valueRead << "\n";
				}
			}
			else {
				keyReadIndex = result.key.as_string();
				valueReadIndex = result.value.as_uint64();
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

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine("Units.mdb");

	mdbx::env_managed& env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();
	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable("Units", txn);

	QElapsedTimer time;

	time.start();

	//	documentTable.StartWriteProcess();

	for (quint64 i = 1; i < 1000000; i++){
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

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine("Units10.mdb");
	mdbx::env_managed& env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable("Units10", txn);


	//		documentTable.StartWriteProcess();

	for (quint64 i = 1; i < 11; i++){
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

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine("Units10Int.mdb");
	mdbx::env_managed& env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable("Units10Int", txn);

	//	documentTable.StartWriteProcess();

	for (quint64 i = 1; i < 11; i++){
		documentTable.AddDocument(i);
	}
	txn.commit();
	//	documentTable.EndWriteProcess();

}

void CMdbxTest::test_create_index(){

	return;

	std::cout << "TEST_create_index!!!";

	QString tableName = "Units10";

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed& env = engine.GetEnv();
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

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed& env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_read();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, txn);

	quint64 key = 2;
	QByteArray result  = documentTable.GetDocument(key);

	qDebug() << "RESULT:: " << QString(result);

}

void CMdbxTest::test_get_key(){

	return;

	std::cout << "TEST_get_key!!!";

	QString tableName = "Units10";

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed& env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_read();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, txn);

	QByteArray value = "unit6";
	quint64 key;
	documentTable.GetKey(key, value);

	qDebug() << "KEY:: " << key;

}

void CMdbxTest::test_update_doc(){

	return;

	qDebug() << "test_update_doc";

	QString tableName = "Units10";
	quint64 index = 9;

	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
	mdbx::env_managed& env = engine.GetEnv();
	mdbx::txn_managed txn = env.start_write();

	imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, txn);
	documentTable.UpdateDocument(index, "unitNEW_9");
	txn.commit();

}



//class ThreadTest: public QObject
//{
//	Q_OBJECT

//public Q_SLOTS:
//	void on_WriteRequest() {

//		qDebug() << "ThreadTest";

//		QString tableName = "Units10";

//		quint64 key = 3;
//		std::string data = "NEW_VALUE!!!";
//		mdbx::slice keySlice(&key, 8);
//		mdbx::slice valueSlice(data.data(), data.length());

//		imtmdbx::CMdbxDatabaseEngine engineWrite = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
//		mdbx::env_managed envWrite = engineWrite.GetEnv();
//		mdbx::txn_managed txnWrite = envWrite.start_write();
//		mdbx::map_handle mapHandleWrite = txnWrite.open_map(tableName.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);
//		//mdbx::cursor_managed cursor = txnWrite.open_cursor(mapHandleWrite);
//		//mdbx::cursor::move_result result = cursor.to_last(false);

//		//txnWrite.put(mapHandleWrite, keySlice, valueSlice, mdbx::put_mode::upsert);
//		txnWrite.update(mapHandleWrite, keySlice, valueSlice);
//		txnWrite.commit();
//	}
//};

//class ThreadController: public QObject
//{
//	Q_OBJECT
//	QThread thread;
//public:
//	ThreadController()
//	{
//		ThreadTest *testObject = new ThreadTest;
//		connect(&thread, &QThread::finished, testObject, &QObject::deleteLater);
//		connect(this, &ThreadController::write, testObject, &ThreadTest::on_WriteRequest);

//		testObject->moveToThread(&thread);
//		thread.start();
//	}

//signals:
//	void write();

//};

//void CMdbxTest::test_read_write(){

//	return;

//	std::cout << "TEST_TEST!!!";

//	/*************************************************************/
//	QString tableName = "Units10";

//	imtmdbx::CMdbxDatabaseEngine engineRead = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
//	//imtmdbx::CMdbxDatabaseEngine engineWrite = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");

//	mdbx::env_managed envRead = engineRead.GetEnv();
//	mdbx::txn_managed txnRead = envRead.start_read();


//	//thread
//	int argc = 0; char *argv = {};
//	QCoreApplication app(argc, &argv);

//	ThreadController controller;
//	emit controller.write();

//	//waiting
//	QDateTime currTime = QDateTime::currentDateTime();
//	QDateTime time = QDateTime::currentDateTime();
//	QDateTime add30Sec = currTime;
//	add30Sec = add30Sec.addSecs(5);

//	while(time < add30Sec){
//		time = QDateTime::currentDateTime();
//	}
//	qDebug() << "Time:: " << time.toString();

//	//reading
//	quint64 key = 3;
//	mdbx::slice keySlice(&key, 8);
//	std::string data = "newNew";

//	mdbx::map_handle mapHandleRead = txnRead.open_map(tableName.toStdString(), mdbx::key_mode::reverse, mdbx::value_mode::single);
//	mdbx::cursor_managed cursorRead = txnRead.open_cursor(mapHandleRead);

//	mdbx::cursor::move_result resultRead = cursorRead.find(keySlice);
//	std::string value = resultRead.value.as_string();

//	qDebug() << "VALUE:: " << value;



//	/*************************************************************/
////		imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
////		imtmdbx::CDocumentTable documentTable = imtmdbx::CDocumentTable(tableName, engine, false);

////		documentTable.UpdateDocument(3, "unitNEW");
////		documentTable.SetIsRead(true);
////		QByteArray newVal = documentTable.GetDocument(3);
////		qDebug() << "NEW_VALUE:: " << newVal;

//}

//class TreadMask: public QThread
//{
//	Q_OBJECT
//public:
//	TreadMask(imtmdbx::CMdbxDatabaseEngine& engine):
//		m_engine(engine)
//	{
//		start();
//	}

//protected:
//	void run() override
//	{

//	}

//	imtmdbx::CMdbxDatabaseEngine& m_engine;
//};

void CMdbxTest::test_write_masks(){
	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "example.mdb";
	{
		mdbx::env_managed env2(path, createParameters, operateParameters);
		mdbx::txn_managed txn2 = env2.start_write();
		mdbx::map_handle testHandle2 = txn2.create_map("fap1", mdbx::key_mode::reverse, mdbx::value_mode::single);
		txn2.commit();
	}
	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write();

	mdbx::map_handle mapHandle = txn.open_map(0);
	mdbx::cursor_managed cursor = txn.open_cursor(mapHandle);
	mdbx::slice keySlice("fap1");
	// mdbx::cursor::move_result result = cursor.find(keySlice, false);
	bool ok = cursor.seek(keySlice);

	mdbx::map_handle testHandle = txn.create_map("fap1", mdbx::key_mode::usual, mdbx::value_mode::single);

	std::cout << "TEST_WRITE_MASKS!!!";

	// return;

	/*************************************************************/
	QString tableName = "Masks";
	imtmdbx::CMdbxDatabaseEngine engine = imtmdbx::CMdbxDatabaseEngine(tableName + ".mdb");
	// mdbx::env_managed env = engine.GetEnv();
	// mdbx::txn_managed txn = env.start_write();
	QString maskNameMln = "Mask1mln";
	QString maskName1000 = "Mask1000";
	QString doubleMaskName = "DoubleMask";

	{
		imtmdbx::CMask mask1mln(maskNameMln, txn);
		QElapsedTimer time;
		time.start();
		for(int i = 0; i < 1000000; i++){
			if ((i < 250000 || i > 750000)){
				mask1mln.SetUnit(i,true);
			}
			if (i%10000 == 0){
				std::cout << "Write i: " << i / 10000 << " " << time.elapsed() << std::endl;
			}
		}
		std::cout << "Write 1mln: " << time.elapsed() << std::endl;
	}
	txn.commit();
	txn = env.start_write();

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
	txn = env.start_write();
	{
		for (int j = 0; j < 3; j++){
			imtmdbx::CDoubleMask doubleMask(doubleMaskName + QString::number(5+j), txn, 5 + j);
			QElapsedTimer time;
			time.start();
			for(int i = 3; i < 10; i++){
				//			if (i%1000 == 0){
				doubleMask.SetUnit(i,true);
				//			}
				if (i%10000 == 0){
					std::cout << "Write i: " << i / 10000 << " " << time.elapsed() << std::endl;
				}
			}

			bool unit = doubleMask.GetUnit(5);
			std::cout << "Write 1000: " << time.elapsed() << std::endl;
		}
	}

	txn.commit();
	txn = env.start_read();

	{
		imtmdbx::CMask mask1mln(maskNameMln, txn);
		imtmdbx::CMask mask1000(maskName1000, txn);
		imtmdbx::CDoubleMask doubleMask(doubleMaskName + QString::number(6), txn, 6);
		//		quint64 unit =  mask1mln.GetUnit(2);
		bool unit = doubleMask.GetUnit(5);

		imtmdbx::CMaskContainer container(imtmdbx::CMaskContainer::OT_OR);
		container.AddMask(&mask1mln, false);
		//		container.AddMask(&doubleMask, false);
		container.AddMask(&mask1000, false);

		QElapsedTimer time;
		time.start();
		imtmdbx::getItemCount = 0;
		int count = 0;
		for (int i = 0; i < 1; i++){
			count += container.GetUnitCount();
		}
		std::cout << "Get item count: " << imtmdbx::getItemCount << std::endl;

		QList<quint64> result;
		result = container.GetUnitPositions(0,10);

		std::cout << "Unit count: " << count << " result count:" << result.count() << " time:" << time.elapsed() << std::endl;
	}


}

void CMdbxTest::test_cursor()
{
	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	std::string path = "CursorTest.mdb";

	mdbx::env_managed env(path, createParameters, operateParameters);
	mdbx::txn_managed txn = env.start_write();
	mdbx::map_handle handle = txn.create_map("CursorTest", mdbx::key_mode::usual, mdbx::value_mode::single);

	//mdbx::slice keySlice(&i, 8);
	//mdbx::cursor::move_result result = cursor.find(keySlice);
	for(int i = 0; i < 10; i++){
		mdbx::slice keySlice(&i, 4);
		std::string str = "Test";
		mdbx::slice valueSlice(str.data(), str.length());
		if(i != 5){
			txn.put(handle, keySlice, valueSlice, mdbx::put_mode::upsert);
		}
	}
	txn.commit();

	txn = env.start_read();


	handle = txn.open_map("CursorTest", mdbx::key_mode::usual, mdbx::value_mode::single);
	mdbx::cursor_managed cursor = txn.open_cursor(handle);

	int int_5 = 7;
	mdbx::slice keySlice(&int_5, 4);
	mdbx::cursor::move_result result = cursor.lower_bound(keySlice, false);
	if(result.done){
		qDebug() << "Found!" << result.key.as_int32();
	}
	//	if(cursor.seek(keySlice)){
	//		mdbx::cursor::move_result result = cursor.current();
	//		if (result.done){
	//			qDebug() << "Found!";
	//		}
	//	}

}


I_ADD_TEST(CMdbxTest);
