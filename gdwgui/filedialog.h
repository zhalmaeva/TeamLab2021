#ifndef FILEDIALOG_H
#define FILEDIALOG_H
#include <QDialog>
#include<QLabel>
class QLineEdit;
class FileDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit *editFilePath;

public:
    FileDialog(QWidget* pwgt = 0);

    QString filePath() const;
};
#endif // FILEDIALOG_H
