#include "childwind.h"
#include <QFileInfo>
#include <QFontDatabase>
#include <QFont>
#include <QFileDialog>
#include <QFileInfo>
#include <QTextDocumentWriter>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

ChildWind::ChildWind()
{

    //子窗口关闭时销毁该类的实例对象
    setAttribute(Qt::WA_DeleteOnClose);
    //默认文件没有保存
    m_bSaved = false;
}

//创建新文件
void ChildWind::newDoc()
{
    static int wndSeqNum = 1;
    m_CurDocPath = QString("WPS 文档 %1").arg(wndSeqNum++);

    //[]一种窗口约定，当窗口被修改时，[]里面的内容被指定为修改的内容
    setWindowTitle(m_CurDocPath + "[*]");
    //设置窗体标题，文档改动后名称后加*
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(docBeModified()));
}

//判断窗口是否被修改
void ChildWind::docBeModified()
{
    //通过文档是否被编辑来判断窗口是否被修改，根据这个状态，窗口系统通常会对窗口标题做出相应的调整，
    setWindowModified(document()->isModified());
}

//尝试保存
bool ChildWind::promptSave()
{
    //如果文档没有被修改，直接退出
    if (!document()->isModified()) return true;
    //定义一个标准按钮
    QMessageBox::StandardButton result;
    result = QMessageBox::warning(this,
                                QString("系统提示"),
                                QString("文档%1已修改，是否保存？")
                                .arg(getCurDocName()),
                                QMessageBox::Yes |
                                QMessageBox::Discard |
                                QMessageBox::No);
    if (result == QMessageBox::Yes)
    {
        return saveDoc();
    }
    else if(result == QMessageBox::No)
    {
        return false;
    }
    return true;

}

//子窗口保存文档
bool ChildWind::saveDoc()
{
    //如果当前文档保存了，执行保存操作，否则执行另存为操作
    if (m_bSaved) return saveDocOpt(m_CurDocPath);
    else return saveAsDoc();
}

//子窗口文档另存为
bool ChildWind::saveAsDoc()
{
    QString docName = QFileDialog::getSaveFileName(this,
                                                   "另存为",
                                                   m_CurDocPath,
                                                   "HTML文档 (*.htm *.html);;"
                                                   "所有文件(*.*)");
    //如果文件是空的，则不保存
    if(docName.isEmpty()) return false;
    else return saveDocOpt(docName);

}

//保存文件的具体操作
bool ChildWind::saveDocOpt(QString docName)
{
    // //判断文档是否是htm或者html文件
    // if(!(docName.endsWith(".htm", Qt::CaseInsensitive) ||
    //     docName.endsWith(".html", Qt::CaseInsensitive))) {
    //     docName += ".html";
    // }
    // QFileInfo fileInfo(docName);
    // // 获取文件后缀
    // QString fileSuffix = fileInfo.suffix();

    //
    QTextDocumentWriter writer(docName);
    bool isSuccess = writer.write(this->document());
    //如果写入成功
    if(isSuccess) setCurDoc(docName);
    return isSuccess;
}

//从文件路径得到文件的名称
QString ChildWind::getCurDocName()
{
    QFileInfo fileInfo(m_CurDocPath);
    QString fileName = fileInfo.fileName(); // 返回 "file.txt"
    return fileName;
}

//加载文档内容
bool ChildWind::loadDoc(const QString& docName)
{
    //判断传入的参数是否为空
    if(!docName.isEmpty()) {
        QFile file(docName);
        //文件不存在
        if(!file.exists()) return false;
        //文件打开失败
        if(!file.open(QFile::ReadOnly)) return false;
        //返回文件内容的字节数组
        QByteArray text = file.readAll();
        // 获取文件路径
        QString filePath = file.fileName();
        // 根据文件扩展名判断文件类型
        QString extension = QFileInfo(filePath).suffix().toLower();

        if (extension == "html" || extension == "htm" || extension == "xhtml") {
            // 如果文件类型是 HTML 格式，则将其解析为 HTML 格式显示
            setHtml(text);
        } else if (extension == "txt" || extension == "text" || extension.isEmpty()) {
            // 如果文件类型是纯文本格式，或者文件没有扩展名，则将其解析为纯文本显示
            setPlainText(text);
        //下面暂时没有实现
        } else if (extension == "xls" || extension == "xlsx" || extension == "csv") {
            // 如果文件类型是 Excel 表格格式，则可以使用相应的方式打开
            // 例如，您可以调用打开 Excel 文件的函数
            // openExcelFile(filePath);
        } else if (extension == "ppt" || extension == "pptx") {
            // 如果文件类型是 PowerPoint 演示文稿格式，则可以使用相应的方式打开
            // 例如，您可以调用打开 PowerPoint 文件的函数
            //openPowerPointFile(filePath);
        } else if (extension == "pdf") {
            // 如果文件类型是 PDF 格式，则可以使用相应的方式打开
            // 例如，您可以调用打开 PDF 文件的函数
            //openPdfFile(filePath);
        } else if (extension == "jpg" || extension == "jpeg" || extension == "png" || extension == "gif") {
            // 如果文件类型是图片格式，则可以显示图片
            // 例如，您可以调用显示图片的函数
            // displayImage(filePath);
        } else {
            // 对于其他未知类型的文件，您可以根据需要进行处理
            // 例如，您可以显示一个提示信息，告知用户该文件类型不受支持
            qDebug() << "error,文件类型不支持";
        }

        setCurDoc(docName);
        //文档被修改时，会在文档标题前加一个"*"
        connect(document(),SIGNAL(contentsChanged()),this,SLOT(docBeModified()));
        return true;

    }
    return false;
}

//设置当前文档，与打开文档相互配合使用
void ChildWind::setCurDoc(const QString &docName)
{
    //canonicalFilePath()返回标准名称路径，可以过滤".",".."
    m_CurDocPath = QFileInfo(docName).canonicalFilePath();
    //设置文档已保存
    m_bSaved = true;
    //设置文档未改动
    document()->setModified(false);
    //设置窗口未发生改动
    setWindowModified(false);
    setWindowTitle(getCurDocName() + "[*]");//设置子窗口标题
}

//设置字体格式
void ChildWind::setFormatOnSelectedWord(const QTextCharFormat &format)
{
    //将文件光标赋值给tcursor
    QTextCursor tcursor  = textCursor();
    //检查当前文本光标是否处于一个复杂的选择状态。复杂选择状态指的是选择范围跨越了多个块（blocks）或行。
    //如果光标处于复杂选择状态，那么 hasComplexSelection() 返回 true；否则返回 false。

    if (!tcursor.hasComplexSelection()) {
        //选择处于复杂选择状态下的文字
        tcursor.select(QTextCursor::WordUnderCursor);
    }
    //设置格式
    //将指定的字符格式 format 与文本光标 tcursor 当前所在位置的字符格式进行合并。
    tcursor.mergeCharFormat(format);
    //将指定的字符格式 format 应用于文本光标 tcursor 当前所在位置的文本。
    mergeCurrentCharFormat(format);
}

//设置对其方式
void ChildWind::setAlignOfDocumentText(int aligntype)
{
    //设置1左，2右，3左右，4居中
    if (aligntype == 1)
    {
        setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    }
    else if (aligntype == 2)
    {
        setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    }
    else if(aligntype == 3)
    {
        setAlignment(Qt::AlignCenter);
    }
    else if(aligntype == 4)
    {
        setAlignment(Qt::AlignJustify);
    }
}


//子窗口关闭事件
void ChildWind::closeEvent(QCloseEvent *event)
{

    if (promptSave()) {
        event->accept();
    }
    else{
        event->ignore();
    }
}




