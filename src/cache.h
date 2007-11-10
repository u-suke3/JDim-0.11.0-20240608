// ライセンス: GPL2

// キャッシュ、ファイル操作まわり

#ifndef _CACHE_H
#define _CACHE_H

#include <string>
#include <list>

namespace Gtk
{
    class Window;
    class FileChooserDialog;
}

namespace CACHE
{
    /////////////////////////////////////////////////
    //
    // 設定ファイルのパス

    // 設定ファイル
    std::string path_conf();
    std::string path_conf_old();  // 旧ファイル

    // セッション情報ファイル
    std::string path_session();

    // ディストリ判別用ファイル
    std::string path_issue_net();

    // ロックファイル
    std::string path_lock();

    // パスワード保存ファイル
    std::string path_passwd( const std::string& basename );

    // キャッシュルートの絶対パス
    std::string path_root();

    // 板
    std::string path_xml_listmain();
    std::string path_xml_listmain_bkup();
    std::string path_xml_listmain_old();

    // お気に入り
    std::string path_xml_favorite();
    std::string path_xml_favorite_bkup();
    std::string path_xml_favorite_old();

    // 外部板設定ファイル( navi2ch 互換 )
    std::string path_etcboard();

    // ユーザーコマンド設定ファイル
    std::string path_usrcmd();

    // スレ履歴
    std::string path_xml_history();
    std::string path_xml_history_old();

    // 板履歴
    std::string path_xml_history_board();
    std::string path_xml_history_board_old();

    // 最近閉じたスレの履歴
    std::string path_xml_history_close();
    std::string path_xml_history_close_old();

    // 板移転情報
    std::string path_movetable();

    // キーボード設定
    std::string path_keyconf();

    // マウスジェスチャ設定
    std::string path_mouseconf();

    // マウスボタン設定
    std::string path_buttonconf();

    std::string path_board_root( const std::string& url );
    std::string path_board_root_fast( const std::string& boardbase );
    std::string path_article_summary( const std::string& url );
    std::string path_board_info( const std::string& url );
    std::string path_jdboard_info( const std::string& url );
    std::string path_article_info_root( const std::string& url );
    std::string path_article_info( const std::string& url, const std::string& id );
    std::string path_article_ext_info( const std::string& url, const std::string& id );        
    std::string path_dat( const std::string& url );

    // 画像関係
    std::string path_img_root();
    std::string path_img_info_root();
    std::string path_img_protect_root();
    std::string path_img_protect_info_root();
    std::string path_img_abone_root();
    std::string filename_img( const std::string& url );
    std::string filename_img_info( const std::string& url );
    std::string path_img( const std::string& url );
    std::string path_img_info( const std::string& url );
    std::string path_img_protect( const std::string& url );
    std::string path_img_protect_info( const std::string& url );
    std::string path_img_abone( const std::string& url );

    // AA
    std::string path_aalist(); // アスキーアートファイル
    std::string path_aadir();  // アスキーアートファイル格納用ディレクトリ .jd/aa/
    std::string path_aahistory();  // AAの使用履歴ファイル

    // css
    std::string path_css();

    // html
    std::string path_reshtml();

    // 書き込みログ
    std::string path_postlog();

    // 検索や名前などの補完情報
    std::string path_completion( int mode );

    /////////////////////////////////////////////////
    //
    // ユーティリティ関数

    // file_exists の戻り値
    enum
    {
        EXIST_FILE = 0,  // ファイル
        EXIST_DIR,       // ディレクトリ
        EXIST,           // 存在しない or 何か存在してる
        EXIST_ERROR      // エラー
    };

    // ファイルタイプ( file_open_diag() で使用 )
    enum
    {
        FILE_TYPE_ALL = 0,
        FILE_TYPE_TEXT,
        FILE_TYPE_DAT
    };

    // キャッシュの mkdir 関係
    bool mkdir_root();
    bool mkdir_imgroot();
    bool mkdir_imgroot_favorite();
    bool mkdir_parent_of_board( const std::string& url );
    bool mkdir_boardroot( const std::string& url );

    // 生データ読み書き
    size_t load_rawdata( const std::string& path, std::string& str );
    size_t load_rawdata( const std::string& path, char* data, size_t n );    
    size_t save_rawdata( const std::string& path, const std::string& str, bool append = false );
    size_t save_rawdata( const std::string& path, const char* data, size_t n, bool append = false );

    // ファイル情報
    const int file_exists( const std::string& path );
    size_t get_filesize( const std::string& path );
    time_t get_filemtime( const std::string& path );

    // ファイル操作
    bool jdmkdir( const std::string& path );
    bool jdcopy( const std::string& file_from, const std::string& file_to );
    bool jdmv( const std::string& file_from, const std::string& file_to );

    // 保存ダイアログを表示して file_from を file_to に保存する
    std::string copy_file( Gtk::Window* parent, const std::string& file_from, const std::string& file_to, const int type );

    // ファイル選択ダイアログにフィルタ追加
    void add_filter_to_diag( Gtk::FileChooserDialog& diag, const int type );

    // ファイル選択ダイアログを表示する
    std::string open_load_diag( Gtk::Window* parent, const std::string& open_path, const int type );

    // 保存ファイル選択ダイアログを表示する
    std::string open_save_diag( Gtk::Window* parent, const std::string& dir, const std::string& name, const int type );

    // dir ディレクトリ内のレギュラーファイルのリストを取得
    std::list< std::string > get_filelist( const std::string& dir );

    // dir ディレクトリ内のレギュラーファイルの合計サイズを取得
    int64_t get_dirsize( const std::string& dir );
}

#endif
