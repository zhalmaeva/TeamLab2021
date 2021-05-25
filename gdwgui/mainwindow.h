#pragma once
#include <QMainWindow>
#include "datadecimator.h"
#include "graph.h"
#include "quabrowser.h"
#include <opcuaclient/UaHistory.h>
#include "connectionnotifier.h"
#include <QtGui>
#include "InputDialog.h"
#include "filedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    using storage_t = QSharedPointer<QCPGraphDataContainer>;
private:
    EleSyOpcUaClient::UaConnection::Ptr         connection;
    EleSyOpcUaClient::UaHistory::Ptr            history;
    EleSyOpcUaClient::ConnectionNotifier::Ptr   notifier { new EleSyOpcUaClient::ConnectionNotifier };

private:
    Ui::MainWindow* ui;
    QWidget*        cw          { new QWidget };
    QGridLayout*    grid        { new QGridLayout };
    QSize           graphSize   {10, 6};
    size_t          gridRow     { };
    size_t          indWay      {0};
    QList<Graph*>   graphField  { };
    QVector<storage_t>*       storage { new QVector<storage_t> };
    QLabel          lStatusServer {};
    QPixmap*        pixDisconnect {new QPixmap("../disconnect.png")};
    QPixmap*        pixConnect    {new QPixmap("../connect.png")};
    int wLabelStatus {0};
    int hLabelStatus {0};

    QVector<AlgorithmComboBoxEl> comboBoxAlg {
            {
                [](const storage_t& data, double sv) { return storage_t{ new storage_t::value_type(*data) }; },
                " None" ,
                [](const QCPRange& xRange, const QCPRange& yRange) { return 0; }
            },
            {
                DataDecimator::douglas_peucker,
                "Douglas-Peucker",
                [](const QCPRange& xRange, const QCPRange& yRange) { return std::pow(yRange.upper - yRange.lower, 1/6); }
            },
            {
                DataDecimator::reumann_witkam,
                "Reumann-Witkam",
                [](const QCPRange& xRange, const QCPRange& yRange) { return (yRange.upper - yRange.lower) / 50.0; }
            },
            {
                DataDecimator::largest_triangle,
                "Largest triangle",
                [](const QCPRange& xRange, const QCPRange& yRange) { return xRange.upper - xRange.lower; }
            },
            {
                DataDecimator::longest_line,
                "Longest line",
                [](const QCPRange& xRange, const QCPRange& yRange) { return xRange.upper - xRange.lower; }
            },
            {
                DataDecimator::opheim,
                "Opheim",
                [](const QCPRange& xRange, const QCPRange& yRange) { return (xRange.upper - xRange.lower) / 700; }
            },
            {
                DataDecimator::lang,
                "Lang",
                [](const QCPRange& xRange, const QCPRange& yRange) { return (yRange.upper - yRange.lower) / 40.0; }
            },
            {
                DataDecimator::swinging_door,
                "Swinging door",
                [](const QCPRange& xRange, const QCPRange& yRange) { return (yRange.upper - yRange.lower) / 80.0; }
            }
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool read_from_file(const QString& path);
    void addGraph();
    storage_t readFromServer(const std::string &nodeId, const QDateTime& start, const QDateTime& end);

    void connectServer();
    void connectFile();

    std::string getNodeId(int index);
    QColor      getColor(int index);

public slots:
    void replot();
    void replot(Graph* graph);

    void graphsDeleted(const std::vector<int> &indexes);

    void btnAddGraphClicked(bool);
    void btnCloseGraphClicked(Graph*);

    void serverUpdate(const QDateTime &startDateTime, const QDateTime &endDateTime);

    void serverConnected();
    void serverDisconnected();

    void colorChanged();
};
