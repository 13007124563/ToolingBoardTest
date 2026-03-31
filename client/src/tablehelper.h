#ifndef TABLEHELPER_H
#define TABLEHELPER_H

#include <QObject>
#include <QStringList>
#include <QVector>
#include <QStandardItemModel>
#include <QTableView>

class TableHelper
{
public:
    TableHelper();

    // 初始化设置tableview控件的表头
    static void initTableHeader(const QStringList &strHeaderList,const QVector<int> headerWidthVector,
                         QStandardItemModel* model,QTableView* table);
    static void initTableHeader(const QStringList &strHeaderList,QStandardItemModel* model,QTableView* table);

    // 更新tableview控件表数据
    static void readyUpdate(QStandardItemModel* model,QTableView* table);
    static void finishUpdate(QStandardItemModel* model,QTableView* table,bool autoResizeRows =true);

    // 选中tableview控件所有行
    static void selectAllRows(QStandardItemModel* model,QTableView* table);

    // 取消选中tableview控件所有行
    static void clearAllSelectedRows(QStandardItemModel* model,QTableView* table);

    // 获取tableview控件内 所有选中行的userdata
    static void getSelectedRowsData(const QTableView* table, const int32_t& dataIndex, QStringList& dataList);

    // 按照datalist 删除对应的行
    static bool removeRedundantRows(QStandardItemModel* model,const int32_t dataIndex, const QStringList& dataList);
};

#endif // TABLEHELPER_H
