/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       二代板卡内网升级工具
*******************************************************************************/
#ifndef APPUPDATE_H
#define APPUPDATE_H

#include <QMainWindow>
#include <QDir>
#include <QTimer>
#include <QLayout>
#include <QWidget>
#include <QDateTime>
#include <QComboBox>
#include <QTableView>
#include <QHeaderView>
#include <QPushButton>
#include <QTextBrowser>
#include <QFileDialog>
#include <QApplication>
#include <QStandardItemModel>
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcess>
#include <QDataStream>

#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkInterface>

#define UDP_CONNECT 100
#define UDP_SIGNOUT 101
#define UDP_CANOPEN 102
#define UDP_SHOWMSG 103
#define UDP_CANDATA 104
#define UDP_CANFILE 105
#define UDP_CANSEND 106
#define UDP_RESTART 107

class AppUpdate : public QMainWindow
{
    Q_OBJECT

public:
    explicit AppUpdate(QWidget *parent = 0);
    ~AppUpdate();
private slots:
    void initUI();
    void initSkin();
    void initTitle();
    void initOnline();
    void initDevice();
    void initButton();
    void initLayout();
    void initSocket();
    void readSocket();
    void sendSignIn();
    void sendUdpMsg(QJsonObject msg);
    bool recvSignIn(QJsonObject msg);
    bool recvLogout(QJsonObject msg);
    bool selfSender(QHostAddress host);
    void recvCanMsg(QByteArray msg);
    void openFile();
    void connectDev();
    void sendUpdate();
    void sendReboot();
    void display(QByteArray msg);
private:
    QTableView *onlineView;
    QStandardItemModel *mOnline;
    QTableView *deviceView;
    QStandardItemModel *mDevice;
    QTextBrowser *text;
    QComboBox *path;
    QGridLayout *btnLayout;

    QHostAddress txHost;
    QMap<QString, int> counts;
    QHostAddress sender;
    quint16 senderPort;
    QByteArray tmpByte;
    QList<QHostAddress> hosts;

    QUdpSocket *udp;
};

#endif // APPUPDATE_H
