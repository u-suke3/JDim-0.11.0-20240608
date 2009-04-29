// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "boardadmin.h"
#include "boardview.h"

#include "dbtree/interface.h"

#include "skeleton/msgdiag.h"

#include "sound/soundmanager.h"

#include "session.h"
#include "command.h"
#include "httpcode.h"
#include "global.h"

using namespace BOARD;


BoardView::BoardView( const std::string& url )
    : BoardViewBase( url )
{
#ifdef _DEBUG
    std::cout << "BoardView::BoardView : url = " << get_url() << std::endl;
#endif

    // オートリロード可
    set_enable_autoreload( true );

    // text/url-list  のドロップ可
    get_treeview().set_enable_drop_uri_list();
}


BoardView::~BoardView()
{
#ifdef _DEBUG
    std::cout << "BoardView::~BoardView : url = " << get_url() << std::endl;
#endif

    DBTREE::board_save_info( get_url_board() );
}


// 更新した
const bool BoardView::is_updated()
{
    const int status = DBTREE::board_status( get_url_board() );

#ifdef _DEBUG
    std::cout << "BoardView::is_updated " << ( status & STATUS_UPDATED ) << std::endl;
#endif

    return ( status & STATUS_UPDATED );
}


// 更新チェックして更新可能か
const bool BoardView::is_check_update()
{
    const int status = DBTREE::board_status( get_url_board() );

#ifdef _DEBUG
    std::cout << "BoardView::is_check_update status = " << ( status & STATUS_UPDATE ) << std::endl;
#endif

    return ( status & STATUS_UPDATE );
}


//
// リロード
//
void BoardView::reload()
{
    // オフライン
    if( ! SESSION::is_online() ){
        SKELETON::MsgDiag mdiag( get_parent_win(), "オフラインです" );
        mdiag.run();
        return;
    }

    // オートリロードのカウンタを0にする
    View::reset_autoreload_counter();

    show_view();

    // 板履歴更新
    CORE::core_set_command( "set_history_board", get_url_board() );
}



//
// view更新
//
// subject.txtのロードが終わったら呼ばれる
//
void BoardView::update_view()
{
    const int code = DBTREE::board_code( get_url_board() );

#ifdef _DEBUG
    std::cout << "BoardView::update_view " << get_url()
              << " code = " << code << std::endl;
#endif    

    // 音を鳴らす
    if( SESSION::is_online() && code != HTTP_INIT ){
        if( code == HTTP_OK ) SOUND::play( SOUND::SOUND_RES );
        else if( code == HTTP_NOT_MODIFIED ) SOUND::play( SOUND::SOUND_NO );
        else SOUND::play( SOUND::SOUND_ERR );
    }

    // 高速化のためデータベースに直接アクセス
    std::list< DBTREE::ArticleBase* >& list_subject = DBTREE::board_list_subject( get_url_board() );
    update_view_impl( list_subject );

    // dat落ちしたスレッドをスレあぼーんのリストから取り除く
    if( code == HTTP_OK ) DBTREE::remove_old_abone_thread( get_url_board() );
}


//
// 板名更新
//
void BoardView::update_boardname()
{
    // ウィンドウタイトル表示
    set_title( DBTREE::board_name( get_url_board() ) );
    BOARD::get_admin()->set_command( "set_title", get_url(), get_title() );

    // タブに名前をセット
    BOARD::get_admin()->set_command( "set_tablabel", get_url(), DBTREE::board_name( get_url_board() ) );
}
