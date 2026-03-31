#ifndef CDBCONNECTOR_H_
#define CDBCONNECTOR_H_

#include <QtSql/QtSql>
#include "CommonType.h"

namespace zl {

// db connector
class CDBConnector
{
public:	
	// 错误码
	enum EDBError {
		EDBError_Success,				
		EDBError_DatabaseError,
		EDBError_ConnectFailed,
		EDBError_InitDatabaseFailed,
		EDBError_ExecQueryFailed,
		EDBError_Undefined			
	};
	// 数据库类型
	enum EDBType {
		EDBType_MYSQL			= 0x01,
		EDBType_SQLITE3			= 0x02,
		EDBType_SQLSERVER		= 0x03,
		EDBType_ORCAL			= 0x04
	};

	static CDBConnector* Instance();
	static void Release();

	int32_t Init(const DatabaseConfig& conf);
    int32_t Init_sqlite();
	int32_t UnInit();

	QSqlQuery CreateQuery();
	int32_t ExecQuery(QSqlQuery& query);
	int32_t ExecBatchQuery(QSqlQuery& query);

protected:
	int32_t init_database();
	void begin_transaction();
	void query_commit();
	void query_rollback();
	void reconnect_database();

    //cjc add
    int createTable();
    int createTable(QString table,QStringList fieldList,QStringList typeList,QString index="");

private:
	explicit CDBConnector() = default;
	~CDBConnector() = default;

	static CDBConnector*	instance_;
	QSqlDatabase			db_;
	DatabaseConfig			config_;

    QMutex                  mutex_data;                 // 线程保护
};

}

#endif // CDBCONNECTOR_H_
