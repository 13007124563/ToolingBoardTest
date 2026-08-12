#ifndef EXCELHELPER_H
#define EXCELHELPER_H
#include <QAxObject>
#include <QDir>
#include <QFile>

class QTableView;
class ExcelHelper{
public:
    ExcelHelper();

    /**
     * @brief importExcel 导入excel
     * @param[out] list 数据传入到list
     * @return true or false
     */
    static bool importExcel(QList<QVariant> &list);

    /**
     * @brief tableToExcel 按照表格的格式导出到excel
     * @param[in] table 表的指针(可以为QTableWidget 或 QTableView)
     * @param[in] title 导出标题
     * @param[in] exportFileName 导出文件名
     * @param[in] dataLists 可以传入自己的数据，格式需要和表格的数据格式一致，无数据则导出表格的数据
     * @return true or false
     */
    static bool tableToExcel(QTableView *table,QString title,QString exportFileName="",QList<QStringList> *dataLists=nullptr);
    /**
     * @brief tableToExcel 将数据导出到excel
     * @param[in] dataLists 可以传入自己的数据，格式需要和表格的数据格式一致，无数据则导出表格的数据
     * @param[in] headers 表头
     * @param[in] columnWidths 列宽
     * @param[in] title 导出标题
     * @param[in] exportFileName 导出文件名
     * @return true or false
     */
    static bool dataToExcel(QList<QStringList> *dataLists,const QStringList &headers,
                            const QVector<int> &columnWidths,QString title,QString exportFileName="");

};

class Excel
{
public:
    Excel();
    virtual ~Excel();

    bool newExcel(const QString &fileName);
    void appendSheet(const QString &sheetName);
    void setCellValue(int row, int column, const QString &value);
    void saveExcel(const QString &fileName);
    void freeExcel();

    QAxObject *m_pApplication=nullptr;
    QAxObject *m_pWorkBooks=nullptr;
    QAxObject *m_pWorkBook=nullptr;
    QAxObject *m_pSheets=nullptr;
    QAxObject *m_pSheet=nullptr;
};

#endif // EXCELHELPER_H




#if 0 // 用法
    QList<QStringList> lists;
    QStringList list;
    list.append("a");
    list.append("b");
    list.append("c");
    list.append("d");
    list.append("e");
    lists.append(list);
    lists.append(list);
    lists.append(list);
    lists.append(list);
    lists.append(list);
    ExcelHelper::tableToExcel(ui->tableWidget,"haha",&lists);//按照tableWidget的表格格式，将lists数据导出

    ExcelHelper::tableToExcel(ui->tableWidget,"haha");//按照tableWidget的表格格式，将数据导出
    ExcelHelper::tableToExcel(ui->tableView,"haha");//按照tableView的表格格式，将数据导出
#endif
