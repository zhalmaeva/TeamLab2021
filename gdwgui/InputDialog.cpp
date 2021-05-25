#include <QtGui>
#include "InputDialog.h"
#include<QLineEdit>
#include<QLabel>
#include<QGridLayout>
#include<QPushButton>

InputDialog::InputDialog(QWidget* pwgt/*= 0*/)
     : QDialog(pwgt, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    editServerIp= new QLineEdit;


    QLabel* lServerIp   = new QLabel("&Server Ip:");

    lServerIp->setBuddy(editServerIp);


    QPushButton* btnConnect = new QPushButton("&Connect");
    QPushButton* btnCancel  = new QPushButton("&Cancel");

    connect(btnConnect, SIGNAL(clicked()), SLOT(accept()));
    connect(btnCancel, SIGNAL(clicked()), SLOT(reject()));

    //Layout setup
    QGridLayout* ptopLayout = new QGridLayout;
    ptopLayout->addWidget(lServerIp, 0, 0);
    ptopLayout->addWidget(editServerIp, 0, 1);
    ptopLayout->addWidget(btnConnect, 0, 2);
    ptopLayout->addWidget(btnCancel, 1, 2);
    setLayout(ptopLayout);
}
QString InputDialog::serverIp() const
{
    return editServerIp->text ();
}
