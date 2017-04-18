#include <QCoreApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QLoggingCategory>
#include <QString>
#include <QTimer>
#include <functional>
#include <memory>

namespace {
Q_LOGGING_CATEGORY(appLog, "app")
Q_LOGGING_CATEGORY(serverLog, "app.server")
Q_LOGGING_CATEGORY(serverSocketLog, "app.server.socket")
Q_LOGGING_CATEGORY(serverWorkLog, "app.server.work")
Q_LOGGING_CATEGORY(clientLog, "app.client")
Q_LOGGING_CATEGORY(clientSocketLog, "app.client.socket")
bool openLocalServer();
bool writeToLocalServer(const QByteArray &data);
QString parse(const QStringList &arguments, bool process = false);
void setupMainRoutine();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationVersion(QStringLiteral("1.0"));
    if (writeToLocalServer(a.arguments().join(' ').toUtf8())) {
        return EXIT_SUCCESS;
    }
    parse(a.arguments(), true);
    openLocalServer();
    setupMainRoutine();

    return a.exec();
}

namespace {
const QString serverName = QLatin1String{"mysockettestserver"};
bool openLocalServer()
{
    std::unique_ptr<QLocalServer> server{new QLocalServer};
    if (!server->listen(serverName)) {
        qCWarning(appLog) << QStringLiteral("Open server failed name '%1'").arg(serverName);
        return false;
    }
    qCInfo(serverLog) << QStringLiteral("Open server name '%1'").arg(serverName);
    auto ptr = server.get();
    server->connect(ptr, &QLocalServer::newConnection, [ptr]{
        auto socket = ptr->nextPendingConnection();
        socket->connect(socket, &QLocalSocket::readyRead, [socket]{
            auto response = parse(QString::fromUtf8(socket->readAll()).split(' '));
            qCInfo(serverSocketLog).noquote() << "WriteData :" << response;
            qCInfo(serverSocketLog).resetFormat();
            socket->write(response.toUtf8());
        });
    });
    server->connect(server.get(), &QLocalServer::destroyed, []{
        qCInfo(serverLog) << "Server closed";
    });
    server.release()->setParent(qApp);
    return true;
}

bool writeToLocalServer(const QByteArray &data)
{
    qCDebug(appLog) << QStringLiteral("Connect to server name '%1'").arg(serverName);
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if (!socket.waitForConnected(300)) {
        qCDebug(appLog) << QStringLiteral("Server '%1' is not found").arg(serverName);
        return false;
    }
    qCInfo(clientLog) << QStringLiteral("Connected to server name '%1'").arg(serverName);
    qCInfo(clientSocketLog) << "Write data :" << data;
    socket.write(data);
    if (!socket.waitForBytesWritten(300)) {
        qCWarning(clientSocketLog) << socket.error() << socket.errorString();
    }
    if (!socket.waitForReadyRead(300)) {
        qCWarning(clientSocketLog) << "No response" << socket.error()
                                   << socket.errorString();
    }
    qCInfo(clientLog).noquote() << "Response received :" << socket.readAll();
    return true;
}

QString parse(const QStringList &arguments, bool process)
{
    QCommandLineParser parser;
    auto help = parser.addHelpOption();
    auto version = parser.addVersionOption();
    parser.setApplicationDescription(QStringLiteral("QLocalSocket test app."));
    parser.addOption({QStringLiteral("message-pattern"),
                      QStringLiteral("For more information show Qt help about qSetMessagePattern."),
                      QStringLiteral("PATTERN")
                     });
    parser.addOption({QStringLiteral("logging-rules"),
                      QStringLiteral("For more information show Qt help about QLoggingCategory Class."),
                      QStringLiteral("RULES")
                     });
    bool ok = true;
    if (process)
        parser.process(arguments);
    else
        ok = parser.parse(arguments);

    if (!ok || parser.isSet(help))
        return parser.helpText();
    if (parser.isSet(version))
        return qApp->applicationVersion();

    QStringList result;
    if (parser.isSet(QStringLiteral("message-pattern"))) {
        auto pattern = parser.value(QStringLiteral("message-pattern"));
        qCInfo(serverLog) << "Set message pattern :" << pattern;
        qSetMessagePattern(pattern);
        result << QStringLiteral("Message pattern is set :") + pattern;
    }
    if (parser.isSet(QStringLiteral("logging-rules"))) {
        auto rules = parser.value(QStringLiteral("logging-rules"));
        qCInfo(serverLog) << "Set logging rules :" << rules;
        QLoggingCategory::setFilterRules(rules);
        result << QStringLiteral("Logging rules is set :") + rules;
    }
    return result.join('\n');
}

void setupTimer(int interval, std::function<void()> func)
{
    auto timer = new QTimer{qApp};
    timer->connect(timer, &QTimer::timeout, func);
    timer->connect(qApp, &QCoreApplication::aboutToQuit, &QTimer::deleteLater);
    timer->start(interval);
}

void setupMainRoutine()
{
    setupTimer(50, []{qCDebug(serverWorkLog) << "debug message.";});
    setupTimer(300, []{qCInfo(serverWorkLog) << "info message.";});
    setupTimer(3000, []{qCWarning(serverWorkLog) << "warning message.";});
}
} // unnamed namespace
