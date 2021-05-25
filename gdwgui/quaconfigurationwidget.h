    #pragma once
#include <QListWidget>
#include <QDateTimeEdit>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QTableWidget>
#include "util.h"

class Tmp : public QTableWidget
{
    Q_OBJECT
public:
    Tmp(QWidget *parent = nullptr) : QTableWidget(parent) {}

    QColor color(int row) { return QColor(dynamic_cast<QComboBox*>(cellWidget(row, 1))->currentText()); }

protected:
    virtual void dropEvent(QDropEvent *event) override {
        int currentRow = rowCount();
        dropMimeData(currentRow, 0, event->mimeData(), Qt::DropAction::CopyAction);
        auto cb = new QComboBox;
        cb->setMaximumWidth(150);
        cb->addItems({
                         "red",
                         "green",
                         "blue",
                         "black",
                         "yellow"
                     });
        setCellWidget(currentRow, 1, cb);
        connect(cb, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Tmp::currentColorChanged);
    }

private slots:
    void currentColorChanged(int index)
    {
        emit colorChanged();
    }
signals:
    void colorChanged();
};

class QUaConfigurationWidget : public QWidget
{
    Q_OBJECT
public:
    QGridLayout*    grid        { new QGridLayout(this) };
    Tmp*            list        { new Tmp };
    QLabel*         lStart      { new QLabel("Start time:") };
    QLabel*         lEnd        { new QLabel("End time:") };
    QDateTimeEdit*  dteStart    { new QDateTimeEdit };
    QDateTimeEdit*  dteEnd      { new QDateTimeEdit };
    QPushButton*    btnUpdate   { new QPushButton("Update") };

public:
    explicit QUaConfigurationWidget(QWidget *parent = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
signals:
    void dateTimeRangeUpdated(const QDateTime &startDateTime, const QDateTime &endDateTime);
    void graphsDeleted(const std::vector<int> &indexes);
    void colorChanged();

private slots:
    void buttonUpdateClicked(bool checked);
    void currentColorChanged();
};
