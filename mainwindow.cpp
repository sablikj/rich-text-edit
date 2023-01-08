#include "mainwindow.h"
#include "qmdisubwindow.h"
#include "ui_mainwindow.h"
#include "subwindow.h"
#include <QtWidgets>

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

    ui->actionSave->setEnabled(true);
    ui->actionSave_as->setEnabled(true);

#ifndef QT_NO_CLIPBOARD
    //connect(child, &QTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    //connect(child, &QTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
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
    //MainWindow::prependToRecentFiles(fileName);
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
        //MainWindow::prependToRecentFiles(child->currentFile());
    }
}

void MainWindow::on_actionOpen_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        openFile(fileName);
}

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

