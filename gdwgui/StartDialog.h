#ifndef STARTDIALOG_H
#define STARTDIALOG_H
#include <QtGui>
#include "InputDialog.h"

class StartDialog : public QAction {
Q_OBJECT
public:
    StartDialog(QWidget* pwgt = 0) : Q("Press Me", pwgt)
    {
        connect(this, SIGNAL(clicked()), SLOT(slotButtonClicked()));
    }

public slots:
    void slotButtonClicked()
    {
        InputDialog* pInputDialog = new InputDialog;
        if (pInputDialog->exec() == QDialog::Accepted) {
            QMessageBox::information(0,
                                     "Information",
                                     "First Name: "
                                     + pInputDialog->firstName()
                                     + "\nLast Name: "
                                     + pInputDialog->lastName()
                                    );
        }
        delete pInputDialog;
    }
};
#endif // STARTDIALOG_H
