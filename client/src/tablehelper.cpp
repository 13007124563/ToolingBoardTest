#include "tablehelper.h"
#include <QHeaderView>
#include <QTimer>
#include <QThread>



TableHelper::TableHelper()
{

}

void TableHelper::initTableHeader(const QStringList &strHeaderList,const QVector<int> headerWidthVector,
                     QStandardItemModel* model,QTableView* table)
{
    if (model == Q_NULLPTR || table == Q_NULLPTR) return;

    int colCount=strHeaderList.count();
    model->clear();
    model->setColumnCount(colCount);
    model->setHorizontalHeaderLabels(strHeaderList);
    table->setModel(model);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);// 列宽可调整

    colCount=headerWidthVector.count();
    for(int i=0;i<colCount;i++){
        table->horizontalHeader()->resizeSection(i,headerWidthVector.at(i));
    }

    table->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置表格只读
    table->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中模式为选中行

}

void TableHelper::initTableHeader(const QStringList &strHeaderList,QStandardItemModel* model,QTableView* table)
{
    if (model == Q_NULLPTR || table == Q_NULLPTR) return;

    int colCount=strHeaderList.count();
    model->clear();
    model->setColumnCount(colCount);
    model->setHorizontalHeaderLabels(strHeaderList);
    table->setModel(model);

    table->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置表格只读
    table->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中模式为选中行

    table->horizontalHeader()->setSectionsMovable(true);//可拖拽列

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void TableHelper::readyUpdate(QStandardItemModel* model,QTableView* table)
{
    if (model == Q_NULLPTR || table == Q_NULLPTR) return;

//    //清除数据
    int row = model->rowCount();
    model->removeRows(0, row);

//    model->setRowCount(0);

    table->setUpdatesEnabled(false); // 禁止刷新
}

void TableHelper::finishUpdate(QStandardItemModel* model,QTableView* table,bool autoResizeRows/* =true*/)
{
    if (model == Q_NULLPTR || table == Q_NULLPTR) return;

    QStandardItem *item=nullptr;
    int colCount=model->columnCount();
    int rowCount=model->rowCount();

    for (int i=0; i< rowCount ; i++)
    {
        // 设置居中对齐
        for(int j=0;j<colCount;j++)
        {
            item=model->item(i, j);
            if(item)
            {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    table->setUpdatesEnabled(true); // 启用刷新
    if(autoResizeRows)
    {
        table->resizeRowsToContents(); // 自适应内容
    }
    else
    {
       table->resizeColumnsToContents(); // 自适应内容
    }
}

void TableHelper::selectAllRows(QStandardItemModel* model,QTableView* table)
{
    if (model == Q_NULLPTR || table == Q_NULLPTR) return;

    QItemSelectionModel* selection_model = table->selectionModel();
    if (selection_model == Q_NULLPTR) return;

    int32_t row_count = model->rowCount();
    int32_t column_count = model->columnCount();

    QItemSelection selection;
    for (int i = 0; i < row_count; i++)
    {
        QModelIndex left = model->index(i, 0);
        QModelIndex right = model->index(i, column_count - 1);
        QItemSelection sel(left, right);

        //将每一个单元格/每一行都作为一个QItemSelection 对象，并拼接到总的QItemSelection 对象中
        selection.merge(sel, QItemSelectionModel::Select);
    }

    selection_model->select(selection, QItemSelectionModel::Select);
}

void TableHelper::clearAllSelectedRows(QStandardItemModel* model,QTableView* table)
{
    if (model == Q_NULLPTR || table == Q_NULLPTR) return;

    QItemSelectionModel* selection_model = table->selectionModel();
    if (selection_model == Q_NULLPTR) return;

    int32_t row_count = model->rowCount();
    int32_t column_count = model->columnCount();

    QItemSelection selection;
    for (int i = 0; i < row_count; i++)
    {
        QModelIndex left = model->index(i, 0);
        QModelIndex right = model->index(i, column_count - 1);
        QItemSelection sel(left, right);

        //将每一个单元格/每一行都作为一个QItemSelection 对象，并拼接到总的QItemSelection 对象中
        selection.merge(sel, QItemSelectionModel::Select);
    }

    selection_model->select(selection, QItemSelectionModel::Clear);
}

void TableHelper::getSelectedRowsData(const QTableView* table, const int32_t& dataIndex, QStringList& dataList)
{
    if (table == Q_NULLPTR || dataIndex < 0) return;

    dataList.clear();

    QModelIndexList indexlist = table->selectionModel()->selectedRows(dataIndex);
    if (indexlist.isEmpty())
    {
        return;
    }

    for (int i = 0; i < indexlist.size(); i++)
    {
        QModelIndex index = indexlist.at(i);
        QString data = index.data(Qt::UserRole + 1).toString();

        dataList.append(data);
    }
}

bool TableHelper::removeRedundantRows(QStandardItemModel *model, const int32_t dataIndex, const QStringList &dataList)
{
    if (model == Q_NULLPTR || dataIndex < 0 || dataList.isEmpty()) return false;

    int32_t row_count = model->rowCount();
    if (row_count <= 0) return false;

    for (int i = (row_count-1); i >=0; i--)
    {
        QString data = model->index(i, dataIndex).data(Qt::UserRole + 1).toString();

        if (dataList.contains(data) == false)
        {
            model->removeRow(i);
        }
    }

    return true;
}
