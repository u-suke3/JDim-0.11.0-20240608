// License: GPL2
//
// FIFOを使ったプロセス間通信を行うクラス
//

#ifndef _IOMONITOR_H
#define _IOMONITOR_H

#include <gtkmm.h>

namespace CORE
{
    class IOMonitor
    {
        // FIFOのファイルディスクリプタ
        int m_fifo_fd;

        // FIFOファイル名
        std::string m_fifo_file;

        // I/Oの架け橋
        Glib::RefPtr< Glib::IOChannel > m_iochannel;

        // メインプロセスか否か
        bool m_main_process;

      private:

        // 初期化
        void init();

        // FIFOに書き込まれたら呼び出される
        bool slot_ioin( Glib::IOCondition io_condition );

      public:

        IOMonitor();
        ~IOMonitor();

        // メインプロセスか否かを取得
        bool is_main_process(){ return m_main_process; }

        // FIFOに書き込み
        bool send_command( const char* command );
    };
}

#endif