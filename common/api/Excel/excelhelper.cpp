#include "excelhelper.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QTableView>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QDateTime>
#include <QStandardItemModel>

ExcelHelper::ExcelHelper(){

}
//导入excel，数据传入list
bool ExcelHelper::importExcel(QList<QVariant> &list)
{
    QString fileName = QFileDialog::getOpenFileName(0,
                                                    QObject::tr("Inport File"),
                                                    ".",
                                                    "Excel Files(*.xls *.xlsx)");
    if(fileName.isEmpty())
    {
        return false;
    }

    Excel excel;
    bool bRet=excel.newExcel(fileName);
    if(bRet)
    {
        //获取表内的数据
        QAxObject *pUsedRange = excel.m_pSheet->querySubObject("UsedRange");
        QVariant var = pUsedRange->dynamicCall("Value");
        list=var.toList();
        pUsedRange->deleteLater();

        int count=list.count();
        qDebug()<<"importExcel data row count "<<count;

#if 0
         //打印数据
        for(int i=0;i<count;i++) // start from 3rd row,   0---> title ,  1---> header
        {
            QList<QVariant>sublist=list.at(i).toList();
            int subCount=sublist.count();
            for(int j=0;j<subCount;j++){
                qDebug()<<"importExcel get data("<<i<<","<<j<<")"
                       <<sublist.at(j).toString();
            }
        }
#endif
        if(list.isEmpty())
        {
            QMessageBox::information(0,QObject::tr("Tips"),QObject::tr("Read data failed!"));
            return false;
        }

    }
    return bRet;
}


bool ExcelHelper::tableToExcel(QTableView *table,QString title,QString exportFileName/*=""*/,QList<QStringList> *dataLists/*=nullptr*/){

    //默认采用表格的数据
    bool bUseTableData=true;
    QAbstractItemModel *pModel=table->model();
    int i=0,j=0,colCount=pModel->columnCount(),rowCount=pModel->rowCount();
    QAxObject *cell,*col;

    if(dataLists && dataLists->count()>0){
        rowCount=dataLists->count();
        if(dataLists->at(0).count()< 1 || dataLists->at(0).count()!=colCount){
            qWarning()<<"dataLists.at(0).count()"<<dataLists->at(0).count()<<"table colCount"<<colCount;
            QMessageBox::information(0,QObject::tr("Tips"),QObject::tr("Input data invalid!"));
            return false;
        }else{
          //采用传入的数据
          bUseTableData=false;
        }
    }

    QString fileName;
    if(exportFileName.isEmpty()){
        fileName= QFileDialog::getSaveFileName(table, QObject::tr("Save"),
                    "export"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss"),
                    "Excel Files(*.xls *.xlsx)");
    }else{
        fileName= QFileDialog::getSaveFileName(table, QObject::tr("Save"),
                    exportFileName,
                    "Excel Files(*.xls *.xlsx)");
    }

    if (fileName.isEmpty()){
        return false;
    }

    //存在就删除原文件
    QFile file(fileName);
    if(file.exists()){
        bool bRet=file.remove();
        if(!bRet){
            QMessageBox::information(0,QObject::tr("Tips"),QObject::tr("File maybe be opened, replace failed!"));
            return false;
        }
    }

    //打开一个excel 应用
    Excel excel;
    bool bRet=excel.newExcel(fileName);
    if(!bRet){
        return false;
    }

    //标题行
    cell=excel.m_pSheet->querySubObject("Cells(int,int)", 1, 1);
    cell->dynamicCall("SetValue(const QString&)", title);
    cell->querySubObject("Font")->setProperty("Size", 18);

    //调整标题行高
    excel.m_pSheet->querySubObject("Range(const QString&)", "1:1")->setProperty("RowHeight", 30);

    //合并标题行
    QString cellTitle;
    cellTitle.append("A1:");
    cellTitle.append(QChar(colCount - 1 + 'A'));
    cellTitle.append(QString::number(1));
    QAxObject *range = excel.m_pSheet->querySubObject("Range(const QString&)", cellTitle);
    range->setProperty("WrapText", true);
    range->setProperty("MergeCells", true);
    range->setProperty("HorizontalAlignment", -4108);//xlCenter
    range->setProperty("VerticalAlignment", -4108);//xlCenter

    //列标题
    for(i=0;i<colCount;i++)
    {
        QString columnName;
        columnName.append(QChar(i  + 'A'));
        columnName.append(":");
        columnName.append(QChar(i + 'A'));
        col = excel.m_pSheet->querySubObject("Columns(const QString&)", columnName);
        //列宽//
        col->setProperty("ColumnWidth", table->columnWidth(i)/6);
        cell=excel.m_pSheet->querySubObject("Cells(int,int)", 2, i+1);
        //表头//
        columnName=pModel->headerData(i,Qt::Horizontal).toString();
        cell->dynamicCall("SetValue(const QString&)", columnName);
        cell->querySubObject("Font")->setProperty("Bold", true);
        cell->querySubObject("Interior")->setProperty("Color",QColor(191, 191, 191));
        cell->setProperty("HorizontalAlignment", -4108);//xlCenter
        cell->setProperty("VerticalAlignment", -4108);//xlCenter
    }

    //数据区
    QString str;
    QModelIndex modelIndex;

    if(bUseTableData){
        //采用表格的数据
        for(i=0;i<rowCount;i++){
            for (j=0;j<colCount;j++)
            {
                modelIndex = pModel->index(i, j);
                str = pModel->data(modelIndex).toString();
                if(str.count()>11  || (!str.isEmpty() && '0'==str.at(0)) )
                {
                    str.insert(0,QString("'"));
                }//解决excel表超过11位显示不完全和首位为0会省略0的问题
                excel.m_pSheet->querySubObject("Cells(int,int)", i+3, j+1)->dynamicCall("SetValue(const QString&)",str);
            }
        }
    }else{
        //采用传入的数据
        int tmpCount=0;
        for(i=0;i<rowCount;i++){
            tmpCount=dataLists->at(i).count();
            for (j=0;j<tmpCount;j++)
            {
                str=dataLists->at(i).at(j);
                if(str.count()>11  || (!str.isEmpty() && '0'==str.at(0)) )
                {
                    str.insert(0,QString("'"));
                }//解决excel表超过11位显示不完全的问题
                excel.m_pSheet->querySubObject("Cells(int,int)", i+3, j+1)->dynamicCall("SetValue(const QString&)",str);
            }
        }
    }

    //画框线
    QString lrange;
    lrange.append("A2:");
    lrange.append(colCount - 1 + 'A');
    lrange.append(QString::number(rowCount + 2));
    range = excel.m_pSheet->querySubObject("Range(const QString&)", lrange);
    range->querySubObject("Borders")->setProperty("LineStyle", QString::number(1));
    range->querySubObject("Borders")->setProperty("Color", QColor(0, 0, 0));
    //调整数据区行高
    QString rowsName;
    rowsName.append(QString("2:"));//
    rowsName.append(QString::number(rowCount + 2));
    range = excel.m_pSheet->querySubObject("Range(const QString&)", rowsName);
    range->setProperty("RowHeight", 20);
    //保存
    excel.m_pWorkBook->dynamicCall("SaveAs(const QString&)",QDir::toNativeSeparators(fileName));//保存至fileName
    excel.freeExcel();

    QMessageBox::information(0,QObject::tr("Complete"),QObject::tr("export success!"));
//    if(QMessageBox::Yes==ret){
//        QDesktopServices::openUrl(QUrl("file:///" + QDir::toNativeSeparators(fileName)));
//    }
    return true;
}

bool ExcelHelper::dataToExcel(QList<QStringList> *dataLists,const QStringList &headers,
                        const QVector<int> &columnWidths,QString title,QString exportFileName/*=""*/)
{
    int colCount=headers.count();
    int colWidthCount=columnWidths.count();
    if(colCount!=colWidthCount){
        qWarning()<<"parameter error: headers.count() != columnWidths.count()";
        return false;
    }

    QTableView table;
    QStandardItemModel model;
    //设置表头
    model.setColumnCount(colCount);
    for(int i=0;i<colCount;i++){
        model.setHeaderData(i,Qt::Horizontal,headers.at(i));
    }
    table.setModel(&model);

    //设置列宽
    for(int i=0;i<colWidthCount;i++){
        table.setColumnWidth(i,columnWidths.at(i));
    }

    QString defaultFileName;
    //导出文件名
    if(exportFileName.isEmpty()){
        defaultFileName=title+QDateTime::currentDateTime().toString("yyyyMMddhhmm");
    }else{
        defaultFileName=exportFileName;
    }

    ExcelHelper::tableToExcel(&table,title,defaultFileName,dataLists);

    return true;
}


////////////////////////////////
////////////////////////////////
Excel::Excel()
{

}

Excel::~Excel()
{
    freeExcel();
}

//新建Execl文件
bool Excel::newExcel(const QString &fileName){
    m_pApplication = new QAxObject();

    if(!m_pApplication->setControl("ket.Application")
            && !m_pApplication->setControl("Excel.Application")){
        QMessageBox::information(0,QObject::tr("Tips"),QObject::tr("Excel is not installed"));
        m_pApplication->deleteLater();
        return false;
    }
    //连接Excel控件
    m_pApplication->dynamicCall("SetVisible(bool)", false); //false不显示窗体
    m_pApplication->setProperty("DisplayAlerts", false); //不显示任何警告信息。
    m_pWorkBooks = m_pApplication->querySubObject("Workbooks");
    QFile file(fileName);
    if(file.exists()){
        m_pWorkBook = m_pWorkBooks->querySubObject("Open(const QString &)", fileName);
    }else{
        m_pWorkBooks->dynamicCall("Add");
        m_pWorkBook = m_pApplication->querySubObject("ActiveWorkBook");
    }
    //默认有一个sheet
    m_pSheets = m_pWorkBook->querySubObject("Sheets");
    m_pSheet=m_pWorkBook->querySubObject("Worksheets(int)", 1);
//    m_pSheet = m_pSheets->querySubObject("Item(int)", 1);
    return true;
}

//增加1个Worksheet，这里要注意：默认有一个sheet，需要的时候再调用这个函数
void Excel::appendSheet(const QString &sheetName){
    if(!m_pSheets){
        qWarning()<<"worksheets is null";
        return;
    }
    int cnt = 1;
    QAxObject *pLastSheet = m_pSheets->querySubObject("Item(int)", cnt);
    m_pSheets->querySubObject("Add(QVariant)", pLastSheet->asVariant());
    m_pSheet = m_pSheets->querySubObject("Item(int)", cnt);
    pLastSheet->dynamicCall("Move(QVariant)", m_pSheet->asVariant());
    m_pSheet->setProperty("Name", sheetName);
}

//向Excel单元格中写入数据 //PS:这里涉及到 对于数据格式的设置
void Excel::setCellValue(int row, int column, const QString &value){
    if(!m_pSheet){
        qWarning()<<"worksheet is null";
        return;
    }
    QAxObject *pRange = m_pSheet->querySubObject("Cells(int,int)", row, column);
    pRange->dynamicCall("SetValue(const QString&)", value);
    //内容居中
    pRange->setProperty("HorizontalAlignment", -4108);
    pRange->setProperty("VerticalAlignment", -4108);
    if(row == 1){
        //加粗
        QAxObject *font = pRange->querySubObject("Font"); //获取单元格字体
        font->setProperty("Bold",true); //设置单元格字体加粗
    }
}

//保存Excel
void Excel::saveExcel(const QString &fileName){
    if(!m_pWorkBook){
        newExcel(fileName);
    }
    m_pWorkBook->dynamicCall("SaveAs(const QString &)",QDir::toNativeSeparators(fileName));
}

//释放Excel
void Excel::freeExcel(){
    if(m_pWorkBook){
        m_pWorkBook->dynamicCall("Close()");//关闭工作簿
    }
    if(m_pApplication){
        m_pApplication->dynamicCall("Quit()");//关闭excel
        m_pApplication->deleteLater();
        m_pApplication=nullptr;
    }
}
