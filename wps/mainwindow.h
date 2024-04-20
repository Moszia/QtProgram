#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "childwind.h"
#include <QSignalMapper>
#include <QMdiSubWindow>
#include <QtPrintSupport/QPrinter>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //初始化窗口
    void initMainWindow();
    //新建文档
    void docNew();
    //打开文档
    void docOpen();
    //文档保存
    void docSave();
    //文件另存为
    void docSaveAs();
    //文档撤销
    void docUndo();
    //文档不撤销
    void docRedo();
    //文档剪切
    void docCut();
    //文档复制
    void docCopy();
    //文档粘贴
    void docPaste();
    //文档打印
    void docPrint();
    //打印预览
    void docPrintPreview();

    //设置粗体
    void textBold();
    //设置倾斜
    void textItalic();
    //设置下划线
    void textUnderline();
    //设置字体
    void textFamily(const QString &fmly);
    //设置大小
    void textSize(const QString &ps);
    //设置颜色
    void textColor();
    //关于
    void aboutHelp();

private slots:
    void on_newAction_triggered();

    void refreshMenus();

    void addSubWndListMenu();

    void on_closeAction_triggered();

    void on_closeAllAction_triggered();

    void on_titleAction_triggered();

    void on_cascadeAction_triggered();

    void on_nextAction_triggered();

    void on_previousAction_triggered();

    void setActiveSubWindow(QWidget*wnd);

    void on_openAction_triggered();

    void on_saveAction_triggered();

    void on_saveAsAction_triggered();

    void on_redoAction_triggered();

    void on_uodoAction_triggered();

    void on_cutAction_triggered();

    void on_copyAction_triggered();

    void on_pasteAction_triggered();

    void on_boldAction_triggered();

    void on_italicAction_triggered();

    void on_underlineAction_triggered();

    void on_fontComboBox_currentTextChanged(const QString &arg1);

    void on_sizeComboBox_currentTextChanged(const QString &arg1);

    void on_leftAlignAction_triggered();

    void on_rightAlignAction_triggered();

    void on_centerAction_triggered();

    void on_justifyAction_triggered();

    void on_colorAction_triggered();

    void on_printAction_triggered();

    void printPreview(QPrinter* printer);

    void on_printPreAction_triggered();

    void on_aboutAction_triggered();

private:
    //私有成员函数

    //使得所有的操作都能够使用
    void formatEnabled();
    //激活子窗口
    ChildWind *activateChildWind();
    //查找子窗口
    QMdiSubWindow* findChildWnd(const QString& docName);
    //关闭大窗口后所有的子窗口应该关闭
    void closeEvent(QCloseEvent*event);

    //私有成员方法

    Ui::MainWindow *ui;
    QSignalMapper* m_WndMapper; //信号映射器，针对相同的信号做出不同的操作
};
#endif // MAINWINDOW_H
