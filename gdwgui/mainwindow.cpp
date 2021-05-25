#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QGridLayout>
#include "connectionnotifier.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cw->setLayout(grid);
    ui->scrollArea->setWidget(cw);
    wLabelStatus = 25;
    hLabelStatus = 25;

    lStatusServer.setPixmap(pixDisconnect->scaled(wLabelStatus, hLabelStatus, Qt::KeepAspectRatio));
    ui->statusbar->addWidget(&lStatusServer);

    QObject::connect(ui->btnAddGraph,    &QPushButton::clicked, this, &MainWindow::btnAddGraphClicked);
    QObject::connect(ui->btnClose,       &QPushButton::clicked, this, &MainWindow::close);
    QObject::connect(ui->cwConfiguration,&QUaConfigurationWidget::dateTimeRangeUpdated, this, &MainWindow::serverUpdate);

    QObject::connect(ui->cwConfiguration, &QUaConfigurationWidget::colorChanged, this, &MainWindow::colorChanged);
    QObject::connect(ui->cwConfiguration, &QUaConfigurationWidget::graphsDeleted, this, &MainWindow::graphsDeleted);

    //QObject::connect(&*notifier,         &EleSyOpcUaClient::ConnectionNotifier::connected, this, &MainWindow::serverConnected);
   // QObject::connect(&*notifier,         &EleSyOpcUaClient::ConnectionNotifier::disconnected, this, &MainWindow::serverDisconnected);

    //          Block for server connect
    QObject::connect(ui->actionServer,             &QAction::triggered, this, &MainWindow::connectServer);
    QObject::connect(ui->actionFile,               &QAction::triggered, this, &MainWindow::connectFile);
    //          Block for server connect
}

MainWindow::~MainWindow()
{
    delete grid;
    delete cw;
    delete ui;
    for(auto& graph : graphField) {
        delete graph;
    }
}

void MainWindow::addGraph()
{
    Graph* newGraph { new Graph };
    newGraph->customPlot()->xAxis->setTicker(QSharedPointer<QCPAxisTickerDateTime>(new QCPAxisTickerDateTime));
    newGraph->setField(comboBoxAlg);
    QObject::connect(newGraph, &Graph::close, this, &MainWindow::btnCloseGraphClicked);
    grid->addWidget(newGraph->widget(), gridRow++, 0, 1, 4);
    graphField.push_back(newGraph);
}

void addDataFromServerResponse(QSharedPointer<QCPGraphDataContainer> dest, const std::vector<EleSyOpcUaClient::DataValue>& source)
{
    using EleSyOpcUaClient::UaType;
    for(const auto& dataVal : source) {
        auto key = dataVal.sourceTimeStamp.toUnixMicrosec() / 1000000;
        switch (dataVal.val->v_type()) {
        case UaType::UA_Bool:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_bool())));
            break;
        case UaType::UA_Int8:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_int8())));
            break;
        case UaType::UA_Uint8:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_uint8())));
            break;
        case UaType::UA_Int16:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_int16())));
            break;
        case UaType::UA_Uint16:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_uint16())));
            break;
        case UaType::UA_Int32:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_int32())));
            break;
        case UaType::UA_Uint32:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_uint32())));
            break;
        case UaType::UA_Int64:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_int64())));
            break;
        case UaType::UA_Uint64:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_uint64())));
            break;
        case UaType::UA_Double:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_double())));
            break;
        case UaType::UA_Float:
            dest->add(QCPGraphData(key, static_cast<double>(dataVal.val->v_float())));
            break;
        }
    }
}

MainWindow::storage_t MainWindow::readFromServer(const std::string &nodeId, const QDateTime &start, const QDateTime &end)
{
    storage_t qcpData { new storage_t::value_type };
    using namespace EleSyOpcUaClient;
    UaBytes cPoint { };
    UaBytes cPointNext { };
    auto data = history->getHistoryRawValues(nodeId, 1000000, UaDateTime(start.toMSecsSinceEpoch() * 1000), UaDateTime(end.toMSecsSinceEpoch() * 1000), false, cPoint);
    addDataFromServerResponse(qcpData, data);
    do {
        data = history->getNextHistoryRawValues(nodeId, cPoint, cPointNext);
        addDataFromServerResponse(qcpData, data);
        cPoint = cPointNext;
    } while(!data.empty());
    history->releasePoint(nodeId, cPoint);
    return qcpData;
}

void MainWindow::connectServer()
{
    QString path = " ";
    InputDialog *pInputDialog  = new InputDialog;
    pInputDialog->resize(400, 100);
    if (pInputDialog->exec() == QDialog::Accepted){
       path = pInputDialog->serverIp();
    }
    delete pInputDialog;
    using namespace EleSyOpcUaClient;
    ConnectionParams params;
    params.serverUrl = path.toStdString();      //  from form
    params.connectionName = "Graph Decimator Connection";   //  from form
    createConnection(params, true, true, connection);
    ui->cwServerBrowser->setConnection(connection);
    if(createHistory(connection, history) != uaGood || !connection->isConnected()) {
        serverDisconnected();
    }
    else {
        connection->setConnectionNotifier(notifier);
        serverConnected();
    }
}

void MainWindow::connectFile()
{
    QString path = " ";
    FileDialog *pFileDialog  = new FileDialog;
    pFileDialog->resize(400, 100);
    if (pFileDialog->exec() == QDialog::Accepted){
       path = pFileDialog->filePath();
    }
    delete pFileDialog;
        FILE* file = fopen(path.toLocal8Bit().data(), "r");
        if(!file) {

        }
        size_t  l;
        storage_t qcpData { new storage_t::value_type };
        char*   line { nullptr };
        double  value;
        storage->clear();
        QTime timer;
        timer.start();
        while(!feof(file)) {
            getline(&line, &l, file);
            struct tm t { };
            sscanf(line, "%d.%d.%d %d:%d:%d\t%lf", &t.tm_mday, &t.tm_mon, &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec, &value);
            t.tm_year -= 1900;
            --t.tm_mon;
            qcpData->add(QCPGraphData({ static_cast<double>(mktime(&t)), value }));
        }
        storage->push_back(qcpData);
        ui->lblRdTm->setText(QString::number(timer.elapsed()) + "ms");
        replot();
}


void MainWindow::replot()
{
    for(auto &graph : graphField)  {
        replot(graph);
    }
}

void MainWindow::replot(Graph* graph)
{
    graph->setStorage(storage);
    for(int i = 0; i < graph->customPlot()->graphCount(); ++i) {
        graph->setPen(i, QPen(getColor(i)));
    }
    graph->replot();
}

void MainWindow::graphsDeleted(const std::vector<int> &indexes)
{
    for(const auto &index : indexes) {
        storage->remove(index);
    }
    replot();
}

std::string MainWindow::getNodeId(int index)
{
    return ui->cwConfiguration->list->item(index, 0)->text().toStdString();
}

QColor MainWindow::getColor(int index)
{
    return ui->cwConfiguration->list->color(index);
}

void MainWindow::btnAddGraphClicked(bool checked)
{
    addGraph();
    replot(graphField.back());
}

void MainWindow::btnCloseGraphClicked(Graph* graph)
{
    grid->removeWidget(graph->widget());
    graphField.removeOne(graph);
}

void MainWindow::serverUpdate(const QDateTime &startDateTime, const QDateTime &endDateTime)
{
    //opc.tcp://192.168.56.1:62456
    storage->clear();
    QTime timer;
    timer.start();
    for(auto i = 0; i < ui->cwConfiguration->list->rowCount(); ++i) {
        storage->push_back(readFromServer(getNodeId(i), startDateTime, endDateTime));
    }
    ui->lblRdTm->setText(QString::number(timer.elapsed()) + "ms");
    replot();
}

void MainWindow::serverConnected()
{
    lStatusServer.setPixmap(pixConnect->scaled(wLabelStatus, hLabelStatus, Qt::KeepAspectRatio));
}

void MainWindow::serverDisconnected()
{
    lStatusServer.setPixmap(pixDisconnect->scaled(wLabelStatus, hLabelStatus, Qt::KeepAspectRatio));
}
void MainWindow::colorChanged()
{
    for(auto &graph : graphField)  {
        for(int i = 0; i < graph->customPlot()->graphCount(); ++i) {
            graph->setPen(i, QPen(getColor(i)));
        }
        graph->replot();
    }
}



