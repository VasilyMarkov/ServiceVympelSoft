#ifndef LOGGER_H
#define LOGGER_H
#include <vector>
#include <memory>

class Logger final
{
public:
    static Logger& getInstance();
    void createLog();
    void log(const std::vector<double>&);
    void closeLog();
private:
    Logger();
    ~Logger();
    Logger(const Logger&);
    Logger& operator=(const Logger&);

    std::unique_ptr<std::ofstream> logfile_;
};

#endif // LOGGER_H
