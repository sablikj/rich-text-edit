#include "subwindow.h"
#include <QtWidgets>

Subwindow::Subwindow()
{
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
}

void Subwindow::newFile()
{
    static int sequenceNumber = 1;
    isUntitled = true;
    curFile = tr("document%1.rtf").arg(sequenceNumber++);
    setWindowTitle(curFile + "[*]");
}

bool Subwindow::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool Subwindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), curFile);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool Subwindow::saveFile(const QString &fileName)
{
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << toPlainText();
        if (!file.commit()) {
            errorMessage = tr("Cannot write file %1:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    } else {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                       .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty()) {
        QMessageBox::warning(this, tr("RichTextEditor"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    return true;
}

bool Subwindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("MDI"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream in(&file);
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    setPlainText(in.readAll());
    QGuiApplication::restoreOverrideCursor();

    setCurrentFile(fileName);

    connect(document(), &QTextDocument::contentsChanged,
            this, &Subwindow::documentWasModified);

    return true;
}

QString Subwindow::userFriendlyCurrentFile()
{
    return QFileInfo(curFile).fileName();
}

void Subwindow::documentWasModified()
{
    setWindowModified(document()->isModified());
}

void Subwindow::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}
