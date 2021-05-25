#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H
#include <QDialog>
#include<QLabel>
class QLineEdit;
class InputDialog : public QDialog {
    Q_OBJECT
private:
    QLineEdit *editServerIp;

public:
    InputDialog(QWidget* pwgt = 0);

    QString serverIp() const;
};
#endif // INPUTDIALOG_H
