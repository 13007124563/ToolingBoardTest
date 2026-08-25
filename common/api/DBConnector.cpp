#include "DBConnector.h"

#include <QFile>
#include <QtSql/QSqlDatabase>

#include <QDebug>

#include "commondefine.h"

using namespace zl;

#define GETDBERRORSTR(DB) DB.lastError().text()

CDBConnector* zl::CDBConnector::instance_ = nullptr;
CDBConnector * zl::CDBConnector::Instance()
{
	if (instance_ == nullptr)
	{
		instance_ = new CDBConnector;
	}

	return instance_;
}

void zl::CDBConnector::Release()
{
	qDebug() << "[WARNING]" << "release db connector...";
	instance_->UnInit();
	delete instance_;
	instance_ = nullptr;
}

int32_t zl::CDBConnector::Init(const DatabaseConfig & conf)
{
	qDebug() << "[INFO]" << "open database, type: " << conf.type;
	config_ = conf;

	if (conf.type == CDBConnector::EDBType_MYSQL)
	{
		db_ = QSqlDatabase::addDatabase("QMYSQL");
        db_.setHostName(conf.addr);
        db_.setUserName(conf.user);
        db_.setPassword(conf.pwd);
		db_.setConnectOptions("MYSQL_OPT_RECONNECT=1");
	}
	else if (conf.type == EDBType_SQLITE3)
	{
		db_ = QSqlDatabase::addDatabase("QSQLITE");
        db_.setDatabaseName(DB_PATH);
        db_.setUserName(conf.user);
        db_.setPassword(conf.pwd);
	}
	else
	{
		qDebug() << "[ERROR]" << "unsupported database type: " << conf.type;
		return EDBError_DatabaseError;
	}

	// open database
	if (!db_.open())
	{
		qDebug() << "[ERROR]" << "Open Database Error: " << GETDBERRORSTR(db_);
		return EDBError_ConnectFailed;
	}

	// 仅MySQL需要设置字符集，SQLite默认UTF-8
	if (conf.type == CDBConnector::EDBType_MYSQL)
	{
		db_.exec("SET NAMES UTF8");
	}
	
	int32_t ret = init_database();
	if (ret != CDBConnector::EDBError_Success)
	{
		qDebug() << "[ERROR]" << "init database error: " << GETDBERRORSTR(db_);
		return EDBError_InitDatabaseFailed;
	}

    return EDBError_Success;
}

int32_t zl::CDBConnector::Init_sqlite()
{
    qDebug() << "[INFO]" << "open database, type: " << EDBType_SQLITE3;
    config_.type = EDBType_SQLITE3;

    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(DB_PATH);

    if (!db_.open())
    {
        qDebug() << "[ERROR]" << "Open Database Error: " << GETDBERRORSTR(db_);
        return EDBError_ConnectFailed;
    }
    // 注意：SQLite不支持SET NAMES，无需设置字符集（默认UTF-8）
    // db_.exec("SET NAMES UTF8");  // 仅MySQL需要
    int32_t ret = createTable();
    if (ret != CDBConnector::EDBError_Success)
    {
        qDebug() << "[ERROR]" << "init database error: " << GETDBERRORSTR(db_);
        return EDBError_InitDatabaseFailed;
    }

    // insert default card
//    QSqlQuery qu2(db_);
//    qu2.prepare("SELECT * FROM T_MANAGER_CARD");
//    if (qu2.exec())
//    {
//        if (!qu2.next())
//        {
//            db_.exec("INSERT INTO T_MANAGER_CARD (CARDNO, STATUS, TYPE) VALUES('72543100', 1, 3);");
//            qDebug() << "[INFO]" << "insert default cardno: " << GETDBERRORSTR(db_);
//        }
//    }

    return EDBError_Success;
}

int32_t zl::CDBConnector::UnInit()
{
	qDebug() << "[INFO]" << "close database";
	return EDBError_Success;
}

QSqlQuery zl::CDBConnector::CreateQuery()
{
    QMutexLocker locker(&mutex_data);

	return QSqlQuery(db_);
}

int32_t zl::CDBConnector::ExecQuery(QSqlQuery& query)
{
    QMutexLocker locker(&mutex_data);

	begin_transaction();
	if (!query.exec())
	{
		qDebug() << "[ERROR]" << "exec query failed :" << GETDBERRORSTR(query);
		query_rollback();
		return EDBError_ExecQueryFailed;
	}

	query_commit();
	return EDBError_Success;
}

int32_t zl::CDBConnector::ExecBatchQuery(QSqlQuery & query)
{
    QMutexLocker locker(&mutex_data);

	begin_transaction();
	if (!query.execBatch())
	{
		qDebug() << "[ERROR]" << "exec query failed :" << GETDBERRORSTR(query);
		query_rollback();
		return EDBError_ExecQueryFailed;
	}

	query_commit();
	return EDBError_Success;
}

int32_t zl::CDBConnector::init_database()
{
	qDebug() << "[INFO]" << "init database";
	QString filename;
	if (config_.type == EDBType_MYSQL)
	{
		filename = ":/sql/locker_mysql.sql";
	}
	else if (config_.type == EDBType_SQLITE3)
	{
		filename = ":/sql/locker_sqlite3.sql";
	}
	else
	{
		qDebug() << "[ERROR]" << "Init database failed, unknown database type:" << config_.type;
		return EDBError_InitDatabaseFailed;
	}

    QFile sql(filename);
    if (!sql.open(QFile::ReadOnly))
    {
        qDebug() << "[ERROR]" << "Init database failed, load sql res failed";
        return EDBError_InitDatabaseFailed;
    }

	// exec query
	QSqlQuery query(db_);
	query.prepare(sql.readAll());
	if (!query.exec())
	{
		qDebug() << "[ERROR]" << "Init database failed, exec sql failed: " << GETDBERRORSTR(query);
		return EDBError_InitDatabaseFailed;
	}

	// insert default card
	QSqlQuery qu2(db_);
	qu2.prepare("SELECT * FROM T_MANAGER_CARD");
	if (qu2.exec())
	{
		if (!qu2.next())
		{
			db_.exec("INSERT INTO T_MANAGER_CARD (CARDNO, STATUS) VALUES('72543100', 1);");
			qDebug() << "[INFO]" << "insert default cardno: " << GETDBERRORSTR(db_);
		}
	}

	qDebug() << "[INFO]" << "init database success " << GETDBERRORSTR(query);
	return EDBError_Success;
}

void zl::CDBConnector::begin_transaction()
{
    if (config_.type == EDBType_MYSQL)
    {
        db_.exec("START TRANSACTION");
    }
    else if (config_.type == EDBType_SQLITE3)
    {
        db_.transaction();
    }
}

void zl::CDBConnector::query_commit()
{
    if (config_.type == EDBType_MYSQL)
    {
        db_.exec("COMMIT");
    }
    else if (config_.type == EDBType_SQLITE3)
    {
        db_.commit();
    }
}

void zl::CDBConnector::query_rollback()
{
    if (config_.type == EDBType_MYSQL)
    {
        db_.exec("ROLLBACK");
    }
    else if (config_.type == EDBType_SQLITE3)
    {
        db_.rollback();
    }
}

void zl::CDBConnector::reconnect_database()
{

}

int CDBConnector::createTable()
{
    QString autoincrement;
    QStringList keyList,valueList;
    int ok1 = 0;

    if(config_.type == EDBType_MYSQL)
        autoincrement = "int not null auto_increment primary key";
    else
        autoincrement = "integer not null primary key autoincrement";


    //t_record 记录表
    keyList.clear();
    valueList.clear();
    keyList << "ID" << "RECORD_ID" << "SIM_CARD_TYPE"
            << "VER_TEST" << "SIM_TEST" << "IOT_TEST" << "SIMIOT_TEST" << "SERIAL_TEST" << "TEST_TIME" << "RESULT_TYPE" << "CMD_RET_INFO" << "RESULT_INFO" << "TEST_LOG"
            << "MODULE_TYPE" << "IOT_MODULE_ID" << "IOT_IMEI"
            << "ICCID" << "NET_STATUS" << "NETWORK_TYPE" << "SIGNAL_STRENGTH"
            << "VERSION";
    valueList << autoincrement << "varchar(100)" << "int(4)"
              << "int(4)" << "int(4)" << "int(4)" << "int(4)" << "int(4)" << "DATETIME" << "int(4)" << "varchar(100)" << "varchar(300)" << "TEXT"
              << "varchar(20)" << "varchar(50)" << "varchar(50)"
              << "varchar(50)" << "int(4)" << "varchar(50)" << "varchar(50)"
              << "varchar(50)";

    ok1 = createTable("T_RECORD",keyList,valueList);


    if(ok1 > 0)
        return EDBError_Success;
    else
        return EDBError_InitDatabaseFailed;
}

int CDBConnector::createTable(QString table, QStringList fieldList, QStringList typeList, QString index)
{
    QString key;
    QSqlQuery query;
    int pos,ret;
    int count = fieldList.count();

    if(table.isEmpty() || count == 0 || count != typeList.count())
    {
        qDebug()<<QString("[TYPE=1005][OPT=%1][OBJ=%2][RET=%3]").arg("Create table",table,"failed");
        return 0;
    }

    if(config_.type == EDBType_MYSQL)
        query.exec(QString("show tables like '%1'").arg(table));
    else if(config_.type == EDBType_SQLITE3)
        query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg(table));
    if(!query.next() || (config_.type == EDBType_SQLITE3 && query.value(0).toInt() == 0))
    {
        for(int i=0;i<fieldList.count();++i)
            key += fieldList.at(i) + " " + typeList.at(i) + ",";
        if(index.isEmpty())
            key.chop(1);
        else
            key += QString("index(%1)").arg(index);
        if(!query.exec(QString("create table %1(%2)").arg(table,key)))
        {
            qDebug()<<QString("[TYPE=1003][OPT=%1][OBJ=%2][RET=%3][DETAIL=%4]").arg("Create Table",table,"Failed",query.lastError().text());
            return 0;
        }
        return 1;
    }
    else
    {
        QStringList oldFieldList,oldTypeList;
        if(config_.type == EDBType_MYSQL)
            query.exec(QString("desc %1").arg(table));
        else if(config_.type == EDBType_SQLITE3)
            query.exec(QString("pragma table_info (%1)").arg(table));
        while(query.next())
        {
            if(config_.type == EDBType_MYSQL)
            {
                oldFieldList << query.record().value("Field").toString();
                oldTypeList  << query.record().value("Type").toString();
            }
            else if(config_.type == EDBType_SQLITE3)
            {
                oldFieldList << query.record().value("name").toString();
                oldTypeList  << query.record().value("Type").toString();
            }
        }
        for(int i=0;i<count;i++)
        {
            pos = oldFieldList.indexOf(fieldList.at(i));
            if(pos == -1)
            {
                if(!query.exec(QString("alter table %1 add column %2 %3").arg(table,fieldList.at(i),typeList.at(i))))
                {
                    qDebug()<<QString("[TYPE=1003][OPT=%1][OBJ=%2][RET=%3][DETAIL=%4]").arg("alter Table",table,"Failed","Failed or Exception");
                    return 0;
                }
                ret = 2;
            }
            else if(config_.type == EDBType_MYSQL && !typeList.at(i).contains("key",Qt::CaseInsensitive) && !typeList.at(i).contains(oldTypeList.at(pos),Qt::CaseInsensitive))
            {
                if(!query.exec(QString("alter table %1 change %2 %3 %4").arg(table,oldFieldList.at(pos),fieldList.at(i),typeList.at(i))))
                {
                    qDebug()<<QString("[TYPE=1003][OPT=%1][OBJ=%2][RET=%3][DETAIL=%4]").arg("alter Table",table,"Failed","Failed or Exception");
                    return 0;
                }
                ret = 3;
            }
        }
        return ret;
    }
    return 4;
}
