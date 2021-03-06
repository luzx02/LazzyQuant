#include <QCoreApplication>
#include <QCommandLineParser>

#include "config.h"
#include "market.h"
#include "message_handler.h"
#include "market_watcher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setApplicationName("market_watcher");
    QCoreApplication::setApplicationVersion(VERSION_STR);

    QCommandLineParser parser;
    parser.setApplicationDescription("Receive and save market data, emit signal via D-Bus when market changes");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
        {{"r", "replay"},
            QCoreApplication::translate("main", "Replay Mode")},
        {{"f", "logtofile"},
            QCoreApplication::translate("main", "Save log to a file")},
    });

    parser.process(a);
    bool replayMode = parser.isSet("replay");
    bool log2File = parser.isSet("logtofile");
    setupMessageHandler(true, log2File, "market_watcher");

    if (!replayMode)
        loadCommonMarketData();

    QList<MarketWatcher*> watcherList;
    for (const auto & config : watcherConfigs) {
        MarketWatcher *pWatcher = new MarketWatcher(config, replayMode);
        watcherList.append(pWatcher);
    }

    int ret = a.exec();

    for (const auto & pWatcher : qAsConst(watcherList)) {
        delete pWatcher;
    }
    restoreMessageHandler();
    return ret;
}
