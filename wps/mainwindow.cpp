#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMdiArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMdiSubWindow>
#include <QMenu>
#include <QCloseEvent>
#include <QFileDialog>
#include <QColorDialog>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initMainWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initMainWindow()
{
    //设置mdiArea为中心区域，自适应主窗口大小
    //setCentralWidget(ui->mdiArea);
    //设置窗口名称
    setWindowTitle("自定义WPS");

    // 创建布局管理器
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(ui->comboBox);
    hLayout->addWidget(ui->fontComboBox);
    hLayout->addWidget(ui->sizeComboBox);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(ui->mdiArea);

    // 创建中心窗口部件并设置布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(vLayout);
    setCentralWidget(centralWidget);

    //初始化字号列表项
    QFontDatabase fontdb;
    //将标准字号添加到链表项中
    foreach (int fontsize,fontdb.standardSizes())  //fontdb.standardSizes()返回值是标准字号的一个链表
        ui->sizeComboBox->addItem(QString::number(fontsize));
    //设置默认字号

    //当前应用程序默认的字体
    QFont defautFont;
    QString sFontSize;
    //当前应用程序默认字体的字号
    int defFontSize;
    //当前字号在组合框(即链表项)中的索引号
    int defFontindex;
    //QApplication::font()返回当前字体所有信息
    defautFont = QApplication::font();
    //获取当前默认字体的点大小，返回的是int类型
    defFontSize = defautFont.pointSize();
    //将数值转化为字符串
    sFontSize = QString::number(defFontSize);
    //在链表项中查找到该默认字号
    defFontindex = ui->sizeComboBox->findText(sFontSize);
    //设置当前字号
    ui->sizeComboBox->setCurrentIndex(defFontindex);

    //设置多文档区域滚动条，当需要的时候显示
    ui->mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    //刷新菜单
    refreshMenus();
    //连接子窗口与操作
    connect(ui->mdiArea,&QMdiArea::subWindowActivated,this,&MainWindow::refreshMenus);
    //添加子窗口链表
    addSubWndListMenu();
    connect(ui->menu_W,&QMenu::aboutToShow,this,&MainWindow::addSubWndListMenu);

    //创建信号映射器
    m_WndMapper = new QSignalMapper(this);
    connect(m_WndMapper,SIGNAL(mapped(QWidget*)),this,SLOT(setActiveSubWindow(QWidget*)));
    //设置字体按钮是可被检查的，默认是不能被检查的,也可以通过ui界面来设置
    ui->boldAction->setCheckable(true);
    ui->italicAction->setCheckable(true);
    ui->underlineAction->setCheckable(true);

    //将对其方式添加到分组中，因为对其方式选择具有互斥性,一次只能选择一种
    QActionGroup* alignGroup = new QActionGroup(this);
    alignGroup->addAction(ui->leftAlignAction);
    alignGroup->addAction(ui->rightAlignAction);
    alignGroup->addAction(ui->centerAction);
    alignGroup->addAction(ui->justifyAction);

}

//新建文档
void MainWindow::docNew()
{
    ChildWind *childwind = new ChildWind;
    //添加子窗口
    ui->mdiArea->addSubWindow(childwind);

    childwind->newDoc();
    childwind->show();

    //当复制信号有效时，剪切，复制动作都能用
    connect(childwind,SIGNAL(copyAvailable(bool)),ui->cutAction,SLOT(setEnabled(bool)));
    connect(childwind,SIGNAL(copyAvailable(bool)),ui->copyAction,SLOT(setEnabled(bool)));

}
//打开文档
void MainWindow::docOpen()
{
    QString docName = QFileDialog::getOpenFileName(this,
                                 tr("打开文档"),
                                 "",
                                 tr("文本文件(*.txt);;"
                                  "HTML文件(*.html);;"
                                  "所有文件(*.*)"));
    //如果文档不为空
    if(!docName.isEmpty()){
        QMdiSubWindow* existWnd = findChildWnd(docName);
        //查看当前文件是否已经被打开过了
        if(existWnd){
            //想要打开的文件已经打开，正好将其设置为活动子窗口
            ui->mdiArea->setActiveSubWindow(existWnd);
            return ;
        }
        ChildWind* childwnd = new ChildWind;
        ui->mdiArea->addSubWindow(childwnd);

        connect(childwnd,SIGNAL(copyAvailable(bool)),ui->cutAction,SLOT(setEnabled(bool)));
        connect(childwnd,SIGNAL(copyAvailable(bool)),ui->copyAction,SLOT(setEnabled(bool)));
        //文档内容加载成功
        if(childwnd->loadDoc(docName)){
            statusBar()->showMessage("文档已打开",3000);
            childwnd->show();
            //使得操作可用
            formatEnabled();
        }else {
            childwnd->close();
        }
    }
}

//保存文档
void MainWindow::docSave()
{
    //如果当前窗口是活动子窗口，调用子窗口保存函数
    if (activateChildWind() && activateChildWind()->saveDoc()) {
        statusBar()->showMessage("保存成功", 3000);
    }
}

//另存为
void MainWindow::docSaveAs()
{
    if (activateChildWind()) {
        activateChildWind()->saveAsDoc();
        statusBar()->showMessage("保存成功", 3000);
    }
}

//撤销
void MainWindow::docUndo()
{
    //如果是活动的子窗口直接调用qt封装好的函数
    if (activateChildWind()) {
        activateChildWind()->undo();
    }
}

//重做
void MainWindow::docRedo()
{
    //如果是活动的子窗口直接调用qt封装好的函数
    if (activateChildWind()) {
        activateChildWind()->redo();
    }
}

//剪切
void MainWindow::docCut()
{
    //如果是活动的子窗口直接调用qt封装好的函数
    if (activateChildWind()) {
        activateChildWind()->cut();
    }
}

//复制
void MainWindow::docCopy()
{
    //如果是活动的子窗口直接调用qt封装好的函数
    if (activateChildWind()) {
        activateChildWind()->copy();
    }
}

//粘贴
void MainWindow::docPaste()
{
    //如果是活动的子窗口直接调用qt封装好的函数
    if (activateChildWind()) {
        activateChildWind()->paste();
    }
}

//打印
void MainWindow::docPrint()
{
    //per打印机
    QPrinter pter(QPrinter::HighResolution);
    QPrintDialog* dlg = new QPrintDialog(&pter, this);
    if (activateChildWind()) {
        dlg->setOption(QAbstractPrintDialog::PrintSelection);
    }
    dlg->setWindowTitle("打印文档");

    ChildWind* childwind = activateChildWind();
    if (dlg->exec() == QDialog::Accepted)
    {
        childwind->print(&pter);
    }
    delete dlg;
}

//打印预览
void MainWindow::docPrintPreview()
{
    QPrinter pter(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&pter, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)),
            this, SLOT(printPreview(QPrinter*)));
    preview.exec();
}

//设置粗体
void MainWindow::textBold()
{
    QTextCharFormat format;

    //即检查用户是否启用了加粗功能
    bool res = ui->boldAction->isChecked();
    format.setFontWeight(res ? QFont::Bold : QFont::Normal);
    if(activateChildWind()) {
        //作用是在当前子窗口中选中的单词上应用指定的字符格式
        activateChildWind()->setFormatOnSelectedWord(format);
    }
}

//设置倾斜
void MainWindow::textItalic()
{
    QTextCharFormat format;

    //如果用户使用了倾斜功能，则将format设置为倾斜
    format.setFontItalic(ui->italicAction->isChecked());
    if(activateChildWind()) {
        //作用是在当前子窗口中选中的单词上应用指定的字符格式
        activateChildWind()->setFormatOnSelectedWord(format);
    }
}

//设置下划线
void MainWindow::textUnderline()
{
    QTextCharFormat format;

    format.setFontUnderline(ui->underlineAction->isChecked());
    if(activateChildWind()) {
        //作用是在当前子窗口中选中的单词上应用指定的字符格式
        activateChildWind()->setFormatOnSelectedWord(format);
    }
}

//设置字体
void MainWindow::textFamily(const QString &fmly)
{
    QTextCharFormat format;
    format.setFontFamily(fmly);
    if(activateChildWind()) {
        //作用是在当前子窗口中选中的单词上应用指定的字符格式
        activateChildWind()->setFormatOnSelectedWord(format);
    }
}

//设置字号
void MainWindow::textSize(const QString &ps)
{
    qreal pointSize = ps.toFloat();
    if (ps.toFloat() > 0)
    {
        QTextCharFormat format;
        format.setFontPointSize(pointSize);
        if(activateChildWind()) {
            //作用是在当前子窗口中选中的单词上应用指定的字符格式
            activateChildWind()->setFormatOnSelectedWord(format);
        }
    }
}

//设置颜色
void MainWindow::textColor()
{
    if(activateChildWind()) {
        //作用是在当前子窗口中选中的单词上应用指定的字符格式
        QColor color = QColorDialog::getColor(activateChildWind()->textColor(), this);
        //判断设置的颜色是否合法
        if (!color.isValid()) return;
        QTextCharFormat format;
        format.setForeground(color);
        activateChildWind()->setFormatOnSelectedWord(format);

        //当设置完颜色后，将图标设置成设置的颜色
        QPixmap pix(16, 16);
        pix.fill(color);
        ui->colorAction->setIcon(pix);
    }
}

void MainWindow::aboutHelp()
{
    QMessageBox::information(this, "提示", "更多情况请联系Moszia6693@gmail.com");
}


//使得对文字操作可用
void MainWindow::formatEnabled()
{
    ui->boldAction->setEnabled(true);
    ui->italicAction->setEnabled(true);
    ui->underlineAction->setEnabled(true);
    ui->leftAlignAction->setEnabled(true);
    ui->centerAction->setEnabled(true);
    ui->rightAlignAction->setEnabled(true);
    ui->justifyAction->setEnabled(true);
    ui->colorAction->setEnabled(true);
}

//返回当前活动的子窗口
ChildWind *MainWindow::activateChildWind()
{
    //activeSubWindow()返回mdiArea的子窗口，是QMdiSubWindow类型的
    //但是我们需要的是ChildWind类型的子窗口，因此需要类型转换
    QMdiSubWindow* actWnd = ui->mdiArea->activeSubWindow();
    if (actWnd) {
        //先转换成部件类型，再转换成ChildWind类型
        return qobject_cast<ChildWind*>(actWnd->widget());
    } else {
        return 0;
    }
}

//查找子窗口
QMdiSubWindow *MainWindow::findChildWnd(const QString &docName)
{
    QString strFile = QFileInfo(docName).canonicalFilePath();
    //遍历当前所有子窗口
    foreach (QMdiSubWindow* subWnd, ui->mdiArea->subWindowList()) {

        ChildWind*childwnd = qobject_cast<ChildWind*>(subWnd);
        if(childwnd->m_CurDocPath == strFile) return subWnd;
    }
    return 0;

}

//刷新菜单
void MainWindow::refreshMenus()
{
    bool hasChild = false;
    //activateChildWind()返回子窗口类型指针，如果不为0代表有活动子窗口
    hasChild = (activateChildWind() != 0);

    ui->saveAction->setEnabled(hasChild);
    ui->saveAsAction->setEnabled(hasChild);
    ui->printAction->setEnabled(hasChild);
    ui->printPreAction->setEnabled(hasChild);
    ui->pasteAction->setEnabled(hasChild);
    ui->closeAction->setEnabled(hasChild);
    ui->closeAllAction->setEnabled(hasChild);
    ui->titleAction->setEnabled(hasChild);
    ui->cascadeAction->setEnabled(hasChild);
    ui->nextAction->setEnabled(hasChild);
    ui->previousAction->setEnabled(hasChild);

    //打开文档且有内容选中                    //光标是否有内容选中
    bool hasSelect = (activateChildWind() && activateChildWind()->textCursor().hasSelection());

    ui->cutAction->setEnabled(hasSelect);
    ui->copyAction->setEnabled(hasSelect);
    ui->boldAction->setEnabled(hasSelect);
    ui->italicAction->setEnabled(hasSelect);
    ui->underlineAction->setEnabled(hasSelect);
    ui->leftAlignAction->setEnabled(hasSelect);
    ui->rightAlignAction->setEnabled(hasSelect);
    ui->centerAction->setEnabled(hasSelect);
    ui->justifyAction->setEnabled(hasSelect);
    ui->colorAction->setEnabled(hasSelect);

}
//获取子窗口链表菜单
void MainWindow::addSubWndListMenu()
{
    //清空菜单
    ui->menu_W->clear();
    //增添
    ui->menu_W->addAction(ui->closeAction);
    ui->menu_W->addAction(ui->closeAllAction);
    ui->menu_W->addSeparator();
    ui->menu_W->addAction(ui->titleAction);
    ui->menu_W->addAction(ui->cascadeAction);
    ui->menu_W->addSeparator();
    ui->menu_W->addAction(ui->nextAction);
    ui->menu_W->addAction(ui->previousAction);

    //获取所有打开的子窗口，将其存储到链表中
    QList<QMdiSubWindow*> wnds = ui->mdiArea->subWindowList();
    //判断链表是否为空，若不为空将菜单加一个分割线
    if (!wnds.isEmpty()) ui->menu_W->addSeparator();

    for(int i =0; i<wnds.size(); i++) {

        ChildWind* childwind = qobject_cast<ChildWind*>(wnds.at(i)->widget());
        QString menuitem_text;
        menuitem_text = QString("%1 %2")
                            .arg(i+1)
                            .arg(childwind->getCurDocName());
        QAction* menuitem_act = ui->menu_W->addAction(menuitem_text);
        //在活动窗口前面加上小勾勾
        menuitem_act->setCheckable(true);
        menuitem_act->setChecked(childwind == activateChildWind());

        //将菜单项与信号映射器联系起来
        connect(menuitem_act,SIGNAL(triggered(bool)),m_WndMapper,SLOT(map()));

        m_WndMapper->setMapping(menuitem_act,wnds.at(i));
    }
    //更新格式
    formatEnabled();
}

//将传递过来的子窗口设置为活动窗口
void MainWindow::setActiveSubWindow(QWidget *wnd)
{
    if(!wnd) return;
    //将部件类型的指针转换成子窗口类型
    ui->mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow*>(wnd));
}

//将新建文件的操作与新建按钮连接起来
void MainWindow::on_newAction_triggered()
{
    docNew();
}

void MainWindow::on_closeAction_triggered()
{
    //将活动的子窗口进行关闭
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_closeAllAction_triggered()
{
    //关闭所有子窗口
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_titleAction_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_cascadeAction_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_nextAction_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_previousAction_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_openAction_triggered()
{
    docOpen();
}

void MainWindow::on_saveAction_triggered()
{
    docSave();
}

void MainWindow::on_saveAsAction_triggered()
{
    docSaveAs();
}

void MainWindow::on_redoAction_triggered()
{
    docRedo();
}

void MainWindow::on_uodoAction_triggered()
{
    docUndo();
}

void MainWindow::on_cutAction_triggered()
{
    docCut();
}

void MainWindow::on_copyAction_triggered()
{
    docCopy();
}

void MainWindow::on_pasteAction_triggered()
{
    docPaste();
}

void MainWindow::on_boldAction_triggered()
{
    textBold();
}

void MainWindow::on_italicAction_triggered()
{
    textItalic();
}

void MainWindow::on_underlineAction_triggered()
{
    textUnderline();
}

void MainWindow::on_fontComboBox_currentTextChanged(const QString &arg1)
{
    textFamily(arg1);
}

void MainWindow::on_sizeComboBox_currentTextChanged(const QString &arg1)
{
    textSize(arg1);
}

void MainWindow::on_leftAlignAction_triggered()
{
    if (activateChildWind())
    {
        activateChildWind()->setAlignOfDocumentText(1);
    }
}

void MainWindow::on_rightAlignAction_triggered()
{
    if (activateChildWind())
    {
        activateChildWind()->setAlignOfDocumentText(2);
    }
}

void MainWindow::on_justifyAction_triggered()
{
    if (activateChildWind())
    {
        activateChildWind()->setAlignOfDocumentText(3);
    }
}

void MainWindow::on_centerAction_triggered()
{
    if (activateChildWind())
    {
        activateChildWind()->setAlignOfDocumentText(4);
    }
}

void MainWindow::on_colorAction_triggered()
{
    textColor();
}

void MainWindow::on_printAction_triggered()
{
    docPrint();
}

void MainWindow::printPreview(QPrinter *printer)
{
    activateChildWind()->print(printer);
}

void MainWindow::on_printPreAction_triggered()
{
    docPrintPreview();
}

void MainWindow::on_aboutAction_triggered()
{
    aboutHelp();
}

//主窗口关闭事件
void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if(ui->mdiArea->currentSubWindow())
        event->ignore();
    else
        event->accept();
}

