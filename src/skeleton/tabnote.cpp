// ライセンス: GPL2

//#define _DEBUG
//#define _DEBUG_RESIZE_TAB
#include "jddebug.h"
#include "gtkmmversion.h"

#include "dragnote.h"
#include "tabnote.h"
#include "tablabel.h"

#include "config/globalconf.h"
#include "jdlib/miscgtk.h"

#include "session.h"
#include "dndmanager.h"


using namespace SKELETON;


// ダミーWidgetを作成してtabにappend ( 表示はされない )
class DummyWidget : public Gtk::Widget
{
public:
    DummyWidget() : Gtk::Widget()
    {
        set_has_window( false );
    }
    ~DummyWidget() noexcept override = default;
};


//////////////////////////////////////////////


TabNotebook::TabNotebook( DragableNoteBook* parent )
    : Gtk::Notebook()
    , m_parent( parent )
    , m_layout_tab{ create_pango_layout( "" ) }
{

    set_border_width( 0 );
    set_size_request( 1, -1 ); // これが無いと最大化を解除したときにウィンドウが勝手にリサイズする

    add_events( Gdk::POINTER_MOTION_MASK );
    add_events( Gdk::LEAVE_NOTIFY_MASK );
    add_events( Gdk::SCROLL_MASK );

    // DnD設定
    // ドロップ側に設定する
    drag_source_unset();
    drag_dest_unset();
    std::vector< Gtk::TargetEntry > targets;
    targets.push_back( Gtk::TargetEntry( DNDTARGET_TAB, Gtk::TARGET_SAME_APP, 0 ) );
    drag_dest_set( targets, Gtk::DEST_DEFAULT_MOTION | Gtk::DEST_DEFAULT_DROP );

    m_tab_mrg = get_margin_start();
    if( m_tab_mrg <= 0 ) {
        m_tab_mrg = get_style_context()->get_margin().get_left();
    }

    m_pre_width = get_width();
}


TabNotebook::~TabNotebook() noexcept = default;


//
// クロック入力
//
void TabNotebook::clock_in()
{
    // Gtk::NoteBook は on_configure_event() と on_window_state_event() をキャッチ出来ない
    // かつ、 on_size_allocate() は最大化、最小化の時に無反応なので
    // 応急処置としてタイマーの中でサイズが変更したか調べて
    // 変わっていたらタブ幅を調整する
    if( ! m_fixtab && get_n_pages() && m_pre_width != get_width()
        && ! SESSION::is_booting()
        && ! SESSION::is_quitting()
        ){

        calc_tabsize();
        adjust_tabwidth();
    }
}



//
// サイズ変更
//
void TabNotebook::on_size_allocate( Gtk::Allocation& allocation )
{
    clock_in(); // タブ再描画の反応を良くする
    Gtk::Notebook::on_size_allocate( allocation );
}


int TabNotebook::append_tab( Widget& tab )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::append_tab\n";
#endif

    // ダミーWidgetを作成してtabにappend (表示はされない )
    DummyWidget* dummypage = Gtk::manage( new DummyWidget );

    return append_page( *dummypage , tab );
}


int TabNotebook::insert_tab( Widget& tab, int page )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::insert_tab position = "  << page << std::endl;
#endif

    // ダミーWidgetを作成してtabにappend (表示はされない )
    DummyWidget* dummypage = Gtk::manage( new DummyWidget );

    return insert_page( *dummypage, tab, page );
}


void TabNotebook::remove_tab( const int page, const bool adjust_tab )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::remove_tab page = " << page << std::endl;
#endif

    // ダミーWidgetはGtk::manage()の効果でdeleteされる
    remove_page( page );

    if( adjust_tab ) adjust_tabwidth();
}


void TabNotebook::reorder_child( int page1, int page2 )
{
    Gtk::Notebook::reorder_child( *get_nth_page( page1 ), page2 );
}


// タブ取得
SKELETON::TabLabel* TabNotebook::get_tablabel( int page )
{
    return dynamic_cast< SKELETON::TabLabel* >( get_tab_label( *get_nth_page( page ) ) );
}


//
// マウスの下にあるタブの番号を取得
//
// タブ上では無いときは-1を返す
// マウスがタブの右側にある場合はページ数( get_n_pages() )を返す
//
int TabNotebook::get_page_under_mouse()
{
    int x, y;
    Gdk::Rectangle rect = get_allocation();
    MISC::get_pointer_at_window( get_window(), x, y );

#ifdef _DEBUG
    std::cout << "TabNotebook::get_page_under_mouse x = " << x << " y = " << y << std::endl;
#endif

    if( y < rect.get_y() || y > rect.get_y() + rect.get_height() ) return -1;

    calc_tabsize();

    const int pages = get_n_pages();
    int ret = pages;
    for( int i = 0; i < pages; ++i ){

        const SKELETON::TabLabel* tab = get_tablabel( i );
        if( tab ){

            int tab_x = tab->get_tab_x();
            int tab_w = tab->get_tab_width();

            if( tab_x < 0 ) continue;

#ifdef _DEBUG
            std::cout << "TabNotebook::get_page_under_mouse page = " << i << " x = " << tab_x << " w = " << tab_w
                      << std::endl;
#endif

            if( x < tab_x ){
                ret = -1;
                break;
            }
            else if( x <= tab_x + tab_w ){
                ret = i;
                break;
            }
        }
    }

#ifdef _DEBUG
    std::cout << "TabNotebook::get_page_under_mouse ret = " << ret << std::endl;
#endif

    return ret;
}


//
// タブの文字列取得
//
const std::string& TabNotebook::get_tab_fulltext( int page )
{
    const SKELETON::TabLabel* tablabel = get_tablabel( page );
    if( ! tablabel ) return m_str_empty;

    return tablabel->get_fulltext();
}



//
// タブに文字列をセットとタブ幅調整
//
void TabNotebook::set_tab_fulltext( const std::string& str, int page )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::set_tab_fulltext page = " << page << " " << str << std::endl;
#endif

    SKELETON::TabLabel* tablabel = get_tablabel( page );
    if( tablabel && str != tablabel->get_fulltext() ){
        tablabel->set_fulltext( str );
        tablabel->set_tooltip_text( str );
        if( m_fixtab ) tablabel->resize_tab( str.length() );
        else adjust_tabwidth();
    }
}


//
// 各タブのサイズと座標を取得
//
void TabNotebook::calc_tabsize()
{
#ifdef _DEBUG
    std::cout << "TabNotebook::calc_tabsize\n";
#endif

    // gtk3は実装の詳細がバージョンによって異なるためタブの代わりにラベルの領域を取得する
    const int n_pages = get_n_pages();
    // ラベルの領域とタブの領域のオフセットを概算する
    // GTKテーマが変更されるとオフセットが変わる可能性があるので毎回計算する
    // XXX: この修正はラベルの左右の余白の大きさが同じであることを前提とする
    int offset = 0;

    if( n_pages > 1 ) {
        const auto* tab1 = get_tablabel( 0 );
        for( int i = 1; i < n_pages; ++i ) {
            const auto* const tab2 = get_tablabel( i );
            if( tab1 && tab2 && tab1->get_mapped() && tab2->get_mapped() ) {
                const auto alloc1 = tab1->get_allocation();
                const auto alloc2 = tab2->get_allocation();
                offset = alloc2.get_x() - ( alloc1.get_x() + alloc1.get_width() );
#ifdef _DEBUG
                std::cout << "computed offset = " << offset << std::endl;
#endif
                break;
            }
            tab1 = tab2;
        }
    }

    for( int i = 0; i < n_pages; ++i ) {
        auto* const tab_label = get_tablabel( i );
        if( tab_label ) {
            int tab_x = -1;
            int tab_y = -1;
            int tab_w = -1;
            int tab_h = -1;

            if( tab_label->get_mapped() ) {
                Gdk::Rectangle rect = tab_label->get_allocation();

                tab_x = rect.get_x() - ( offset / 2 );
                tab_y = rect.get_y();
                tab_w = rect.get_width() + offset;
                tab_h = rect.get_height();

                m_tab_mrg = 0;
            }

#ifdef _DEBUG
            std::cout << "page = " << i << " x = " << tab_x << " w = " << tab_w << " mrg = " << m_tab_mrg << std::endl;
#endif
            tab_label->set_tab_x( tab_x );
            tab_label->set_tab_y( tab_y );
            tab_label->set_tab_width( tab_w );
            tab_label->set_tab_height( tab_h );
        }
    }
}


//
// タブ幅調整
//
bool TabNotebook::adjust_tabwidth()
{
    // 起動中とシャットダウン中は処理しない
    if( SESSION::is_booting() ) return false;
    if( SESSION::is_quitting() ) return false;

    const int mrg_notebook = 30;

    const int pages = get_n_pages();
    if( ! pages ) return false;

    // layoutにラベルのフォントをセットする
    SKELETON::TabLabel* tab = get_tablabel( 0 );
    if( ! tab ) return false;
    m_layout_tab->set_font_description( tab->get_label_font_description() );
    const int label_margin = tab->get_label_margin() + m_tab_mrg;

    m_pre_width = get_width();
    const int width_notebook = m_pre_width - mrg_notebook;

    int avg_width_tab = static_cast<int>( static_cast<double>(width_notebook) / MAX( 3, pages ) );  // タブ幅の平均値
    if( avg_width_tab < label_margin ){
        avg_width_tab = label_margin;
    }

#ifdef _DEBUG_RESIZE_TAB
    std::cout << "TabNotebook::adjust_tabwidth\n"
              << "width_notebook = " << width_notebook << " page = " << pages << std::endl
              << "avg_width_tab = " << avg_width_tab
              << " tab_mrg = " << m_tab_mrg
              << " label_margin " << label_margin
              << std::endl;
#endif

    int label_width_org; // 変更前のタブの文字数
    int label_width; // 変更後のタブの文字数
    int width_total = 0;
    int tab_width, max_width;

    for( int i = 0; i < pages; ++i ){

        tab = get_tablabel( i );
        if( tab ){

            Glib::ustring ulabel( tab->get_fulltext() );
            Glib::ustring ulabel_org( ulabel );

            bool cut_suffix = false;
            label_width_org = label_width = ulabel.length();

            tab_width = -1; // タブ幅を未計算状態に初期化
            max_width = avg_width_tab * ( i + 1 ) - width_total; // 最大値 ( 収まらないときマイナスになる )

            // 長すぎるタブの文字列は表示しないよう、あらかじめ最大値を256に制限する
            if( label_width > 256 ){
                label_width = 256;
                ulabel.resize( label_width );
            }

            // タブの幅を最大値以下に縮める
            while( label_width > CONFIG::get_tab_min_str() ){

                if( ! cut_suffix && label_width < label_width_org ){
                    cut_suffix = true; // 文字列を切り詰めたら "..." を付与する
                    ulabel.append( "..." );
                }

                m_layout_tab->set_text( ulabel );
                int width = m_layout_tab->get_pixel_logical_extents().get_width() + label_margin;

#ifdef _DEBUG_RESIZE_TAB
                std::cout << "s " << i << " " << width << " / " << max_width << " + " << width_total
                          << " lng = " << label_width << " : " << ulabel << std::endl;
#endif
                if( width < max_width ){ // 最大値以下
                    tab_width = width; // タブ幅を保存
                    break;
                }

                // 最大値を越えていたら、概算で収まるように縮める
                int n = label_width - static_cast<int>( static_cast<double>(label_width) * max_width / width );
                if( n < 1 ) n = 1;
                if( label_width - n < CONFIG::get_tab_min_str() ){
                    n = label_width - CONFIG::get_tab_min_str();
                }
                label_width -= n;
                ulabel.erase( label_width, n ); // 末尾のn文字を消す
            }

            // 横をはみださないようにタブ幅を延ばしていく
            for(;;){

                if( label_width >= label_width_org ) break; // 全て収まった
                if( max_width < 0 ) break; // 確実に収まらない

                ulabel.insert( label_width, 1, ulabel_org[ label_width ] ); // 末尾の1文字を戻す
                ++label_width;

                m_layout_tab->set_text( ulabel );
                int width = m_layout_tab->get_pixel_logical_extents().get_width() + label_margin;

#ifdef _DEBUG_RESIZE_TAB
                std::cout << "w " << i << " " << width << " / " << max_width << " + " << width_total
                          << " lng = " << label_width << " : " << ulabel << std::endl;
#endif
                // 最大値を越えたらひとつ戻してbreak;
                if( width > max_width ){
                    --label_width;
                    ulabel.erase( label_width, 1 ); // 末尾の1文字を消す
                    break;
                }
                tab_width = width; // 最大値を超えていないタブ幅を保存
            }

            // タブ幅を確定する
            if( tab_width < 0 ){
                m_layout_tab->set_text( ulabel );
                tab_width = m_layout_tab->get_pixel_logical_extents().get_width() + label_margin;

#ifdef _DEBUG_RESIZE_TAB
                std::cout << "f " << i << " " << tab_width << " / " << max_width << " + " << width_total
                          << " lng = " << label_width << " : " << ulabel << std::endl;
#endif
            }
            width_total += tab_width;

            tab->resize_tab( label_width );
        }
    }

    // 枠の再描画
    m_parent->queue_draw();

    return true;
}


// signal_button_press_event と signal_button_release_event は emit されない
// ときがあるので自前でemitする
bool TabNotebook::on_button_press_event( GdkEventButton* event )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::on_button_press_event\n";
#endif

    if( m_sig_button_press.emit( event ) ) return true;

#ifdef _DEBUG
    std::cout << "Gtk::Notebook::on_button_press_event\n";
#endif

    return Gtk::Notebook::on_button_press_event( event );
}


bool TabNotebook::on_button_release_event( GdkEventButton* event )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::on_button_release_event\n";
#endif

    if( m_sig_button_release.emit( event ) ) return true;

#ifdef _DEBUG
    std::cout << "Gtk::Notebook::on_button_release_event\n";
#endif

    return Gtk::Notebook::on_button_release_event( event );
}


//
// マウスが動いた
//
bool TabNotebook::on_motion_notify_event( GdkEventMotion* event )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::on_motion_notify_event\n";
#endif

    m_sig_tab_motion_event.emit();

    return Gtk::Notebook::on_motion_notify_event( event );
}


//
// マウスが出た
//
bool TabNotebook::on_leave_notify_event( GdkEventCrossing* event )
{
#ifdef _DEBUG
    std::cout << "TabNotebook::leave\n";
#endif

    m_sig_tab_leave_event.emit();

    return Gtk::Notebook::on_leave_notify_event( event );
}


//
// マウスホイールを回した
//
bool TabNotebook::on_scroll_event( GdkEventScroll* event )
{
    if( ! CONFIG::get_switchtab_wheel() ) return true;

#ifdef _DEBUG
    std::cout << "TabNotebook::scroll\n";
#endif

    if( m_sig_scroll_event.emit( event ) ) return true;

    return Gtk::Notebook::on_scroll_event( event );
}


//
// ドラッグ中にマウスを動かした
//
bool TabNotebook::on_drag_motion( const Glib::RefPtr<Gdk::DragContext>& context, int x, int y, guint time)
{
#ifdef _DEBUG
    std::cout << "Gtk::Notebook::on_drag_motion x = " << x << " y = " << y << std::endl;
#endif

    int tab_x = -1;
    int tab_y = -1;
    int tab_w = -1;

    int page = get_page_under_mouse();
    if( page >= 0 ){

        if( page >= get_n_pages() ) page = get_n_pages() -1;

        const SKELETON::TabLabel* tab = get_tablabel( page );
        if( tab ){

            tab_x = tab->get_tab_x();
            tab_y = tab->get_tab_y();
            tab_w = tab->get_tab_width();

#ifdef _DEBUG
            std::cout << "page = " << page
                      << " tab_x = " << tab_x << " tab_y = " << tab_y << " tab_w " << tab_w << std::endl;
#endif
        }
    }

    m_sig_tab_drag_motion( page, tab_x, tab_y, tab_w );

    // on_drag_motion をキャンセルしないとDnD中にタブが勝手に切り替わる( gtknotebook.c をハック )
    return true;
}
