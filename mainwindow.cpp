#include "mainwindow.h"
#include "qmdisubwindow.h"
#include "ui_mainwindow.h"
#include "subwindow.h"

#include <QSettings>
#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);    

    // MDI setup
    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setViewMode(QMdiArea::TabbedView);
    mdiArea->setTabsClosable(true);
    mdiArea->setTabsMovable(true);
    mdiArea->setDocumentMode(true);
    setCentralWidget(mdiArea);

    // Drag and Drop
    setAcceptDrops(true);

    // Find dialog setup
    findDialog = new FindDialog();
    findDialog->setParent(this, Qt::Tool | Qt::MSWindowsFixedSizeDialogHint);    
}

MainWindow::~MainWindow()
{
    delete ui;    
}

bool MainWindow::openFile(const QString &fileName)
{
    if (QMdiSubWindow *existing = findSubwindow(fileName)) {
        mdiArea->setActiveSubWindow(existing);
        return true;
    }
    const bool succeeded = loadFile(fileName);
    if (succeeded)
        statusBar()->showMessage(tr("File loaded"), 2000);
    return succeeded;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        event->ignore();
    } else {        
        event->accept();
    }
}

Subwindow *MainWindow::createSubwindow()
{
    Subwindow *child = new Subwindow;
    mdiArea->addSubWindow(child);

    ui->actionSave->setEnabled(true);
    ui->actionSave_as->setEnabled(true);

    ui->actionFont->setEnabled(true);
    ui->actionColor->setEnabled(true);
    ui->actionBackground_Color->setEnabled(true);

#ifndef QT_NO_CLIPBOARD
    connect(child, &QTextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    connect(child, &QTextEdit::copyAvailable, ui->actionCut, &QAction::setEnabled);
#endif

    return child;
}

Subwindow *MainWindow::activeSubwindow() const
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<Subwindow *>(activeSubWindow->widget());
    return nullptr;
}

QMdiSubWindow *MainWindow::findSubwindow(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    const QList<QMdiSubWindow *> subWindows = mdiArea->subWindowList();
    for (QMdiSubWindow *window : subWindows) {
        Subwindow *mdiChild = qobject_cast<Subwindow *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
    }
    return nullptr;
}

bool MainWindow::loadFile(const QString &fileName)
{
    Subwindow *child = createSubwindow();
    const bool succeeded = child->loadFile(fileName);
    if (succeeded)
        child->show();
    else
        child->close();
    return succeeded;
}

///////////
// Basic actions
///////////

void MainWindow::on_actionNew_triggered()
{
    Subwindow *child = createSubwindow();
    child->newFile();
    child->show();
}

void MainWindow::on_actionSave_triggered()
{
    if (activeSubwindow() && activeSubwindow()->save())
        statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::on_actionSave_as_triggered()
{
    Subwindow *child = activeSubwindow();
    if (child && child->saveAs()) {
        statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::on_actionOpen_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

void MainWindow::on_actionUndo_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->undo();
    }
}

void MainWindow::on_actionRedo_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->redo();
    }
}

void MainWindow::on_actionClose_triggered()
{
    mdiArea->closeActiveSubWindow();
}

void MainWindow::on_actionClose_all_triggered()
{
    mdiArea->closeAllSubWindows();
}

///////////
// Clipboard actions
///////////

#ifndef QT_NO_CLIPBOARD
void MainWindow::on_actionCopy_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->copy();
    }
}

void MainWindow::on_actionPaste_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->paste();
    }
}

void MainWindow::on_actionCut_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->cut();
    }
}
#endif

///////////
// Font and Color
///////////

void MainWindow::on_actionFont_triggered()
{
    bool success;
    QFont font = QFontDialog::getFont(&success, this);

    if(success){
        if(activeSubwindow()){
            QTextCharFormat format;
            format.setFont(font);
            activeSubwindow()->textCursor().mergeCharFormat(format);
        }
    }
    else{
        return;
    }
}

void MainWindow::on_actionColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Choose text color");

    if(color.isValid())
    {
        if(activeSubwindow()){
            QTextCharFormat format;
            format.setForeground(color);
            activeSubwindow()->textCursor().mergeCharFormat(format);
        }
    }
}

void MainWindow::on_actionBackground_Color_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Choose text background color");

    if(color.isValid())
    {
        if(activeSubwindow()){
            QTextCharFormat format;
            format.setBackground(color);
            activeSubwindow()->textCursor().mergeCharFormat(format);
        }
    }
}

///////////
// Print
///////////

void MainWindow::on_actionPrint_triggered()
{
    QPrinter printer;
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setFullPage(true);

    QPrintPreviewDialog *printPreview = new QPrintPreviewDialog(&printer);
    connect(printPreview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(print(QPrinter*)));

    printPreview->setWindowTitle("Preview Dialog");
    Qt::WindowFlags flags(Qt::WindowTitleHint);
    printPreview->setWindowFlags(flags);
    printPreview->exec();

}

void MainWindow::print(QPrinter *printer)
{
    QPainter painter(printer);
    painter.setRenderHints(QPainter::Antialiasing |
                       QPainter::TextAntialiasing |
                       QPainter::SmoothPixmapTransform, true);

    if(activeSubwindow()){
        QTextDocument *doc = activeSubwindow()->document();
        doc->drawContents(&painter);
    }

    painter.end();
}

///////////
// Find and Replace
///////////

void MainWindow::on_actionFind_triggered()
{
    connect(findDialog, SIGNAL(startFinding(QString, bool, bool)), activeSubwindow(), SLOT(find(QString, bool, bool)));
    connect(findDialog, SIGNAL(startReplacing(QString, QString, bool, bool)), activeSubwindow(), SLOT(replace(QString, QString, bool, bool)));
    connect(findDialog, SIGNAL(startReplacingAll(QString, QString, bool, bool)), activeSubwindow(), SLOT(replaceAll(QString, QString, bool, bool)));
    connect(activeSubwindow(), SIGNAL(findResultReady(QString)), findDialog, SLOT(onFindResultReady(QString)));

    if (findDialog->isHidden())
    {
        findDialog->show();
        findDialog->activateWindow();
        findDialog->raise();
        findDialog->setFocus();
    }
}

void MainWindow::on_actionReplace_triggered()
{
    on_actionFind_triggered();
}

///////////
// Drag and drop
///////////

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
event->acceptProposedAction();
}

 void MainWindow::dropEvent(QDropEvent* event)
 {
    foreach (const QUrl &url, event->mimeData()->urls()) {
         QString fileName = url.toLocalFile();
         qDebug() << "Dropped file:" << fileName;
         if(openFile(fileName))
           event->acceptProposedAction();
    }
}

///////////
// Align
///////////

void MainWindow::on_actionAlign_left_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->align(Qt::AlignLeft);
    }
}

void MainWindow::on_actionAlign_center_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->align(Qt::AlignCenter);
    }
}

void MainWindow::on_actionAlign_right_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->align(Qt::AlignRight);
    }
}

///////////
// Lists
///////////

void MainWindow::on_actionList_bullet_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->list(QTextListFormat::ListDisc);
    }
}

void MainWindow::on_actionList_numbered_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->list(QTextListFormat::ListDecimal);
    }
}

void MainWindow::on_actionList_letter_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->list(QTextListFormat::ListLowerAlpha);
    }
}

void MainWindow::on_actionList_roman_triggered()
{
    if(activeSubwindow()){
        activeSubwindow()->list(QTextListFormat::ListUpperRoman);
    }
}
