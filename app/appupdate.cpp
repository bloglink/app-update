/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       二代板卡内网升级工具
*******************************************************************************/
#include <QApplication>
#include "appupdate.h"

AppUpdate::AppUpdate(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
}

AppUpdate::~AppUpdate()
{
}

void AppUpdate::initUI()
{
    initSkin();
    initTitle();
    initOnline();
    initDevice();
    initButton();
    initLayout();
}

void AppUpdate::initSkin()
{
    QFile file;
    QString qss;
    file.setFileName(":/qss_black.css");
    file.open(QFile::ReadOnly);
    qss = QLatin1String(file.readAll());
    qApp->setStyleSheet(qss);
}

void AppUpdate::initTitle()
{
    char s_month[5];
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    int month, day, year;

    sscanf((__DATE__), "%s %d %d", s_month, &day, &year);
    month = (strstr(month_names, s_month)-month_names)/3+1;

    QDate dt;
    dt.setDate(year, month, day);
    static const QTime tt = QTime::fromString(__TIME__, "hh:mm:ss");

    QDateTime t(dt, tt);
    QString verNumb = QString("V-0.1.%1").arg(t.toString("yyMMdd-hhmm"));

    this->setWindowTitle(tr("二代板卡内网升级工具%1").arg(verNumb));
}

void AppUpdate::initOnline()
{
    mOnline = new QStandardItemModel(this);
    QStringList headers;
    headers << "地址" << "端口" << "版本";
    mOnline->setColumnCount(headers.size());
    for (int i=0; i < headers.size(); i++) {
        mOnline->setHeaderData(i, Qt::Horizontal, headers.at(i));
    }
    onlineView = new QTableView(this);
    onlineView->setModel(mOnline);
    onlineView->verticalHeader()->setVisible(false);
    onlineView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    onlineView->setSelectionBehavior(QAbstractItemView::SelectRows);
    onlineView->setSelectionMode(QAbstractItemView::SingleSelection);
#if (QT_VERSION <= QT_VERSION_CHECK(5, 0, 0))
    onlineView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    onlineView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
}

void AppUpdate::initDevice()
{
    mDevice = new QStandardItemModel(this);
    QStringList header;
    header << "名称" << "地址" << "状态";
    mDevice->setColumnCount(header.size());
    for (int i=0; i < header.size(); i++) {
        mDevice->setHeaderData(i, Qt::Horizontal, header.at(i));
    }
    QStringList boards;
    boards << tr("电阻") << tr("耐压") << tr("匝间") << tr("备用")
           << tr("电感") << tr("功率") << tr("备用") << tr("电源")
           << tr("备用") << tr("备用") << tr("备用") << tr("备用")
           << tr("备用") << tr("备用") << tr("输出1") << tr("输出2");
    QStringList ids;
    ids << "02" << "03" << "04" << "05"
        << "06" << "07" << "08" << "09"
        << "0A" << "0B" << "0C" << "0D"
        << "0E" << "0F" << "13" << "14";
    for (int t=0; t < boards.size(); t++) {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(boards.at(t)));
        items.append(new QStandardItem(ids.at(t)));
        items.append(new QStandardItem(tr("离线")));
        for (int i=0; i < items.size(); i++) {
            items.at(i)->setTextAlignment(Qt::AlignCenter);
        }
        mDevice->appendRow(items);
    }
    deviceView = new QTableView(this);
    deviceView->setModel(mDevice);
    deviceView->verticalHeader()->setVisible(false);
    deviceView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    deviceView->setSelectionBehavior(QAbstractItemView::SelectRows);
    deviceView->setSelectionMode(QAbstractItemView::SingleSelection);
#if (QT_VERSION <= QT_VERSION_CHECK(5, 0, 0))
    deviceView->verticalHeader()->setResizeMode(QHeaderView::Stretch);
    deviceView->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    deviceView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    deviceView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif
}

void AppUpdate::initButton()
{
    btnLayout = new QGridLayout;

    int c = 0;
    QPushButton *btnInit = new QPushButton(tr("开始监听"), this);
    btnInit->setFixedHeight(64);
    connect(btnInit, SIGNAL(clicked(bool)), this, SLOT(initSocket()));
    btnLayout->addWidget(btnInit, 2, c);

    c = btnLayout->columnCount();
    QPushButton *btnOpen = new QPushButton(tr("打开文件"), this);
    btnOpen->setFixedHeight(64);
    connect(btnOpen, SIGNAL(clicked(bool)), this, SLOT(openFile()));
    btnLayout->addWidget(btnOpen, 2, c);

    c = btnLayout->columnCount();
    QPushButton *btnConnect = new QPushButton(tr("连接设备"), this);
    btnConnect->setFixedHeight(64);
    connect(btnConnect, SIGNAL(clicked(bool)), this, SLOT(connectDev()));
    btnLayout->addWidget(btnConnect, 2, c);

    c = btnLayout->columnCount();
    QPushButton *btnUpdate = new QPushButton(tr("升级设备"), this);
    btnUpdate->setFixedHeight(64);
    connect(btnUpdate, SIGNAL(clicked(bool)), this, SLOT(sendUpdate()));
    btnLayout->addWidget(btnUpdate, 2, c);

    c = btnLayout->columnCount();
    QPushButton *btnReboot = new QPushButton(tr("重启设备"), this);
    btnReboot->setFixedHeight(64);
    connect(btnReboot, SIGNAL(clicked(bool)), this, SLOT(sendReboot()));
    btnLayout->addWidget(btnReboot, 2, c);

    c = btnLayout->columnCount();
    text = new QTextBrowser(this);
    btnLayout->addWidget(text, 0, 0, 1, c);

    QDir *dir = new QDir("network");
    QFileInfoList dirs = dir->entryInfoList();
    QStringList names;
    for (int i=0; i < dirs.size(); i++) {
        if (dirs.at(i).fileName().contains(".bin"))
            names.append(dirs.at(i).filePath());
    }
    path = new QComboBox(this);
    path->addItems(names);
    path->setFixedHeight(44);
    btnLayout->addWidget(path, 1, 0, 1, c);
}

void AppUpdate::initLayout()
{
    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addWidget(onlineView);
    leftLayout->addWidget(deviceView);
    leftLayout->setStretch(0, 1);
    leftLayout->setStretch(1, 4);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(leftLayout);
    layout->addLayout(btnLayout);
    layout->setStretch(0, 3);
    layout->setStretch(1, 3);

    QFrame *frame = new QFrame(this);
    frame->setLayout(layout);
    this->setCentralWidget(frame);
    this->resize(800, 600);
}

void AppUpdate::initSocket()
{
    udp = new QUdpSocket(this);
#if (QT_VERSION <= QT_VERSION_CHECK(5, 0, 0))
    udp->bind(QHostAddress::Any, 5999);
#else
    udp->bind(QHostAddress::AnyIPv4, 5999);
#endif
    connect(udp, SIGNAL(readyRead()), this, SLOT(readSocket()));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendSignIn()));
    timer->start(5000);

    hosts = QNetworkInterface::allAddresses();

    QJsonObject msg;
    msg.insert("txCommand", UDP_CONNECT);
    msg.insert("txMessage", "V-0.1");
    udp->writeDatagram(QJsonDocument(msg).toJson(), QHostAddress::Broadcast, 5999);
}

void AppUpdate::readSocket()
{
    while (udp->hasPendingDatagrams()) {
        tmpByte.resize(udp->pendingDatagramSize());
        udp->readDatagram(tmpByte.data(), tmpByte.size(), &sender, &senderPort);
        if (selfSender(sender))  // 自发自收不处理
            return;
        if (!QJsonDocument::fromJson(tmpByte).isObject())  // 非QJsonObject数据不处理
            return;
        QJsonObject msg = QJsonDocument::fromJson(tmpByte).object();
        msg.insert("hostaddr", sender.toString());
        msg.insert("hostport", senderPort);
        quint16 cmd = msg.value("txCommand").toInt();
        qDebug() << "udp recv:" << tmpByte << sender << senderPort << cmd;

        counts[sender.toString()] = 0;

        switch (cmd) {
        case UDP_CONNECT:
            recvSignIn(msg);
            break;
        case UDP_SIGNOUT:
            recvLogout(msg);
            break;
        case UDP_SHOWMSG:
            display(msg.value("txMessage").toString().toUtf8());
            break;
        case UDP_CANDATA:
            recvCanMsg(QByteArray::fromBase64(msg.value("txMessage").toString().toUtf8()));
            break;
        default:
            break;
        }
    }
}

void AppUpdate::sendSignIn()
{
    QJsonObject msg;
    msg.insert("txCommand", UDP_CONNECT);
    msg.insert("txMessage", "V-0.1");
    udp->writeDatagram(QJsonDocument(msg).toJson(), QHostAddress::Broadcast, 5999);

    for (int i=0; i < mOnline->rowCount(); i++) {
        QString hostaddr = mOnline->index(i, 0).data().toString();
        counts[hostaddr]++;
        if (counts[hostaddr] > 5)
            mOnline->removeRow(i);
    }
}

void AppUpdate::sendUdpMsg(QJsonObject msg)
{
    QHostAddress host(msg.value("hostaddr").toString());
    quint16 port = msg.value("hostport").toInt();
    udp->writeDatagram(QJsonDocument(msg).toJson(), host, port);
}

bool AppUpdate::recvSignIn(QJsonObject msg)
{
    for (int i=0; i < mOnline->rowCount(); i++) {
        if (msg.value("hostaddr").toString() == mOnline->index(i, 0).data().toString())
            return false;
    }
    QList<QStandardItem*> items;
    items.append(new QStandardItem(msg.value("hostaddr").toString()));
    items.append(new QStandardItem(QString::number(msg.value("hostport").toInt())));
    items.append(new QStandardItem(msg.value("txMessage").toString()));
    for (int i=0; i < items.size(); i++) {
        items.at(i)->setTextAlignment(Qt::AlignCenter);
    }
    mOnline->appendRow(items);
    return true;
}

bool AppUpdate::recvLogout(QJsonObject msg)
{
    for (int i=0; i < mOnline->rowCount(); i++) {
        if (msg.value("hostaddr").toString() == mOnline->index(i, 0).data().toString()) {
            mOnline->removeRow(i);
            return true;
        }
    }
    return false;
}

bool AppUpdate::selfSender(QHostAddress host)
{
    foreach(QHostAddress address, hosts) {
        if (host.toIPv4Address() == address.toIPv4Address())
            return true;
    }
    return false;
}

void AppUpdate::recvCanMsg(QByteArray msg)
{
    quint16 id;
    quint8 dlc;
    quint8 dat;
    QByteArray dats;
    QDataStream in(&msg,  QIODevice::ReadWrite);
    while (!in.atEnd()) {
        in >> id;
        in >> dlc;
        for (int i=0; i < dlc; i++) {
            in >> dat;
            dats.append(dat);
        }
        QByteArray ids;
        ids.append(id/32);
        qDebug() << "recv udp can data" << id/32;
        for (int i=0; i < mDevice->rowCount(); i++) {
            if (mDevice->index(i, 1).data().toString() == ids.toHex()) {
                mDevice->setData(mDevice->index(i, 2), tr("在线"), Qt::DisplayRole);
            }
        }
    }
}

void AppUpdate::display(QByteArray msg)
{
    text->insertPlainText(msg);
    text->moveCursor(QTextCursor::End);
}

void AppUpdate::connectDev()
{
    int row = onlineView->currentIndex().row();
    if (row < 0)
        return;
    QHostAddress sender;
    sender.setAddress(mOnline->index(row, 0).data().toString());
    int senderPort = mOnline->index(row, 1).data().toInt();
    QJsonObject obj;
    obj.insert("hostaddr", sender.toString());
    obj.insert("hostport", senderPort);
    obj.insert("txCommand", UDP_CANOPEN);
    sendUdpMsg(obj);

    for (int i=0; i < mDevice->rowCount(); i++) {
        mDevice->setData(mDevice->index(i, 2), tr("离线"), Qt::DisplayRole);
    }
}

void AppUpdate::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("打开文件"),
                                                    "/",
                                                    tr("二进制文件(*.bin *.hex)"));
    if (fileName.length() != 0) {
        path->addItem(fileName);
        path->count();
        path->setCurrentIndex(path->count()-1);
    }
}

void AppUpdate::sendUpdate()
{
    int row = onlineView->currentIndex().row();
    if (row < 0)
        return;
    int t = deviceView->currentIndex().row();
    if (t < 0)
        return;
    int id = mDevice->index(t, 1).data().toString().toInt(0, 16);
    QHostAddress sender;
    sender.setAddress(mOnline->index(row, 0).data().toString());
    int senderPort = mOnline->index(row, 1).data().toInt();

    display(tr("正在打开文件......   \n").toUtf8());

    QString filePath = path->currentText();
    QFile file(filePath);
    file.open(QIODevice::ReadOnly);
    QByteArray msg = file.readAll();
    display(tr("正在发送文件......   \n").toUtf8());
    for (int i=0; i <= msg.size()/1024; i++) {
        QJsonObject obj;
        obj.insert("hostaddr", sender.toString());
        obj.insert("hostport", senderPort);
        obj.insert("txCommand", UDP_CANFILE);
        obj.insert("txMessage", QString(msg.mid(i*1024, 1024).toBase64()));
        sendUdpMsg(obj);
    }
    qDebug() << msg.size();
    QJsonObject obj;
    obj.insert("hostaddr", sender.toString());
    obj.insert("hostport", senderPort);
    obj.insert("txCommand", UDP_CANSEND);
    obj.insert("txMessage", msg.size());
    obj.insert("txAddress", id);
    sendUdpMsg(obj);
}

void AppUpdate::sendReboot()
{
    int row = onlineView->currentIndex().row();
    if (row < 0)
        return;
    QHostAddress sender;
    sender.setAddress(mOnline->index(row, 0).data().toString());
    int senderPort = mOnline->index(row, 1).data().toInt();
    QJsonObject obj;
    obj.insert("hostaddr", sender.toString());
    obj.insert("hostport", senderPort);
    obj.insert("txCommand", UDP_RESTART);
    sendUdpMsg(obj);
    text->clear();
}
