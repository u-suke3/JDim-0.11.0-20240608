// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "messageadmin.h"
#include "post.h"
#include "confirmdiag.h"

#include "skeleton/msgdiag.h"

#include "jdlib/loaderdata.h"
#include "jdlib/jdiconv.h"
#include "jdlib/miscmsg.h"
#include "jdlib/miscutil.h"
#include "jdlib/jdregex.h"

#include "dbtree/interface.h"

#include "config/globalconf.h"

#include "httpcode.h"

#include <cstring>


namespace {

// PostStrategyの実装
// newするのは大げさなので静的変数を定義した

// 書き込み用のインターフェース
struct WriteStrategy : public MESSAGE::PostStrategy
{
    WriteStrategy() noexcept = default;
    ~WriteStrategy() noexcept override = default;
    std::string url_bbscgi( const std::string& url ) override { return DBTREE::url_bbscgi( url ); }
    std::string url_subbbscgi( const std::string& url ) override { return DBTREE::url_subbbscgi( url ); }

    std::string get_referer( const std::string& url ) const override { return DBTREE::get_write_referer( url ); }

} s_write_strategy;


// スレ立て用のインターフェース
struct NewArticleStrategy : public MESSAGE::PostStrategy
{
    NewArticleStrategy() noexcept = default;
    ~NewArticleStrategy() noexcept override = default;
    std::string url_bbscgi( const std::string& url ) override { return DBTREE::url_bbscgi_new( url ); }
    std::string url_subbbscgi( const std::string& url ) override { return DBTREE::url_subbbscgi_new( url ); }

    std::string get_referer( const std::string& url ) const override { return DBTREE::get_newarticle_referer( url ); }

} s_new_article_strategy;

} // namespace


using namespace MESSAGE;

enum
{
    SIZE_OF_RAWDATA = 2 * 1024 * 1024
};

Post::Post( Gtk::Widget* parent, const std::string& url, const std::string& msg, bool new_article )
    : SKELETON::Loadable()
    , m_parent( parent )
    , m_url( url )
    , m_msg( msg )
    , m_new_article( new_article )
{
#ifdef _DEBUG
    std::cout << "Post::Post " << m_url << std::endl;
#endif

    if( new_article ) m_post_strategy = &s_new_article_strategy;
    else m_post_strategy = &s_write_strategy;

    clear();
}



Post::~Post()
{
#ifdef _DEBUG
    std::cout << "Post::~Post " << m_url << std::endl;
#endif

    clear();
}


void Post::clear()
{
#ifdef _DEBUG
    std::cout << "Post::clear\n";
#endif

    m_rawdata.clear();
    m_rawdata.shrink_to_fit();

    if( m_writingdiag ) m_writingdiag->hide();
}


void Post::emit_sigfin()
{
#ifdef _DEBUG
    std::cout << "Post::emit_sigfin\n";
#endif

    m_sig_fin.emit();

    clear();

    m_writingdiag.reset();
}


//
// 書き込み中ダイアログ表示
//
void Post::show_writingdiag( const bool show_buttons )
{
    Gtk::Window* toplevel = dynamic_cast< Gtk::Window* >( m_parent->get_toplevel() );
    if( ! toplevel ) return;

    Gtk::ButtonsType buttons = Gtk::BUTTONS_NONE;
    if( show_buttons ) buttons = Gtk::BUTTONS_OK;

    if( ! m_writingdiag ){
        m_writingdiag = std::make_unique<SKELETON::MsgDiag>( *toplevel, "書き込み中・・・", false, Gtk::MESSAGE_INFO,
                                                             buttons, false );
        m_writingdiag->signal_response().connect( sigc::mem_fun( *this, &Post::slot_response ) );
    }
    m_writingdiag->show();

    // gtkのバージョンによってはラベルが選択状態になっている場合があるので
    // 選択状態を解除する
    Gtk::Label *label = dynamic_cast< Gtk::Label* >( m_writingdiag->get_focus() );
    if( label ) label->set_selectable( false );
}


//
// 書き込み中ダイアログのボタンを押した
//
void Post::slot_response( int id )
{
#ifdef _DEBUG
    std::cout << "Post::slot_response id = " << id << std::endl;
#endif

    if( m_writingdiag ) m_writingdiag->hide();
}


//
// ポスト実行
//
void Post::post_msg()
{
    if( is_loading() ) return;

    clear();
    m_rawdata.reserve( SIZE_OF_RAWDATA );

    // 書き込み中ダイアログ表示
    if( ! CONFIG::get_hide_writing_dialog() ) show_writingdiag( false );

    JDLIB::LOADERDATA data;

    if( ! m_subbbs ) data.url = m_post_strategy->url_bbscgi( m_url ); // 1回目の投稿先
    else data.url = m_post_strategy->url_subbbscgi( m_url ); // 2回目の投稿先

    // Content-Type (2009/02/18に報告された"したらば"に書き込めない問題で追加)
    // http://www.asahi-net.or.jp/~sd5a-ucd/rec-html401j/interact/forms.html#h-17.13.4.1
    // http://www.w3.org/TR/html401/interact/forms.html#h-17.13.4.1
    data.contenttype = "application/x-www-form-urlencoded";
    // trueならUTF-8で書き込む
    if( DBTREE::board_check_utf8_post( m_url ) ) {
        data.contenttype.append( "; charset=utf-8" );
    }

    data.agent = DBTREE::get_agent_w( m_url );
    data.referer = m_count < 1 ? m_post_strategy->get_referer( m_url ) : m_post_strategy->url_bbscgi( m_url );
    // WebブラウザのUAならOriginを含める
    if( data.agent.rfind( "Mozilla/5.0", 0 ) == 0 ) {
        data.origin = MISC::get_hostname( m_url );
    }
    data.str_post = m_msg;
    data.host_proxy = DBTREE::get_proxy_host_w( m_url );
    data.port_proxy = DBTREE::get_proxy_port_w( m_url );
    data.basicauth_proxy = DBTREE::get_proxy_basicauth_w( m_url );
    data.size_buf = CONFIG::get_loader_bufsize();
    data.timeout = CONFIG::get_loader_timeout_post();
    data.cookie_for_request = DBTREE::board_cookie_for_post( m_url );
    data.basicauth = DBTREE::board_basicauth( m_url );

#ifdef _DEBUG
    std::cout << "Post::post_msg : " << std::endl
              << "url = " << data.url << std::endl
              << "contenttype = " << data.contenttype << std::endl
              << "agent = " << data.agent << std::endl
              << "origin = " << data.origin << std::endl
              << "referer = " << data.referer << std::endl
              << "cookie = " << data.cookie_for_request << std::endl
              << "proxy = " << data.host_proxy << ":" << data.port_proxy << std::endl
              << m_msg << std::endl;
#endif

    if( data.url.empty() ) return;

    start_load( data );
}



//
// ローダからデータを受け取る
//
void Post::receive_data( std::string_view buf )
{
    if( get_code() != HTTP_OK ) return;

    m_rawdata.append( buf );
}



//
// ローダがsendを終了したので戻り値解析
//
void Post::receive_finish()
{
#ifdef _DEBUG
    std::cout << "Post::receive_finish\n";
#endif

    {
        JDLIB::Iconv libiconv( Encoding::utf8, DBTREE::board_encoding( m_url ) );
        libiconv.convert( m_rawdata.data(), m_rawdata.size(), m_return_html );
    }

#ifdef _DEBUG
    std::cout << "code = " << get_code() << std::endl;
    std::cout << m_return_html << std::endl;
#endif

    clear();

    ///////////////////
    
    // ポスト失敗
    if( get_code() != HTTP_OK
        && ! ( ( get_code() == HTTP_MOVED_PERM || get_code() == HTTP_REDIRECT || get_code() == HTTP_PERMANENT_REDIRECT )
               && ! location().empty() ) // リダイレクトは成功(かもしれない)
        ){

        m_errmsg = get_str_code();
        emit_sigfin();
        return;
    }

    // 以下、code == 200、 又は 302 かつ locationがセットされている(リダイレクト) の場合

    JDLIB::Regex regex;
    const size_t offset = 0;
    bool icase = false;
    bool newline = true;
    const bool usemigemo = false;
    const bool wchar = false;

    std::string title;
    std::string tag_2ch;
    std::string msg;
    std::string conf;

    // タイトル
    icase = true;
    newline = false; // . に改行をマッチさせる
    regex.exec( ".*<title>([^<]*)</title>.*", m_return_html, offset, icase, newline, usemigemo, wchar );
    title = MISC::utf8_trim( regex.str( 1 ) );

    // 2chタグ
    icase = false;
    newline = false; // . に改行をマッチさせる
    regex.exec( ".*2ch_X:([^\\-]*)\\-\\->.*", m_return_html, offset, icase, newline, usemigemo, wchar );
    tag_2ch = MISC::utf8_trim( regex.str( 1 ) );

    // エラー内容を取得

    // 一番内側の<b>〜</b>を探して取得
    icase = true;
    newline = false; // . に改行をマッチさせる
    m_errmsg = std::string();
    if( regex.exec( "([^>]|[^b]>)*<b>(([^>]|[^b]>)*)</b>.*", m_return_html, offset, icase, newline, usemigemo, wchar ) ){

        m_errmsg = regex.str( 2 );
    }

    // 2ch タグで error が返った場合
    else if( tag_2ch.find( "error" ) != std::string::npos ){

        icase = true;
        newline = false; // . に改行をマッチさせる
        if( regex.exec( "error +-->(.*)</body>", m_return_html, offset, icase, newline, usemigemo, wchar ) ) m_errmsg = regex.str( 1 );
    }

    // p2 型
    // XXX: p2ログイン機能を削除したのでもう不要か？
    else if( title.find( "error" ) != std::string::npos ){

        icase = true;
        newline = false; // . に改行をマッチさせる
        if( regex.exec( "<h4>(.*)</h4>", m_return_html, offset, icase, newline, usemigemo, wchar ) ) m_errmsg = regex.str( 1 );
    }

    if( ! m_errmsg.empty() ){

        m_errmsg = MISC::replace_str( m_errmsg, "\n", "" ); 

        // <a 〜を取り除く
        icase = false;
        newline = true;
        while( regex.exec( "(.*)<a +href *= *\"([^\"]*)\" *>(.*)</a>(.*)", m_errmsg, offset, icase, newline, usemigemo, wchar ) ){
            m_errmsg = regex.str( 1 ) + " " + regex.str( 2 ) + " " + regex.str( 3 ) + regex.str( 4 );
        }

        // 改行その他
        m_errmsg= MISC::replace_str( m_errmsg, "<br>", "\n" );
        m_errmsg= MISC::replace_str( m_errmsg, "<hr>", "\n-------------------\n" );

        // samba秒取得
        icase = false;
        newline = false; // . に改行をマッチさせる
        // Smaba24規制の場合
        //   ＥＲＲＯＲ - 593 60 sec たたないと書けません。(1回目、8 sec しかたってない)
        //   ERROR: Samba24:Caution 25 秒たたないと書けません。(1 回目、24 秒しかたってない)
        if( regex.exec( "(ＥＲＲＯＲ +- +593|ERROR: +Samba24:Caution|) +([0-9]+) +",
                        m_errmsg, offset, icase, newline, usemigemo, wchar ) ) {
            time_t sec = atoi( regex.str( 2 ).c_str() );
#ifdef _DEBUG
            std::cout << "samba = " << sec << std::endl;
#endif
            DBTREE::board_set_samba_sec( m_url, sec );
            DBTREE::board_update_writetime( m_url );
        }
    }

    // 書き込み確認
    icase = false;
    newline = true;
    regex.exec( ".*<font size=\\+1 color=#FF0000>([^<]*)</font>.*", m_return_html, offset, icase, newline, usemigemo, wchar );
    conf = MISC::utf8_trim( regex.str( 1 ) );

    // メッセージ本文

    // 2ch 型
    icase = false;
    newline = false; // . に改行をマッチさせる
    if( ! regex.exec( ".*</ul>.*<b>(.*)</b>.*<form.*", m_return_html, offset, icase, newline, usemigemo, wchar ) ) {
        // 0ch 型
        regex.exec( ".*</ul>.*<b>(.*)</b>.*<input.*", m_return_html, offset, icase, newline, usemigemo, wchar );
    }

    msg = MISC::utf8_trim( regex.str( 1 ) );
    const::std::list< std::string > list_cookies = SKELETON::Loadable::cookies();

#ifdef _DEBUG
    std::cout << "TITLE: [" << title << "]\n";
    std::cout << "2ch_X: [" << tag_2ch << "]\n";
    std::cout << "CONF: [" << conf << "]\n";
    std::cout << "MSG: [" << msg << "]\n";
    std::cout << "ERR: [" << m_errmsg << "]\n";

    for( const std::string& cookie : list_cookies ) std::cout << "cookie : [" << cookie << "]\n";

    std::cout << "location: [" << location() << "]\n";
#endif

    // クッキーのセット
    const bool empty_cookies = DBTREE::board_cookie_for_post( m_url ).empty();
    if( list_cookies.size() ) DBTREE::board_set_list_cookies( m_url, list_cookies );

    // 成功
    if( title.find( "書きこみました" ) != std::string::npos
        || tag_2ch.find( "true" ) != std::string::npos
        || ( ( get_code() == HTTP_MOVED_PERM || get_code() == HTTP_REDIRECT || get_code() == HTTP_PERMANENT_REDIRECT )
             && ! location().empty() )  // リダイレクトされた場合
        ){

#ifdef _DEBUG
        std::cout << "write ok" << std::endl;
#endif        

        DBTREE::article_update_writetime( m_url );
        if( m_new_article ) {
            // 板のフロントページをダウンロードしてスレ立てに使うキーワードを更新する
            DBTREE::board_download_front( m_url );
        }
        emit_sigfin();
        return;
    }

    // クッキー確認
    else if( m_count < 1 && // 永久ループ防止
        ( title.find( "書き込み確認" ) != std::string::npos
          || tag_2ch.find( "cookie" ) != std::string::npos
          || ( empty_cookies && list_cookies.size() )
            ) ){

        clear();

        // 書き込み確認表示
        if( ! CONFIG::get_always_write_ok() ){

            std::string diagmsg = MISC::replace_str( msg, "<br>", "\n" );
            if( diagmsg.empty() ){
                if( m_errmsg.empty() ) diagmsg = "クッキーを有功にして書き込みますか？";
                else diagmsg = m_errmsg;
            }

            ConfirmDiag mdiag( m_url, diagmsg );
            mdiag.set_default_size( 600, 400 );
            const int response = mdiag.run();
            mdiag.hide();
            if( response != Gtk::RESPONSE_OK ){

                set_code( HTTP_CANCEL );
                emit_sigfin();
                return;
            }
            if( mdiag.get_chkbutton().get_active() ) CONFIG::set_always_write_ok( true );
        }

        // HTMLからフォームデータを取得できたらメッセージボディを更新する
        std::string msg_body = DBTREE::board_parse_form_data( m_url, m_return_html );
        if( ! msg_body.empty() ) m_msg = std::move( msg_body );

        // subbbs.cgi にポスト先を変更してもう一回ポスト
        m_subbbs = true;
        ++m_count; // 永久ループ防止
        post_msg();

        return;
    }

    // スレ立て時の書き込み確認
    else if( m_count < 1 // 永久ループ防止
             && ! m_subbbs && conf.find( "書き込み確認" ) != std::string::npos ){

        // HTMLからフォームデータを取得できたらメッセージボディを更新する
        std::string msg_body = DBTREE::board_parse_form_data( m_url, m_return_html );
        if( ! msg_body.empty() ) m_msg = std::move( msg_body );

        // subbbs.cgi にポスト先を変更してもう一回ポスト
        m_subbbs = true;
        ++m_count; // 永久ループ防止
        post_msg();
        return;
    }

    // その他のエラー

#ifdef _DEBUG
    std::cout << "Error" << std::endl;
    std::cout << m_return_html << std::endl;
#endif        

    MISC::ERRMSG( m_errmsg );

    set_code( HTTP_ERR );
    emit_sigfin();
}
