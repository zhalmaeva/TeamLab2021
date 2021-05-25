#include <QtGui>
#include "filedialog.h"
#include<QLineEdit>
#include<QLabel>
#include<QGridLayout>
#include<QPushButton>

FileDialog::FileDialog(QWidget* pwgt/*= 0*/)
     : QDialog(pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    editFilePath= new QLineEdit;


    QLabel* lFilePath  = new QLabel("&File path: ");

    lFilePath->setBuddy(editFilePath);


    QPushButton* btnConnect = new QPushButton("&Open");
    QPushButton* btnCancel  = new QPushButton("&Cancel");

    connect(btnConnect, SIGNAL(clicked()), SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), SLOT(reject()));

    //Layout setup
    QGridLayout* ptopLayout = new QGridLayout;
    ptopLayout->addWidget(lFilePath, 0, 0);
    ptopLayout->addWidget(editFilePath, 0, 1);
    ptopLayout->addWidget(btnConnect, 0, 2);
    ptopLayout->addWidget(btnCancel, 1, 2);
    setLayout(ptopLayout);
}
QString FileDialog::filePath() const
{
    return editFilePath->text ();
}
