#include "formatter/PlainTextFormatter.cpp"
#include "appenders/ConsoleAppender.cpp"
#include "appenders/FileAppender.cpp"
#include "handlers/InfoHandler.cpp"
#include "handlers/WarnHandler.cpp"
#include "handlers/ErrorHandler.cpp"
#include "logger/Logger.cpp"

int main(){
    // Formatter
    LogFormatter* formatter = new PlainTextFormatter();

    // Appenders
    LogAppender* console = new ConsoleAppender(formatter);
    LogAppender* file = new FileAppender(formatter, "logs.txt");

    // Handlers
    InfoHandler info;
    WarnHandler warn;
    ErrorHandler error;

    info.addAppender(console);

    warn.addAppender(console);
    warn.addAppender(file);

    error.addAppender(console);
    error.addAppender(file);

    info.setNext(&warn);
    warn.setNext(&error);

    // Logger
    Logger& logger = Logger::getInstance();
    logger.setHandler(&info);

    // Test
    logger.info("Application Started");

    logger.warn("Memory Usage High");

    logger.error("Database Connection Failed");
}