#ifndef UNIXSIGNALQHANDLER_H
#define UNIXSIGNALQHANDLER_H

#include <signal.h>
#include <stdio.h>
#include <QApplication>

namespace UnixSignal {

    void SignalHanlder(int sig)
    {
        // std::cerr << "system signal " << sig << std::endl;
        char msg[34];
        sprintf(msg, "unix signal[%d] coming, quit Qapp\n", sig);
        write(STDOUT_FILENO, msg, sizeof(msg) - 1);
        qApp->exit(0);
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
