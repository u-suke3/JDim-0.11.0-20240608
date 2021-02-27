// ライセンス: GPL2

//
// BEログイン管理クラス
//
// セッション管理やログイン、パスワードの保存などを行う
//

#ifndef _LOGINBE_H
#define _LOGINBE_H

#include "skeleton/login.h"

#include <string>


namespace CORE
{
    class LoginBe : public SKELETON::Login
    {
        std::string m_rawdata;

      public:

        LoginBe();
        ~LoginBe() = default;

        void start_login() override;
        void logout() override;

      private:

        void receive_data( const char* , size_t ) override;
        void receive_finish() override;
    };


    LoginBe* get_loginbe();
    void delete_loginbe();
}


#endif
