// ライセンス: GPL2

#include "preference.h"

#include "dbtree/interface.h"

#include "jdlib/misccharcode.h"
#include "jdlib/misctime.h"
#include "jdlib/miscutil.h"

#include "config/globalconf.h"

#include "skeleton/msgdiag.h"

#include "cache.h"
#include "command.h"

#include <algorithm>
#include <cstdlib>
#include <list>
#include <set>
#include <string>

using namespace ARTICLE;

Preferences::Preferences( Gtk::Window* parent, const std::string& url, const std::string& command )
    : SKELETON::PrefDiag( parent, url )
    , m_label_name{ "スレタイトル:" }
    , m_label_name_value{ MISC::to_plain( DBTREE::article_subject( get_url() ) ) }
    , m_label_url{ "スレのURL:" }
    , m_label_url_value{ DBTREE::url_readcgi( get_url(), 0, 0 ) }
    , m_label_url_dat{ "DATファイルのURL:" }
    , m_label_url_dat_value{ DBTREE::url_dat( get_url() ) }
    , m_label_cache{ "ローカルキャッシュパス:" }
    , m_label_size{ "サイズ( byte / Kbyte ):" }
    , m_label_maxres{ "最大レス数 (0 : 未設定):" }
    , m_label_charset{ "テキストエンコーディング:" }
    , m_check_transpabone( "透明あぼ〜ん" )
    , m_check_chainabone( "連鎖あぼ〜ん" )
    , m_check_ageabone( "sage以外をあぼ〜ん" )
    , m_check_defnameabone( "デフォルト名無しをあぼ〜ん" )
    , m_check_noidabone( "ID無しをあぼ〜ん" )
    , m_check_boardabone( "板レベルでのあぼ〜んを有効にする" )
    , m_check_globalabone( "全体レベルでのあぼ〜んを有効にする" )
    , m_label_since{ "スレ立て日時:" }
    , m_label_modified{ "最終更新日時:" }
    , m_button_clearmodified( "日時クリア" )
    , m_label_write{ "最終書き込み日時:" }
    , m_bt_clear_post_history( "書き込み履歴クリア" )
    , m_label_write_name{ "名前:" }
    , m_label_write_mail{ "メール:" }
{
    // 一般
    if( DBTREE::article_is_cached( get_url() ) ){

        int size = DBTREE::article_lng_dat( get_url() );

        m_label_cache_value.set_text( CACHE::path_dat( get_url() ) );
        m_label_size_value.set_text( std::to_string( size )  + " / " + std::to_string( size/1024 ) );

        if( DBTREE::article_date_modified( get_url() ).empty() ) m_label_modified_value.set_text( "未取得" );
        else m_label_modified_value.set_text(
            MISC::timettostr( DBTREE::article_time_modified( get_url() ), MISC::TIME_WEEK )
            + " ( " + MISC::timettostr( DBTREE::article_time_modified( get_url() ), MISC::TIME_PASSED ) + " )"
            );

        if( DBTREE::article_write_time( get_url() ) ) m_label_write_value.set_text(
            MISC::timettostr( DBTREE::article_write_time( get_url() ), MISC::TIME_WEEK )
            + " ( " + MISC::timettostr( DBTREE::article_write_time( get_url() ), MISC::TIME_PASSED ) + " )"
            );
    }

    m_label_since_value.set_text(
            MISC::timettostr( DBTREE::article_since_time( get_url() ), MISC::TIME_WEEK )
            + " ( " + MISC::timettostr( DBTREE::article_since_time( get_url() ), MISC::TIME_PASSED ) + " )"
        );

    m_button_clearmodified.signal_clicked().connect( sigc::mem_fun(*this, &Preferences::slot_clear_modified ) );

    m_bt_clear_post_history.signal_clicked().connect( sigc::mem_fun(*this, &Preferences::slot_clear_post_history ) );

    // テキストエンコーディング
    const char* tmpcharset = MISC::encoding_to_cstr( DBTREE::article_encoding( get_url() ) );
    if( CONFIG::get_choose_character_encoding() ) {
        m_combo_charset.append( MISC::encoding_to_cstr( Encoding::utf8 ) );
        m_combo_charset.append( MISC::encoding_to_cstr( Encoding::sjis ) );
        m_combo_charset.append( MISC::encoding_to_cstr( Encoding::eucjp ) );
        m_combo_charset.set_active_text( tmpcharset );

        m_grid_info.attach( m_label_charset, 2, 5, 1, 1 );
        m_grid_info.attach( m_combo_charset, 3, 5, 1, 1 );
    }
    else {
        // エンコーディング設定は安全でないので無効のときは設定欄(コンボボックス)を表示しない
        m_label_charset_value.set_halign( Gtk::ALIGN_START );
        m_label_charset_value.set_hexpand( true );
        m_label_charset_value.set_text( tmpcharset );

        m_grid_info.attach( m_label_charset, 2, 5, 1, 1 );
        m_grid_info.attach( m_label_charset_value, 3, 5, 1, 1 );
    }

    // 最大レス数
    const int max_res = DBTREE::article_number_max( get_url() );
    m_spin_maxres.set_range( 0, CONFIG::get_max_resnumber() );
    m_spin_maxres.set_increments( 1, 1 );
    m_spin_maxres.set_value( max_res );

    m_label_name_value.set_ellipsize( Pango::ELLIPSIZE_END );
    m_label_url_value.set_ellipsize( Pango::ELLIPSIZE_END );
    m_label_url_dat_value.set_ellipsize( Pango::ELLIPSIZE_END );
    m_label_cache_value.set_ellipsize( Pango::ELLIPSIZE_END );
    m_label_size_value.set_ellipsize( Pango::ELLIPSIZE_END );
    m_label_write_name_value.set_ellipsize( Pango::ELLIPSIZE_END );
    m_label_write_mail_value.set_ellipsize( Pango::ELLIPSIZE_END );

    m_label_maxres.set_halign( Gtk::ALIGN_START );
    m_label_charset.set_halign( Gtk::ALIGN_START );

    m_button_clearmodified.set_halign( Gtk::ALIGN_END );
    m_bt_clear_post_history.set_halign( Gtk::ALIGN_END );

    m_grid_info.property_margin() = 16;
    m_grid_info.set_column_spacing( 10 );
    m_grid_info.set_row_spacing( 8 );

    // grid layout: width = 4, height = 10
    m_grid_info.attach( m_label_name, 0, 0, 1, 1 );
    m_grid_info.attach( m_label_name_value, 1, 0, 3, 1 );
    m_grid_info.attach( m_label_url, 0, 1, 1, 1 );
    m_grid_info.attach( m_label_url_value, 1, 1, 3, 1 );
    m_grid_info.attach( m_label_url_dat, 0, 2, 1, 1 );
    m_grid_info.attach( m_label_url_dat_value, 1, 2, 3, 1 );
    m_grid_info.attach( m_label_cache, 0, 3, 1, 1 );
    m_grid_info.attach( m_label_cache_value, 1, 3, 3, 1 );
    m_grid_info.attach( m_label_size, 0, 4, 1, 1 );
    m_grid_info.attach( m_label_size_value, 1, 4, 1, 1 );
    m_grid_info.attach( m_label_maxres, 2, 4, 1, 1 );
    m_grid_info.attach( m_spin_maxres, 3, 4, 1, 1 );

    m_grid_info.attach( m_label_since, 0, 5, 1, 1 );
    m_grid_info.attach( m_label_since_value, 1, 5, 1, 1 );

    m_grid_info.attach( m_label_modified, 0, 6, 1, 1 );
    m_grid_info.attach( m_label_modified_value, 1, 6, 1, 1 );
    m_grid_info.attach( m_button_clearmodified, 2, 6, 2, 1 );

    m_grid_info.attach( m_label_write, 0, 7, 1, 1 );
    m_grid_info.attach( m_label_write_value, 1, 7, 1, 1 );
    m_grid_info.attach( m_bt_clear_post_history, 2, 7, 2, 1 );

    if( DBTREE::write_fixname( get_url() ) ) m_label_write_name_value.set_text( DBTREE::write_name( get_url() ) );
    if( DBTREE::write_fixmail( get_url() ) ) m_label_write_mail_value.set_text( DBTREE::write_mail( get_url() ) );
    m_grid_info.attach( m_label_write_name, 0, 8, 1, 1 );
    m_grid_info.attach( m_label_write_name_value, 1, 8, 3, 1 );
    m_grid_info.attach( m_label_write_mail, 0, 9, 1, 1 );
    m_grid_info.attach( m_label_write_mail_value, 1, 9, 3, 1 );

    for( int y = 0; y < 10; ++y ) {
        // label column
        Gtk::Widget* child = m_grid_info.get_child_at( 0, y );
        child->set_halign( Gtk::ALIGN_START );

        // value column
        child = m_grid_info.get_child_at( 1, y );
        child->set_halign( Gtk::ALIGN_START );
        child->set_hexpand( true );
        static_cast<Gtk::Label*>( child )->set_selectable( true );
    }

    // あぼーん設定

    m_vbox_abone.set_border_width( 16 );
    m_vbox_abone.set_spacing( 8 );

    // 透明あぼーん
    m_check_transpabone.set_active( DBTREE::get_abone_transparent( get_url() ) );

    // 連鎖あぼーん
    m_check_chainabone.set_active( DBTREE::get_abone_chain( get_url() ) );

    // ageあぼーん
    m_check_ageabone.set_active( DBTREE::get_abone_age( get_url() ) );

    // デフォルト名無しあぼーん
    m_check_defnameabone.set_active( DBTREE::get_abone_default_name( get_url() ) );

    // ID無しあぼーん
    m_check_noidabone.set_active( DBTREE::get_abone_noid( get_url() ) );

    // 板レベルあぼーん
    m_check_boardabone.set_active( DBTREE::get_abone_board( get_url() ) );

    // 全体レベルあぼーん
    m_check_globalabone.set_active( DBTREE::get_abone_global( get_url() ) );

    if( CONFIG::get_abone_transparent() ) m_check_transpabone.set_sensitive( false );
    if( CONFIG::get_abone_chain() ) m_check_chainabone.set_sensitive( false );

    m_vbox_abone.pack_start( m_check_transpabone, Gtk::PACK_SHRINK );
    m_vbox_abone.pack_start( m_check_chainabone, Gtk::PACK_SHRINK );
    m_vbox_abone.pack_start( m_check_ageabone, Gtk::PACK_SHRINK );
    m_vbox_abone.pack_start( m_check_defnameabone, Gtk::PACK_SHRINK );
    m_vbox_abone.pack_start( m_check_noidabone, Gtk::PACK_SHRINK );
    m_vbox_abone.pack_start( m_check_boardabone, Gtk::PACK_SHRINK );
    m_vbox_abone.pack_start( m_check_globalabone, Gtk::PACK_SHRINK );

    if( CONFIG::get_abone_transparent() || CONFIG::get_abone_chain() ){
        m_label_abone.set_text( "チェック出来ない場合は設定メニューから「デフォルトで透明/連鎖あぼ〜ん」を解除して下さい" );
        m_label_abone.set_xalign( 0 );
        m_vbox_abone.pack_start( m_label_abone, Gtk::PACK_SHRINK );
    }

    if( DBTREE::article_is_cached( get_url() ) ){ 

        std::string str_res;

        // id
        std::list< std::string > list_id = DBTREE::get_abone_list_id( get_url() );
        m_edit_id.set_text( MISC::concat_with_suffix( list_id, '\n' ) );
        // あぼーんレス番号
        // 連番は 12-34 の様なフォーマットに変換
        const std::unordered_set< int >& set_res = DBTREE::get_abone_reses( get_url() );
        // レス番号をソートする
        const std::set< int > tmp_set{ set_res.begin(), set_res.end() };
        int pre_res = 0;
        int save = 0;
        for( const int res : tmp_set ) {
            if( !pre_res ) {
                save = res;
            }
            else if( res - pre_res > 1 ) {
                str_res.append( std::to_string( save ) );
                if( pre_res != save ) str_res.append( '-' + std::to_string( pre_res ) );
                str_res.push_back( '\n' );
                save = res;
            }
            pre_res = res;
        }
        if( !tmp_set.empty() ) {
            str_res.append( std::to_string( save ) );
            if( pre_res != save ) str_res.append( '-' + std::to_string( pre_res ) );
            str_res.push_back( '\n' );
        }

        m_edit_res.set_text( str_res );

        // name
        std::list< std::string > list_name = DBTREE::get_abone_list_name( get_url() );
        m_edit_name.set_text( MISC::concat_with_suffix( list_name, '\n' ) );

        // word
        std::list< std::string > list_word = DBTREE::get_abone_list_word( get_url() );
        m_edit_word.set_text( MISC::concat_with_suffix( list_word, '\n' ) );

        // regex
        std::list< std::string > list_regex = DBTREE::get_abone_list_regex( get_url() );
        m_edit_regex.set_text( MISC::concat_with_suffix( list_regex, '\n' ) );
    }
    else{
        m_edit_id.set_editable( false );
        m_edit_res.set_editable( false );
        m_edit_name.set_editable( false );
        m_edit_word.set_editable( false );
        m_edit_regex.set_editable( false );
    }

    m_label_abone_id.set_text(
        "ここでIDを削除してもレスが表示されない場合は板全体に対してIDがあぼーん指定されている可能性があります。\n"
        "板のプロパティのあぼーん設定も確認してください。\n"
        "連続投稿したIDをあぼーんする設定が優先されるためIDを削除しても再び登録されることがあります。"
        );

    m_vbox_abone.set_spacing( 8 );
    m_vbox_abone_id.pack_start( m_label_abone_id, Gtk::PACK_SHRINK );
    m_vbox_abone_id.pack_start( m_edit_id );

    m_notebook_abone.append_page( m_vbox_abone, "一般" );
    m_notebook_abone.append_page( m_vbox_abone_id, "NG ID" );
    m_notebook_abone.append_page( m_edit_res, "NG レス番号" );
    m_notebook_abone.append_page( m_edit_name, "NG 名前" );
    m_notebook_abone.append_page( m_edit_word, "NG ワード" );
    m_notebook_abone.append_page( m_edit_regex, "NG 正規表現" );

    m_notebook.append_page( m_grid_info, "一般" );
    const int page_abone = 1;
    m_notebook.append_page( m_notebook_abone, "あぼ〜ん設定" );

    get_content_area()->pack_start( m_notebook );
    set_title( "「" + MISC::to_plain( DBTREE::article_modified_subject( get_url() ) ) + "」のプロパティ" );
    resize( 600, 400 );
    show_all_children();

    if( command == "show_abone" ) m_notebook.set_current_page( page_abone );
}

Preferences::~Preferences() noexcept = default;


//
// OK 押した
//
void Preferences::slot_ok_clicked()
{
    // あぼーん再設定
    std::list< std::string > list_id = MISC::get_lines( m_edit_id.get_text() );
    std::list< std::string > list_name = MISC::get_lines( m_edit_name.get_text() );
    std::list< std::string > list_word = MISC::get_lines( m_edit_word.get_text() );
    std::list< std::string > list_regex = MISC::get_lines( m_edit_regex.get_text() );

    // あぼーんレス番号
    std::vector< char > vec_abone_res;
    vec_abone_res.resize( DBTREE::article_number_load( get_url() ) + 1 );
    std::list< std::string > list_res = MISC::get_lines( m_edit_res.get_text() );
    for( const std::string& num_str : list_res ) {

        int number = atoi( num_str.c_str() );
        if( number >= 1 ){
            int number_end = number;
            size_t pos = num_str.find( '-' );
            if( pos != std::string::npos ) {
                number_end = std::clamp( std::atoi( num_str.substr( pos + 1 ).c_str() ),
                                         number, static_cast<int>(vec_abone_res.size()) );
            }
            for( int i = number; i <= number_end; ++i ) vec_abone_res[ i ] = true;
        }
    }

    DBTREE::reset_abone( get_url(), list_id, list_name, list_word, list_regex, vec_abone_res
                         , m_check_transpabone.get_active(), m_check_chainabone.get_active(), m_check_ageabone.get_active(),
                         m_check_defnameabone.get_active(), m_check_noidabone.get_active(),
                         m_check_boardabone.get_active(), m_check_globalabone.get_active() );

    // 最大レス数
    DBTREE::article_set_number_max( get_url(), m_spin_maxres.get_value_as_int() );

    // charset
    if( m_combo_charset.get_mapped() ) {
        const std::string tmpcharset = m_combo_charset.get_active_text();
        const Encoding tmpencoding = MISC::encoding_from_sv( tmpcharset );
        if( tmpencoding != DBTREE::article_encoding( get_url() ) ){
            // Encodingを更新
            DBTREE::article_set_encoding( get_url(), tmpencoding );
            // Viewが開かれていない場合があるのでここでNodeTreeを削除する
            DBTREE::article_clear_nodetree( get_url() );
        }
    }

    // viewの再レイアウト
    CORE::core_set_command( "relayout_article", get_url() );
}


void Preferences::slot_clear_modified()
{
    DBTREE::article_set_date_modified( get_url(), "" );

    if( DBTREE::article_date_modified( get_url() ).empty() ) m_label_modified_value.set_text( "未取得" );
    else m_label_modified_value.set_text(
        MISC::timettostr( DBTREE::article_time_modified( get_url() ), MISC::TIME_WEEK )
        + " ( " + MISC::timettostr( DBTREE::article_time_modified( get_url() ), MISC::TIME_PASSED ) + " )"
        );

}


void Preferences::slot_clear_post_history()
{
    if( m_label_write_value.get_text().empty() ) return;

    SKELETON::MsgDiag mdiag( nullptr, "書き込み履歴を削除しますか？", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO );
    if( mdiag.run() != Gtk::RESPONSE_YES ) return;

    DBTREE::article_clear_post_history( get_url() );
    m_label_write_value.set_text( "" );
    CORE::core_set_command( "redraw_article" );

    // BoardViewの行を更新
    CORE::core_set_command( "update_board_item", DBTREE::url_boardbase( get_url() ), DBTREE::article_id( get_url() ) );
}
