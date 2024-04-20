#ifndef CHILDWIND_H
#define CHILDWIND_H

#include <QTextEdit>

class ChildWind : public QTextEdit
{
    Q_OBJECT
public:
    //公共成员函数

    ChildWind();
    //新建文档
    void newDoc();
    //保存文档
    bool saveDoc();
    //另存为
    bool saveAsDoc();
    //用来真正执行保存文档的操作
    bool saveDocOpt(QString docName);
    //从文档路径中提取文档名
    QString getCurDocName();
    //加载文档，因为文档加载会有成功和失败
    bool loadDoc(const QString& docName);
    //设置当前文档
    void setCurDoc(const QString& docName);
    //设置字体格式
    void setFormatOnSelectedWord(const QTextCharFormat& format);
    //设置对其方式
    void setAlignOfDocumentText(int aligntype);
    //打开pdf文件
    //void openPdfFile(const QString& filePath);

    //公共成员变量

    //当前文档路径
    QString m_CurDocPath;

private slots:
    //文档修改时在文档标题栏加上“*”
    void docBeModified();

private:
    //私有成员函数

    //当文档被修改时且文件要被关闭时，提示是否保存
    bool promptSave();
    //子窗口关闭事件
    void closeEvent(QCloseEvent* event);

    //私有成员变量

    //用这个参数记录文档是否保存
    bool m_bSaved;
};

#endif // CHILDWIND_H
