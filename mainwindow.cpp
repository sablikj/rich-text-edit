#include "mainwindow.h"
#include "qmdisubwindow.h"
#include "ui_mainwindow.h"
#include "subwindow.h"

#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
        //writeSettings();
        event->accept();
    }
}

Subwindow *MainWindow::createSubwindow()
{
    Subwindow *child = new Subwindow;
    mdiArea->addSubWindow(child);

    // TODO: Rework
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
// Actions
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

void MainWindow::on_actionFont_triggered()
{
    bool success;
    QFont font = QFontDialog::getFont(&success, this);

    if(success){
        if(activeSubwindow()){
            activeSubwindow()->setFont(font);
        }
    }
    else{
        return;
    }
}


void MainWindow::on_actionColor_triggered()
{
    QColor color = QColorDialog::getColor(Qt::black, this, "Choose color");

    if(color.isValid())
    {
        if(activeSubwindow()){
            activeSubwindow()->setTextColor(color);
        }
    }
}


void MainWindow::on_actionBackground_Color_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Choose text background color");

    if(color.isValid())
    {
        if(activeSubwindow()){
            activeSubwindow()->setTextBackgroundColor(color);
        }
    }
}


void MainWindow::on_actionWindow_background_triggered()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "Choose text background color");

    if(color.isValid())
    {
        if(activeSubwindow()){
            activeSubwindow()->setPalette(QPalette(color));
        }
    }
}


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
