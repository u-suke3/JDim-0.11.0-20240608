// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "imageadmin.h"
#include "imageviewbase.h"
#include "imageareabase.h"

#include "skeleton/msgdiag.h"

#include "dbtree/interface.h"

#include "dbimg/imginterface.h"
#include "dbimg/img.h"

#include "jdlib/miscutil.h"
#include "jdlib/miscgtk.h"

#include "cache.h"
#include "command.h"
#include "sharedbuffer.h"
#include "global.h"
#include "controlutil.h"
#include "controlid.h"
#include "prefdiagfactory.h"
#include "httpcode.h"
#include "session.h"

#include <sstream>


#define SIZE_MENU { 25, 50, 75, 100, 150, 200, 400 }



using namespace IMAGE;


ImageViewBase::ImageViewBase( const std::string& url, const std::string& arg1, const std::string& arg2 )
    : SKELETON::View( url ),
      m_loading( false ),
      m_under_mouse( false ),
      m_enable_menuslot( true )
{
    // 高速化のためデータベースに直接アクセス
    m_img =  DBIMG::get_img( get_url() );
    assert( m_img );

    // マウスジェスチャ可能
    set_enable_mg( true );
}



ImageViewBase::~ImageViewBase()
{
#ifdef _DEBUG    
    std::cout << "ImageViewBase::~ImageViewBase : " << get_url() << std::endl;
#endif
}


//
// 共通セットアップ
//
void ImageViewBase::setup_common()
{
    const int default_width = 200;
    const int default_height = 50;

    set_width_client( default_width );
    set_height_client( default_height );

    // focus 可、モーションキャプチャ可
    m_event.set_flags( m_event.get_flags() | Gtk::CAN_FOCUS );
    m_event.add_events( Gdk::POINTER_MOTION_MASK );

    m_event.signal_button_press_event().connect( sigc::mem_fun( *this, &ImageViewBase::slot_button_press ) );
    m_event.signal_button_release_event().connect( sigc::mem_fun( *this, &ImageViewBase::slot_button_release ) );
    m_event.signal_motion_notify_event().connect(  sigc::mem_fun( *this, &ImageViewBase::slot_motion_notify ) );
    m_event.signal_key_press_event().connect( sigc::mem_fun(*this, &ImageViewBase::slot_key_press ) );
    m_event.signal_scroll_event().connect( sigc::mem_fun(*this, &ImageViewBase::slot_scroll_event ) );
    m_event.signal_enter_notify_event().connect(  sigc::mem_fun( *this, &ImageViewBase::slot_enter_notify_event ) );
    m_event.signal_leave_notify_event().connect(  sigc::mem_fun( *this, &ImageViewBase::slot_leave_notify_event ) );

    m_event.grab_focus();

    // ポップアップメニューの設定
    // アクショングループを作ってUIマネージャに登録
    action_group() = Gtk::ActionGroup::create();
    action_group()->add( Gtk::Action::create( "CancelMosaic", "CancelMosaic"),
                         sigc::mem_fun( *this, &ImageViewBase::slot_cancel_mosaic ) );
    action_group()->add( Gtk::Action::create( "ShowLargeImg", "サイズが大きい画像を表示(_L)"),
                         sigc::mem_fun( *this, &ImageViewBase::slot_show_large_img ) );
    action_group()->add( Gtk::Action::create( "LoadStop", "StopLoading" ), sigc::mem_fun( *this, &ImageViewBase::stop ) );
    action_group()->add( Gtk::Action::create( "Reload", "強制再読み込み(_E)"), sigc::mem_fun( *this, &ImageViewBase::slot_reload_force ) );
    action_group()->add( Gtk::Action::create( "AppendFavorite", "お気に入りに追加(_F)..."), sigc::mem_fun( *this, &ImageViewBase::slot_favorite ) );

    action_group()->add( Gtk::Action::create( "ZoomFitImage", "ZoomFitImage" ),
                         sigc::mem_fun( *this, &ImageViewBase::slot_fit_win ) );
    action_group()->add( Gtk::Action::create( "ZoomInImage", "ZoomInImage" ),
                         sigc::mem_fun( *this, &ImageViewBase::slot_zoom_in ) );
    action_group()->add( Gtk::Action::create( "ZoomOutImage", "ZoomOutImage" ),
                         sigc::mem_fun( *this, &ImageViewBase::slot_zoom_out ) );
    action_group()->add( Gtk::Action::create( "OrgSizeImage", "OrgSizeImage" ),
                         sigc::bind< int >( sigc::mem_fun( *this, &ImageViewBase::slot_resize_image ), 100 ) );

    action_group()->add( Gtk::Action::create( "Size_Menu", "サイズ変更(_R)" ) );

    // サイズ
    unsigned int size[] = SIZE_MENU;
    for( unsigned int i = 0; i < sizeof( size )/sizeof( unsigned int ) ; ++i ){
        int tmp_size = size[ i ];
        std::string str_size = MISC::itostr( tmp_size );
        std::string str_shortcut = "(_" + MISC::itostr( i ) + ")";
        Glib::RefPtr< Gtk::Action > action = Gtk::Action::create( "Size" + str_size, str_size + "%" + str_shortcut );
        action_group()->add( action, sigc::bind< int >( sigc::mem_fun( *this, &ImageViewBase::slot_resize_image ), tmp_size ) );
    }

    action_group()->add( Gtk::Action::create( "Move_Menu", "移動(_M)" ) );
    action_group()->add( Gtk::Action::create( "MoveHead", "先頭に移動(_H)" ), sigc::mem_fun( *this, &ImageViewBase::slot_move_head ) );
    action_group()->add( Gtk::Action::create( "MoveTail", "最後に移動(_T)" ), sigc::mem_fun( *this, &ImageViewBase::slot_move_tail ) );

    action_group()->add( Gtk::Action::create( "Quit", "Quit" ), sigc::mem_fun( *this, &ImageViewBase::close_view ) );

    action_group()->add( Gtk::Action::create( "Close_Menu", "複数の画像を閉じる(_L)" ) );
    action_group()->add( Gtk::Action::create( "CloseOther", "他の画像(_O)" ), sigc::mem_fun( *this, &ImageViewBase::slot_close_other_views ) );
    action_group()->add( Gtk::Action::create( "CloseLeft", "左←の画像(_L)" ), sigc::mem_fun( *this, &ImageViewBase::slot_close_left_views ) );
    action_group()->add( Gtk::Action::create( "CloseRight", "右→の画像(_R)" ), sigc::mem_fun( *this, &ImageViewBase::slot_close_right_views ) );
    action_group()->add( Gtk::Action::create( "CloseAll", "全ての画像(_A)" ), sigc::mem_fun( *this, &ImageViewBase::slot_close_all_views ) );

    action_group()->add( Gtk::ToggleAction::create( "LockTab", "タブをロックする(_K)", std::string(), false ),
                         sigc::mem_fun( *this, &ImageViewBase::slot_lock ) );

    action_group()->add( Gtk::Action::create( "OpenBrowser", "ブラウザで開く(_W)"),
                         sigc::mem_fun( *this, &ImageViewBase::slot_open_browser ) );
    action_group()->add( Gtk::Action::create( "OpenRef", "参照元のレスを開く(_O)"), sigc::mem_fun( *this, &ImageViewBase::slot_open_ref ) );
    action_group()->add( Gtk::Action::create( "CopyURL", "URLをコピー(_C)"), sigc::mem_fun( *this, &ImageViewBase::slot_copy_url ) );
    action_group()->add( Gtk::Action::create( "Save", "Save"), sigc::mem_fun( *this, &ImageViewBase::slot_save ) );
    action_group()->add( Gtk::Action::create( "SaveAll", "全ての画像を保存(_A)..."), sigc::mem_fun( *this, &ImageViewBase::slot_save_all ) );

    action_group()->add( Gtk::Action::create( "DeleteMenu", "Delete" ) );    
    action_group()->add( Gtk::Action::create( "DeleteImage", "削除する(_D)"), sigc::mem_fun( *this, &ImageViewBase::delete_view ) );
    action_group()->add( Gtk::ToggleAction::create( "ProtectImage", "キャッシュを保護する(_R)", std::string(), false ),
                         sigc::mem_fun( *this, &ImageViewBase::slot_toggle_protectimage ) );

    action_group()->add( Gtk::Action::create( "AboneImage", "画像をあぼ〜んする(_B)"), sigc::mem_fun( *this, &ImageViewBase::slot_abone_img ) );

    action_group()->add( Gtk::Action::create( "Preference", "プロパティ(_P)..."), sigc::mem_fun( *this, &ImageViewBase::show_preference ) );

    ui_manager() = Gtk::UIManager::create();    
    ui_manager()->insert_action_group( action_group() );

    Glib::ustring str_ui = 

    "<ui>"

    "<popup name='popup_menu'>"

    "<menuitem action='CancelMosaic'/>"
    "<menuitem action='ShowLargeImg'/>"
    "<separator/>"

    "<menu action='Size_Menu'>"
    "<menuitem action='Size25'/>"
    "<menuitem action='Size50'/>"
    "<menuitem action='Size75'/>"
    "<menuitem action='Size100'/>"
    "<menuitem action='Size150'/>"
    "<menuitem action='Size200'/>"
    "<menuitem action='Size400'/>"
    "</menu>"
    "<menuitem action='OrgSizeImage'/>"
    "<menuitem action='ZoomFitImage'/>"
    "<menuitem action='ZoomInImage'/>"
    "<menuitem action='ZoomOutImage'/>"
    "<separator/>"

    "<menuitem action='OpenBrowser'/>"
    "<menuitem action='OpenRef'/>"
    "<separator/>"

    "<menuitem action='CopyURL'/>"
    "<separator/>"

    "<menuitem action='AppendFavorite'/>"
    "<menuitem action='Save'/>"
    "<separator/>"

    "<menuitem action='LoadStop'/>"
    "<menuitem action='Reload'/>"
    "<separator/>"

    "<menuitem action='ProtectImage'/>"
    "<menu action='DeleteMenu'>"
    "<menuitem action='DeleteImage'/>"
    "</menu>"

    "<separator/>"
    "<menuitem action='AboneImage'/>"

    "<separator/>"

    "<menuitem action='Preference'/>"


    "</popup>"

    //////////////////////////

    // アイコンのメニュー
    
    "<popup name='popup_menu_icon'>"

    "<menu action='Move_Menu'>"
    "<menuitem action='MoveHead'/>"
    "<menuitem action='MoveTail'/>"
    "</menu>"
    "<separator/>"

    "<menuitem action='LockTab'/>"
    "<separator/>"

    "<menuitem action='Quit'/>"
    "<separator/>"

    "<menu action='Close_Menu'>"
    "<menuitem action='CloseAll'/>"
    "<menuitem action='CloseOther'/>"
    "<menuitem action='CloseLeft'/>"
    "<menuitem action='CloseRight'/>"
    "</menu>"
    "<separator/>"

    "<menuitem action='SaveAll'/>"
    "<separator/>"

    "<menuitem action='OpenBrowser'/>"
    "<menuitem action='OpenRef'/>"
    "<separator/>"

    "<menuitem action='CopyURL'/>"
    "<separator/>"

    "<menuitem action='AppendFavorite'/>"
    "<menuitem action='Save'/>"
    "<separator/>"

    "<menuitem action='LoadStop'/>"
    "<menuitem action='Reload'/>"
    "<separator/>"

    "<menuitem action='ProtectImage'/>"
    "<menu action='DeleteMenu'>"
    "<menuitem action='DeleteImage'/>"
    "</menu>"
    "<separator/>"

    "<separator/>"
    "<menuitem action='AboneImage'/>"

    "<separator/>"
    "<menuitem action='Preference'/>"

    "</popup>"

    "</ui>";

    ui_manager()->add_ui_from_string( str_ui );


    // ポップアップメニューにキーアクセレータやマウスジェスチャを表示
    Gtk::Menu* popupmenu = dynamic_cast< Gtk::Menu* >( ui_manager()->get_widget( "/popup_menu" ) );
    CONTROL::set_menu_motion( popupmenu );

    popupmenu = dynamic_cast< Gtk::Menu* >( ui_manager()->get_widget( "/popup_menu_icon" ) );
    CONTROL::set_menu_motion( popupmenu );
}


//
// ImageAreaBaseのセット
// 
void ImageViewBase::set_imagearea( ImageAreaBase* imagearea )
{
    assert( imagearea );

    m_imagearea.clear();
    m_imagearea = imagearea;

    set_width_client( imagearea->get_width() );
    set_height_client( imagearea->get_height() );
    m_event.add( *m_imagearea );
}



//
// ImageAreaBaseのクリア
// 
void ImageViewBase::remove_imagearea()
{
    if( m_imagearea ){
        m_event.remove();
        m_imagearea.clear();
    }
}



//
// コマンド
//
bool ImageViewBase::set_command( const std::string& command, const std::string& arg )
{
    if( command == "switch_icon" ) switch_icon();

    return true;
}



//
// 再読込
//
void ImageViewBase::reload()
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::reload url = " << get_url() << std::endl;
#endif

    std::string refurl = m_img->get_refurl();
    m_img->download_img( refurl );
}




//
// ロード停止
//
void ImageViewBase::stop()
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::stop url = " << get_url() << std::endl;
#endif
    m_img->stop_load();
}




//
// 再描画
//
void ImageViewBase::redraw_view()
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::redraw_view url = " << get_url() << std::endl;
#endif    

    show_view();
}



//
// 先頭に移動
//
void ImageViewBase::slot_move_head()
{
    IMAGE::get_admin()->set_command( "tab_head", "" );
}



//
// 最後に移動
//
void ImageViewBase::slot_move_tail()
{
    IMAGE::get_admin()->set_command( "tab_tail", "" );
}


//
// 閉じる
//
void ImageViewBase::close_view()
{
    IMAGE::get_admin()->set_command( "close_view", get_url() );
}



//
// 他の画像を閉じる
//
void ImageViewBase::slot_close_other_views()
{
    IMAGE::get_admin()->set_command( "close_other_views", get_url() );
}


//
// 左の画像を閉じる
//
void ImageViewBase::slot_close_left_views()
{
    IMAGE::get_admin()->set_command( "close_left_views", get_url() );
}


//
// 右の画像を閉じる
//
void ImageViewBase::slot_close_right_views()
{
    IMAGE::get_admin()->set_command( "close_right_views", get_url() );
}



//
// 全ての画像を閉じる
//
void ImageViewBase::slot_close_all_views()
{
    IMAGE::get_admin()->set_command( "close_all_views" );
}


//
// プロパティ
//
void ImageViewBase::show_preference()
{
    SKELETON::PrefDiag* pref= CORE::PrefDiagFactory( IMAGE::get_admin()->get_win(), CORE::PREFDIAG_IMAGE, get_url() );

    IMAGE::get_admin()->set_command_immediately( "disable_fold_win" ); // run 直前に呼ぶこと
    pref->run();
    IMAGE::get_admin()->set_command_immediately( "enable_fold_win" );  // run 直後に呼ぶこと

    delete pref;
}


//
// 削除
//
void ImageViewBase::delete_view()
{
    CORE::core_set_command( "delete_image", get_url() );
}



//
// viewの操作
//
void ImageViewBase::operate_view( const int& control )
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::operate_view control = " << control << std::endl;
#endif

    switch( control ){

        case CONTROL::CancelMosaic:
        case CONTROL::CancelMosaicButton:
            slot_cancel_mosaic();
            break;

        case CONTROL::ZoomInImage:
            slot_zoom_in();
            break;

        case CONTROL::ZoomOutImage:
            slot_zoom_out();
            break;

        case CONTROL::ZoomFitImage:
            slot_fit_win();
            break;

        case CONTROL::OrgSizeImage:
            slot_resize_image( 100 );
            break;

        case CONTROL::ReloadTabButton:
        case CONTROL::Reload:
            reload();
            break;

        case CONTROL::StopLoading:
            stop();
            break;

        case CONTROL::CloseImageButton:
        case CONTROL::CloseImageTabButton:
        case CONTROL::Quit:
            close_view();
            break;

        case CONTROL::TabLeft:
            IMAGE::get_admin()->set_command( "tab_left" );
            break;

        case CONTROL::TabRight:
            IMAGE::get_admin()->set_command( "tab_right" );
            break;

        case CONTROL::ScrollUpImage:
            IMAGE::get_admin()->set_command( "scroll_up" );
            break;

        case CONTROL::ScrollDownImage:
            IMAGE::get_admin()->set_command( "scroll_down" );
            break;

        case CONTROL::ScrollLeftImage:
            IMAGE::get_admin()->set_command( "scroll_left" );
            break;

        case CONTROL::ScrollRightImage:
            IMAGE::get_admin()->set_command( "scroll_right" );
            break;

            // article に切り替え
        case CONTROL::Left:
            CORE::core_set_command( "switch_leftview" );
            break;

        case CONTROL::ToggleArticle:
            CORE::core_set_command( "toggle_article" );
            break;

        case CONTROL::Save:
            slot_save();
            break;

        case CONTROL::Delete:

            if( !m_img->is_protected() ){

                SKELETON::MsgDiag mdiag( IMAGE::get_admin()->get_win(),
                                         "画像を削除しますか？", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO );
                mdiag.set_default_response( Gtk::RESPONSE_NO );
                if( mdiag.run() == Gtk::RESPONSE_YES ) delete_view();
            }
            else{

                SKELETON::MsgDiag mdiag( IMAGE::get_admin()->get_win(), "キャッシュ保護されています" );
                mdiag.run();
            }

            break;

            // サイドバー表示/非表示
        case CONTROL::ShowSideBar:
            CORE::core_set_command( "toggle_sidebar" );
            break;

            // メニューバー表示/非表示
        case CONTROL::ShowMenuBar:
            CORE::core_set_command( "toggle_menubar" );
            break;
    }
}
                                                    




//
// キープレスイベント
//
bool ImageViewBase::slot_key_press( GdkEventKey* event )
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::slot_key_press url = " << get_url() << std::endl;
#endif

    operate_view( get_control().key_press( event ) );

    return true;
}



//
// ボタンクリック
//
bool ImageViewBase::slot_button_press( GdkEventButton* event )
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::slot_button_press url = " << get_url() << std::endl;
#endif

    // マウスジェスチャ
    get_control().MG_start( event );

    // ホイールマウスジェスチャ
    get_control().MG_wheel_start( event );

    // ダブルクリック
    // button_release_eventでは event->type に必ず GDK_BUTTON_RELEASE が入る
    m_dblclick = false;
    if( event->type == GDK_2BUTTON_PRESS ) m_dblclick = true; 

    // クリック
    // 反応を良くするため slot_button_release() ではなくてここで処理する
    if( get_control().button_alloted( event, CONTROL::ClickButton ) ){
        IMAGE::get_admin()->set_command( "switch_image", get_url() );
        IMAGE::get_admin()->set_command( "switch_admin" );
    }

    return true;
}



//
// マウスボタンのリリースイベント
//
bool ImageViewBase::slot_button_release( GdkEventButton* event )
{
    /// マウスジェスチャ
    int mg = get_control().MG_end( event );

    // ホイールマウスジェスチャ
    // 実行された場合は何もしない 
    if( get_control().MG_wheel_end( event ) ) return true;

    if( mg != CONTROL::None && enable_mg() ){
        operate_view( mg );
        return true;
    }

    // ダブルクリックの処理のため一時的にtypeを切替える
    GdkEventType type_copy = event->type;
    if( m_dblclick ) event->type = GDK_2BUTTON_PRESS;

    // ポップアップメニュー
    if( get_control().button_alloted( event, CONTROL::PopupmenuButton ) ){

        show_popupmenu( "", false );
    }

    else if( is_under_mouse() ) operate_view( get_control().button_press( event ) );

    event->type = type_copy;

    return true;
}



//
// マウスモーション
//
bool ImageViewBase::slot_motion_notify( GdkEventMotion* event )
{
    /// マウスジェスチャ
    get_control().MG_motion( event );

    return true;
}



//
// マウスホイールイベント
//
bool ImageViewBase::slot_scroll_event( GdkEventScroll* event )
{
    // ホイールマウスジェスチャ
    int control = get_control().MG_wheel_scroll( event );
    if( enable_mg() && control != CONTROL::None ){
        operate_view( control );
        return true;
    }

    return false;
}


//
// マウスが入った
//
bool ImageViewBase::slot_enter_notify_event( GdkEventCrossing* event )
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::slot_enter_notify_event\n";
#endif
    m_under_mouse = true;

    return true;
}

//
// マウスが出た
//
bool ImageViewBase::slot_leave_notify_event( GdkEventCrossing* event )
{
#ifdef _DEBUG
    std::cout << "ImageViewBase::slot_leave_notify_event\n";
#endif
    m_under_mouse = false;

    return true;
}


//
// 強制再読み込み
//
void ImageViewBase::slot_reload_force()
{
    if( ! m_enable_menuslot ) return;

    if( ! SESSION::is_online() ){
        SKELETON::MsgDiag mdiag( NULL, "オフラインです" );
        mdiag.run();
        return;
    }

    // コードをリセットしてから再読み込みをかける
    m_img->reset();
    m_img->set_code( HTTP_INIT );
    m_img->set_type( DBIMG::T_UNKNOWN );
    reload();
    CORE::core_set_command( "redraw", get_url() );
}


//
// モザイク解除
//
void ImageViewBase::slot_cancel_mosaic()
{
    if( ! m_enable_menuslot ) return;

    if( ! m_img->is_cached() ) return;

    if( m_img->is_fake() ){

        std::string type = "本当の画像タイプは";
        switch( DBIMG::get_type_real( get_url() ) ){
            case DBIMG::T_JPG: type += "JPEG"; break;
            case DBIMG::T_PNG: type += "PNG"; break;
            case DBIMG::T_GIF: type += "GIF"; break;
            case DBIMG::T_BMP: type += "BMP"; break;
        }
        type += "です。";

        SKELETON::MsgDiag mdiag( IMAGE::get_admin()->get_win(),
                                 "拡張子が偽装されています。" + type + "\n\nモザイクを解除しますか？",
                                 false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO );

        mdiag.set_default_response( Gtk::RESPONSE_NO );
        if( mdiag.run() != Gtk::RESPONSE_YES ) return;
    }

    m_img->set_mosaic( false );
    CORE::core_set_command( "redraw", get_url() );
}


//
// 大きい画像を表示
//
void ImageViewBase::slot_show_large_img()
{
    m_img->show_large_img();
}


//
// ウィンドウにサイズを合わせる
//
void ImageViewBase::slot_fit_win()
{
    m_img->set_zoom_to_fit( true );
    CORE::core_set_command( "redraw", get_url() );
}



//
// ズームイン
//
void ImageViewBase::slot_zoom_in()
{
    zoom_in_out( true );
}


//
// ズームアウト
//
void ImageViewBase::slot_zoom_out()
{
    zoom_in_out( false );
}


//
// ズームイン、アウトの実行部
void ImageViewBase::zoom_in_out( bool zoomin )
{
    unsigned int size[] = SIZE_MENU;
    unsigned int size_current = m_img->get_size();
    int size_zoomin = 0, size_zoomout = 0;

    // 現在のサイズから次のサイズを決定
    if( size_current ){

        size_zoomin = size_current + 100;
        size_zoomout = size_current - 100;

        unsigned int maxsize = sizeof( size )/sizeof( unsigned int );
        for( unsigned int i = 1; i < maxsize ; ++i ){

            if( zoomin && size[ i ] > size_current + 5 ){
                size_zoomin = size[ i ];
                break;
            }

            if( !zoomin && size[ i ] > size_current -5 ){
                size_zoomout = size[ i -1 ];
                break;
            }
        }
    }

#ifdef _DEBUG
    std::cout << "ImageViewBase::zoom_in_out\n"
              << "size_current = " << size_current << std::endl
              << "zoomin = "  << size_zoomin << std::endl
              << "zoomout = "  << size_zoomout << std::endl;
#endif

    if( zoomin ) slot_resize_image( size_zoomin );
    else slot_resize_image( size_zoomout );
}



//
// 画像サイズ
//
void ImageViewBase::slot_resize_image( int size )
{
    unsigned int sizemenu[] = SIZE_MENU;
    int maxsize = sizemenu[ sizeof( sizemenu )/sizeof( unsigned int ) -1 ];

    if( size <= 0 ) return;
    if( size > maxsize ) return;
    if( !m_img->is_zoom_to_fit() && m_img->get_size() == size ) return;

    m_img->set_zoom_to_fit( false );
    m_img->set_size( size );
    CORE::core_set_command( "redraw", get_url() );
}


//
// ロックする
//
void ImageViewBase::slot_lock()
{
    if( ! m_enable_menuslot ) return;

    if( is_locked() ) unlock();
    else lock();
}


//
// ブラウザで開く
//
void ImageViewBase::slot_open_browser()
{
    if( ! m_enable_menuslot ) return;

    std::string url = get_url();
    if( m_img->is_cached() ) url = "file://" + m_img->get_cache_path();
    CORE::core_set_command( "open_url_browser", url );
}



//
// 参照元を開く
//
void ImageViewBase::slot_open_ref()
{
    if( ! m_enable_menuslot ) return;

    std::string refurl = m_img->get_refurl();

    int center, from, to;
    std::string url = DBTREE::url_dat( refurl, center, to );
    if( url.empty() ) return;

    const int range = 10;
    from = MAX( 0, center - range );
    to = center + range;
    std::stringstream ss;
    ss << from << "-" << to;

    CORE::core_set_command( "open_article_res" ,url, ss.str(), MISC::itostr( center ) );
}



//
// URLをクリップボードにコピー
//
void ImageViewBase::slot_copy_url()
{
    if( ! m_enable_menuslot ) return;

    MISC::CopyClipboard( get_url() );
}


//
// 保存メニュー
//
void ImageViewBase::slot_save()
{
    if( ! m_enable_menuslot ) return;

    m_img->save( IMAGE::get_admin()->get_win(), std::string() );
}



//
// すべて保存
//
void ImageViewBase::slot_save_all()
{
    if( ! m_enable_menuslot ) return;

    IMAGE::get_admin()->set_command( "save_all" );
}



//
// お気に入り
//
void ImageViewBase::slot_favorite()
{
    if( ! m_enable_menuslot ) return;

    set_image_to_buffer();
    CORE::core_set_command( "append_favorite", URL_FAVORITEVIEW );
}


//
// 画像キャッシュ保護
//
void ImageViewBase::slot_toggle_protectimage()
{
    if( ! m_enable_menuslot ) return;

    m_img->set_protect( ! m_img->is_protected( ) );
    CORE::core_set_command( "redraw", get_url() ); // ステータス再描画
}


//
// 画像あぼーん
//
void ImageViewBase::slot_abone_img()
{
    m_img->set_abone( true );
    delete_view();
}


//
// 共有バッファセット
//
void ImageViewBase::set_image_to_buffer()
{
    CORE::DATA_INFO info;
    info.type = TYPE_IMAGE;
    info.url = get_url();
    info.name = get_url();

    CORE::SBUF_clear_info();
    CORE::SBUF_append( info );
}



//
// ポップアップメニューを表示する前にメニューのアクティブ状態を切り替える
//
// SKELETON::View::show_popupmenu() を参照すること
//
void ImageViewBase::activate_act_before_popupmenu( const std::string& url )
{
    if( !m_img ) return;

    // toggle　アクションを activeにするとスロット関数が呼ばれるので処理しないようにする
    m_enable_menuslot = false;

    Glib::RefPtr< Gtk::Action > act;
    Glib::RefPtr< Gtk::ToggleAction > tact;

    bool current_protect = m_img->is_protected();

    // ロック
    act = action_group()->get_action( "LockTab" );
    if( act ){

        Glib::RefPtr< Gtk::ToggleAction > tact = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic( act ); 
        if( is_locked() ) tact->set_active( true );
        else tact->set_active( false );
    }

    // 閉じる
    act = action_group()->get_action( "Quit" );
    if( act ){
        if( is_locked() ) act->set_sensitive( false );
        else act->set_sensitive( true );
    }

    // モザイク
    act = action_group()->get_action( "CancelMosaic" );
    if( act ){
        if( m_img->is_cached() && m_img->get_mosaic() ) act->set_sensitive( true );
        else act->set_sensitive( false );
    }

    // サイズの大きい画像を表示
    act = action_group()->get_action( "ShowLargeImg" );
    if( act ){
        if( m_img->get_type() == DBIMG::T_LARGE ) act->set_sensitive( true );
        else act->set_sensitive( false );
    }

    // サイズ系メニュー、お気に入り、保存
    std::string sizemenus[] =
    {
        "Size_Menu",
        "OrgSizeImage",
        "ZoomFitImage",
        "ZoomInImage",
        "ZoomOutImage",
        "AppendFavorite",
        "Save"
    };
    for( int i = 0; i < 7; ++i ){
	    act = action_group()->get_action( sizemenus[ i ] );
        if( act ){
            if( m_img->is_cached() ) act->set_sensitive( true );
            else act->set_sensitive( false );
        }
    }

    // 参照元スレ
    act = action_group()->get_action( "OpenRef" );
    if( act ){
        if( ! m_img->get_refurl().empty() ) act->set_sensitive( true );
        else act->set_sensitive( false );
    }
    
    // 保護
    act = action_group()->get_action( "ProtectImage" );
    if( act ){

        if( m_img->is_cached() ){

            act->set_sensitive( true );

            tact = Glib::RefPtr< Gtk::ToggleAction >::cast_dynamic( act ); 
            if( tact ){
                if( current_protect ) tact->set_active( true );
                else tact->set_active( false );
            }
        }
        else act->set_sensitive( false );
    }

    // 削除
    act = action_group()->get_action( "DeleteMenu" );
    if( act ){
        if( ( m_img->is_cached() || m_img->get_type() == DBIMG::T_LARGE ) && ! m_img->is_protected() ) act->set_sensitive( true );
        else act->set_sensitive( false );
    }

    // ロード停止
    act = action_group()->get_action( "LoadStop" );
    if( act ){
        if( m_img->is_loading() ) act->set_sensitive( true );
        else act->set_sensitive( false );
    }

    // あぼーん
    act = action_group()->get_action( "AboneImage" );
    if( act ){
        if( ! m_img->is_protected() ) act->set_sensitive( true );
        else act->set_sensitive( false );
    }

    m_enable_menuslot = true;
}

