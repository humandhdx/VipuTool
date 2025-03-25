#ifndef UNIXSIGNALQHANDLER_H
#define UNIXSIGNALQHANDLER_H

#include <signal.h>
#include <stdio.h>
#include <QApplication>
#include <functional>
#include <list>

namespace UnixSignal {

    std::list<std::function<void(int)>> callbacks_system_interrupt;

    void SignalHanlder(int sig)
    {
        // qApp->exit(0);
        const char msg[] = "system interrupt is comming\r\n";
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);
        for(auto cb: callbacks_system_interrupt)
        {
            cb(sig);
        }
    }

    void Register_SystemInterrupt_Callback(std::function<void(int)> cb)
    {
        callbacks_system_interrupt.emplace_back(cb);
    }

    void Register_Hookers()
    {
        //中断信号 trigger by 'Ctrl+C'
        signal(SIGINT, SignalHanlder);
        //终止信号 trigger by 'kill'
        signal(SIGTERM, SignalHanlder);
        //段错误信号 trigger by segment fault
        signal(SIGSEGV, SignalHanlder);
    }

}

#endif // UNIXSIGNALQHANDLER_H
