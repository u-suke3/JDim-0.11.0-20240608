// ライセンス: GPL2
//
// 設定のデフォルト値
//

#ifndef _DEFAULTCONF_H
#define _DEFAULTCONF_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

namespace CONFIG
{
    enum{
        CONF_RESTORE_BOARD = 0,     // スレ一覧を復元
        CONF_RESTORE_ARTICLE = 0,   // スレを復元
        CONF_RESTORE_IMAGE = 0,     // 画像を復元
        CONF_MANAGE_WINPOS = 1,    // 自前でウィンドウ配置を管理する
        CONF_REF_PREFIX_SPACE = 1, // 参照文字( CONF_REF_PREFIX ) の後のスペースの数
        CONF_USE_PROXY_FOR2CH = 0, // 2ch 読み込み用プロクシを使用するか
        CONF_SEND_COOKIE_TO_PROXY_FOR2CH = 0, // 2ch 読み込み用プロクシにクッキーを送信するか
        CONF_PROXY_PORT_FOR2CH = 8080, // 2ch 読み込み用プロクシポート番号
        CONF_USE_FALLBACK_PROXY_FOR2CH = 0, ///< プロキシを使わない接続で過去ログが見つからなかったときは2ch読み込み用プロキシを使う
        CONF_USE_PROXY_FOR2CH_W = 0, // 2ch 書き込み用プロクシを使用するか
        CONF_SEND_COOKIE_TO_PROXY_FOR2CH_W = 0, // 2ch 書き込み用プロクシにクッキーを送信するか
        CONF_PROXY_PORT_FOR2CH_W = 8080, // 2ch 書き込み用プロクシポート番号
        CONF_USE_PROXY_FOR_DATA = 0, // 2ch 以外にアクセスするときにプロクシを使用するか
        CONF_SEND_COOKIE_TO_PROXY_FOR_DATA = 0, // 2ch 以外にアクセスするときのプロクシにクッキーを送信するか
        CONF_PROXY_PORT_FOR_DATA = 8080, // 2ch 以外にアクセスするときのプロクシポート番号
        CONF_LOADER_BUFSIZE = 32,   // ローダのバッファサイズ (一般)
        CONF_LOADER_BUFSIZE_BOARD = 2,   // ローダのバッファサイズ (スレ一覧用)
        CONF_LOADER_TIMEOUT = 10,   // ローダのタイムアウト値        
        CONF_LOADER_TIMEOUT_POST = 30, // ポストローダのタイムアウト値        
        CONF_LOADER_TIMEOUT_IMG = 30,  // 画像ローダのタイムアウト値
        CONF_LOADER_TIMEOUT_CHECKUPDATE = 10,  // 更新チェックのタイムアウト値
        CONF_USE_IPV6 = 1,          // ipv6使用
        CONF_TLS_NONBLOCKING = 1,   ///< TLSでノンブロッキングI/Oを使用する
        CONF_VERIFY_CERT = 1,       ///< TLSでサーバー証明書を検証する (unsafe to turn off)
        CONF_CONNECTION_NUM = 2,    // 同一ホストに対する最大コネクション数( 1 または 2 )
        CONF_USE_COOKIE_HAP = 0,    // 2chのクッキーを保存する (互換性のため設定名は旧名称を使う)
        CONF_REFPOPUP_BY_MO = 0,    // レス番号の上にマウスオーバーしたときに参照ポップアップ表示する
        CONF_NAMEPOPUP_BY_MO = 0,   // 名前の上にマウスオーバーしたときにポップアップ表示する
        CONF_IDPOPUP_BY_MO = 0,     // IDの上にマウスオーバーしたときにIDをポップアップ表示する
        CONF_USE_DARK_THEME = 0,    ///< @brief ダークテーマを使用するか
        CONF_USE_SYMBOLIC_ICON = 1, ///< @brief シンボリックアイコンを使用するか
        CONF_USE_MESSAGE_GTKTHEME = 0, // 書き込みビューでGTKテーマの設定を使用するか (GTK3版のみ)
        CONF_USE_TREE_GTKRC = 0,    // ツリービューでgtkrcの設定を使用するか
        CONF_USE_SELECT_GTKRC = 0,  ///< @brief スレビューの文字色、背景色、選択色でGTKテーマの設定を使用するか
        CONF_USE_COLOR_HTML = 1,    ///< スレビューでHTMLタグ指定の色を使用するか
        CONF_TREE_YPAD = 1,         // ツリービューの行間スペース
        CONF_TREE_SHOW_EXPANDERS = 1, // ツリービューにエクスパンダを表示
        CONF_TREE_LEVEL_INDENT = 0, // ツリービューのレベルインデント調整量(ピクセル)
        CONF_SCROLL_TREE = 1,       // カテゴリやディレクトリを開いたときにツリービューをスクロールする
        CONF_SELECT_ITEM_SYNC = 1,  // ツリービューの選択を表示中のビューと同期する ( 0: 同期しない 1: 同期する 2: 同期する(フォルダを開く) )
        CONF_VIEW_MARGIN = 0,       // 各ビューと枠との間の余白
        CONF_LEFT_SCRBAR = 0,       // スクロールバーを左に配置
        CONF_SHOW_OLDARTICLE = 0,   // スレ一覧で古いスレも表示
        CONF_NEWTHREAD_HOUR = 24,   // スレ一覧で指定した値(時間)よりも後に立てられたスレを新着とみなす
        CONF_INC_SEARCH_BOARD = 0,  // スレ一覧でインクリメント検索をする
        CONF_SHOW_DELDIAG = 1 ,     // スレ一覧でdeleteを押したときに確認ダイアログを表示する
        CONF_SHOW_CACHED_BOARD = 1, // スレ一覧をロードする前にキャッシュにある一覧を表示
        CONF_SHOW_924 = 1,          // スレ一覧でお知らせスレ(924)のアイコンを表示する
        CONF_TREE_SCROLL_SIZE = 4,  // ツリービューのスクロール量(行数)
        CONF_SCROLL_SIZE = 3,       // スレビューのスクロール量
        CONF_KEY_SCROLL_SIZE = 2,   // スレビューのスクロール量(キー上下)
        CONF_KEY_FASTSCROLL_SIZE = 2,  // スレビューの高速スクロール量(キー上下・ ページ高 - 行高 * key_fastscroll_size )
        CONF_JUMP_AFTER_RELOAD = 0, // スレビューでリロード後に一番下までスクロール
        CONF_JUMP_NEW_AFTER_RELOAD = 0, // スレビューでリロード後に新着までスクロール
        CONF_LIVE_SPEED = 2,        // 実況速度
        CONF_LIVE_THRESHOLD = 10,   // 実況のスクロールモードを切り替えるしきい値
        CONF_OPEN_ONE_CATEGORY = 0, // 板一覧でカテゴリを常にひとつだけ開く
        CONF_OPEN_ONE_FAVORITE = 0, // お気に入りでディレクトリを常にひとつだけ開く
        CONF_ALWAYS_WRITE_OK = 0,   // 書き込み時に書き込み確認ダイアログを出さない
        CONF_SAVE_POSTLOG = 0,      //書き込みログを保存
        CONF_SAVE_POSTHIST = 1,      //書き込み履歴を保存
        CONF_MAXSIZE_POSTLOG = ( 256 * 1024 ), // 書き込みログの最大サイズ
        CONF_HIDE_WRITING_DIALOG = 0, // 「書き込み中」のダイアログを表示しない
        CONF_SHOW_SAVEMSGDIAG = 1,  // 編集中のメッセージの保存確認ダイアログを表示する
        CONF_MESSAGE_WRAP = 1,      // 書き込みビューでテキストを折り返す
        CONF_FOLD_MESSAGE = 0,      // 非アクティブ時に書き込みビューを折りたたむ
        CONF_FOLD_IMAGE = 1,        // 非アクティブ時に画像ビューを折りたたむ
        CONF_KEEP_IM_STATUS = 0,    // 書き込み欄の日本語のON/OFF状態を保存
        CONF_MARGIN_POPUP = 30,     // レスアンカーとポップアップの間のマージン ( 垂直方向 )
        CONF_MARGIN_IMGPOPUP_X = 0, // レスアンカーと画像ポップアップの間のマージン ( 水平方向 )
        CONF_MARGIN_IMGPOPUP = CONF_MARGIN_POPUP,  // レスアンカーと画像ポップアップの間のマージン ( 垂直方向 )
        CONF_HIDE_POPUP_MSEC = 0,   // ポップアップが消えるまでの時間(ミリ秒)
        CONF_ENABLE_MG = 1,         // マウスジェスチャを有効
        CONF_MOUSE_RADIUS = 25,     // マウスジェスチャの判定開始半径
        CONF_NUMBERJMP_MSEC = 1000, // 数字入力ジャンプの待ち時間(ミリ秒)
        CONF_HISTORY_SIZE = 20,      // 履歴メニューの表示数
        CONF_HISTORYVIEW_SIZE = 100, // 履歴ビューの表示数
        CONF_AAHISTORY = 7,         // AA履歴の保持数
        CONF_INSTRUCT_POPUP = 100,  // 0以上なら多重ポップアップの説明を表示する
        CONF_INSTRUCT_TGLART = 1, // スレビューを開いたときにスレ一覧との切り替え方法を説明する
        CONF_INSTRUCT_TGLIMG = 1, // 画像ビューを開いたときにスレビューとの切り替え方法を説明する
        CONF_SHOW_DELARTDIAG = 1, // スレビューでdeleteを押したときに確認ダイアログを表示する
        CONF_ADJUST_UNDERLINE_POS = 1, // 下線位置
        CONF_ADJUST_LINE_SPACE = 1,    // 行間スペース
        CONF_DRAW_UNDERLINE = 1,     // リンク下線を表示
        CONF_STRICT_CHAR_WIDTH = 0,  // スレビューで文字幅の近似を厳密にする
        CONF_CHECK_ID = 1,           ///< スレビューで発言数(ID)とレスの順番(何番目の投稿)をカウントする
        CONF_NUM_REFERENCE_HIGH = 3,//レス参照で色を変える回数 (高) 
        CONF_NUM_REFERENCE_LOW = 1, //レス参照で色を変える回数 (低) 
        CONF_NUM_ID_HIGH = 4,       // 発言数で色を変える回数 (高)
        CONF_NUM_ID_LOW = 2,        // 発言数で色を変える回数 (低)
        CONF_LOOSE_URL = 1,         // datのパース時にURL判定を甘くする(^なども含める)
        CONF_PERCENT_DECODE = 0,    ///< URLのパーセントエンコーディングをデコードして表示する
        CONF_HIDE_USRCMD = 0, // ユーザーコマンドで選択できない項目を非表示にする
        CONF_RELOAD_ALLTHREAD = 0,  // スレビューで再読み込みボタンを押したときに全タブを更新する
        CONF_TAB_MIN_STR = 4, // タブに表示する文字列の最小値
        CONF_SHOW_TAB_ICON = 1, // タブにアイコンを表示するか
        CONF_SWITCHTAB_WHEEL = 1, // タブ上でマウスホイールを回転してタブを切り替える
        CONF_NEWTAB_POS = 1, // 他のビューを開くときのタブの位置 ( 0: 一番右端 1:右隣 2:左隣 )
        CONF_OPENTAB_POS = 0, // ツリービューで選択したビューを開くときのタブの位置 ( 0: 一番右端 1:右隣 2:左隣 )
        CONF_BOARDNEXTTAB_POS = 0, // 次スレ検索を開くときのタブの位置 ( 0: 次スレ検索タブ 1:新しいタブ 2:アクティブなタブを置き換え )
        CONF_SHOW_POST_MARK = 1, // スレビューに書き込みマークを表示するか
        CONF_FLAT_BUTTON = 1,    // ボタンをフラットにするか
        CONF_DRAW_TOOLBARBACK = 0, // ツールバーの背景描画
        CONF_IMGEMB_INTERP = 0,   // 埋め込み画像のスムージングレベル(0-2, 2が最も高画質かつ低速)
        CONF_IMGMAIN_INTERP = 0,   // 画像ビューのスムージングレベル(0-2, 2が最も高画質かつ低速)
        CONF_IMGPOPUP_INTERP = 0,   // 画像ポップアップのスムージングレベル(0-2, 2が最も高画質かつ低速)
        CONF_IMGPOPUP_WIDTH = 320,  // 画像ポップアップ幅
        CONF_IMGPOPUP_HEIGHT = 240, // 画像ポップアップ高さ
        CONF_USE_IMAGE_POPUP = 1,    // 画像ポップアップを使用する
        CONF_USE_IMAGE_VIEW = 1,    // 画像ビューを使用する
        CONF_INLINE_IMG = 0,        // インライン画像を表示する
        CONF_SHOW_SSSPICON = 1,     // sssp アイコン表示
        CONF_EMBIMG_WIDTH = 100, // インライン画像の最大幅
        CONF_EMBIMG_HEIGHT = 100, // インライン画像の最大高さ
        CONF_HIDE_IMAGETAB = 1,   // 埋め込み画像ビューを閉じたときにタブも閉じる
        CONF_SHOW_DELIMGDIAG = 1 ,     // 画像ビューでdeleteを押したときに確認ダイアログを表示する
        CONF_USE_MOSAIC = 1,        // 画像にモザイクをかける
        CONF_MOSAIC_SIZE = 60,        // モザイクの大きさ
        CONF_USE_GRAYSCALE_MOSAIC = 0, ///< 画像をモザイクで開くときはグレースケール(白黒)で表示する
        CONF_ZOOM_TO_FIT = 1,       // 画像をデフォルトでウィンドウサイズに合わせる
        CONF_DEL_IMG_DAY = 20,      // 画像キャッシュ削除の日数
        CONF_DEL_IMGABONE_DAY = 20, // 画像あぼーん削除の日数
        CONF_MAX_IMG_SIZE = 16,     // ダウンロードする画像の最大サイズ(Mbyte)
        CONF_MAX_IMG_PIXEL = 20,     // 画像の最大サイズ(Mピクセル)
        CONF_IMGCACHE_SIZE = 3,      // 画像のメモリキャッシュ枚数
        CONF_ENABLE_IMG_HASH = 0, ///< 画像のハッシュ値を計算してNG 画像ハッシュとの差がしきい値以下の画像をあぼ〜んする
        CONF_IMG_HASH_INITIAL_THRESHOLD = 30, ///< NG 画像ハッシュの初期設定のしきい値
        CONF_USE_LINK_AS_BOARD = 0,     // bbsmenu.html内にあるリンクは全て板とみなす
        CONF_SHOW_MOVEDIAG = 1,    // 板移転時に確認ダイアログを表示する
        CONF_REMOVE_OLD_ABONE_THREAD = 0, // dat落ちしたスレをNGスレタイトルリストから取り除くか( 0: ダイアログ表示 1: 取り除く 2: 除かない )
        CONF_ABONE_LOW_NUMBER_THREAD = 0,  // nレス以下のスレをあぼーんする
        CONF_ABONE_HIGH_NUMBER_THREAD = 0, // nレス以上のスレをあぼーんする
        CONF_ABONE_HOUR_THREAD = 0,   // スレあぼーん( スレ立てからの経過時間 )
        CONF_ABONE_TRANSPARENT = 0, // デフォルトで透明あぼーんをする
        CONF_ABONE_CHAIN = 0,       // デフォルトで連鎖あぼーんをする
        CONF_ABONE_ICASE = 0,       // NG正規表現によるあぼーん時に大小文字の違いを無視
        CONF_ABONE_WCHAR = 0,       // NG正規表現によるあぼーん時に全角半角文字の違いを無視
        CONF_SHOW_ABONE_REASON = 0, ///< (実験的な機能) あぼーんしたレスに判定理由を表示するか
        CONF_EXPAND_SIDEBAR = 0,      // 右ペーンが空の時にサイドバーを閉じる
        CONF_EXPAND_RPANE = 1,        // 3ペーン時にスレ一覧やスレビューを最大化する
        CONF_OPEN_SIDEBAR_BY_CLICK = 1, // ペーンの境界をクリックしてサイドバーを開け閉めする
        CONF_THRESHOLD_NEXT = 5,   // 次スレ検索の類似度のしきい値
        CONF_REPLACE_FAVORITE_NEXT = 1,  // 次スレを開いたときにお気に入りのアドレスと名前を自動更新
        CONF_SHOW_DIAG_REPLACE_FAVORITE = 1, // お気に入りの自動更新をするかダイアログを出す
        CONF_BOOKMARK_DROP = 0, // スレをお気に入りに追加したときにしおりをセットする
        CONF_CHECK_UPDATE_BOARD = 0,  // お気に入りの更新チェック時に板の更新もチェックする
        CONF_CHECK_UPDATE_BOOT = 0,  // 起動時にお気に入りを自動でチェックする
        CONF_CHECK_FAVORITE_DUP = 1, // お気に入り登録時に重複項目を登録するか ( 0: 登録する 1: ダイアログ表示 2: 登録しない )
        CONF_SHOW_FAVORITE_SELECT_DIAG = 0, // お気に入り登録時に挿入先ダイアログを表示する ( 0 : 表示する 1: 表示せず先頭に追加 2: 表示せず最後に追加 )
        CONF_DISABLE_CLOSE = 0, // Ctrl+qでウィンドウを閉じない
        CONF_SHOW_HIDE_MENUBAR_DIAG = 1, // メニューバーを非表示にした時にダイアログを表示
        CONF_CHANGE_STASTATUS_COLOR = 1, // 状態変更時にメインステータスバーの色を変える
        CONF_CHANGE_STATITLE_COLOR = 1, // 状態変更時にスレビュータイトルの色を変える
        CONF_USE_HEADER_BAR = 2, // Client-Side Decorationを使うか( 0: 使わない 1: 使う 2: デスクトップに合わせる )
        CONF_SHOW_MACHI_ID = 0, ///< まちBBSでID表示を使用する
        CONF_SHOW_DEL_WRITTEN_THREAD_DIAG = 1, // 書き込み履歴のあるスレを削除する時にダイアログを表示
        CONF_DELETE_IMG_IN_THREAD = 0, // スレを削除する時に画像キャッシュも削除する ( 0: ダイアログ表示 1: 削除 2: 削除しない )
        CONF_MAX_RESNUMBER = 65536, //最大表示可能レス数
#ifdef USE_PANGOLAYOUT
        CONF_TEXT_RENDERING_METHOD = 1, ///< スレビューのテキストを描画する方法 ( 0: PangoGlyphString 1: PangoLayout )
#else
        CONF_TEXT_RENDERING_METHOD = 0, ///< スレビューのテキストを描画する方法 ( 0: PangoGlyphString 1: PangoLayout )
#endif
        CONF_SHARE_QUERY_AMONG_TABS = 0, ///< (実験的な機能) スレビューの検索クエリをタブ間で共有する
        CONF_SHOW_DIAG_FIFO_ERROR = 1, // FIFOの作成などにエラーがあったらダイアログを表示する
        CONF_SAVE_SESSION = 0, // 指定した分ごとにセッションを自動保存 (0: 保存しない)
        CONF_BROKEN_SJIS_BE_UTF8 = 0, ///< 不正なMS932文字列をUTF-8と見なす
        CONF_CORRECT_CHAR_REFERENCE = 0, ///< 不正な数値文字参照(サロゲートペア)をデコードする
        CONF_CHOOSE_CHAR_ENCODING = 0, ///< スレ一覧とスレビューのプロパティにあるエンコーディング設定を有効にする (unsafe)
    };

// browsers.cpp のデフォルトのラベル番号
// browsers.cpp のブラウザの順番に気をつけること
    enum{
        // xdg-open をデフォルトにする
        CONF_BROWSER_NO = 1
    };

#define CONF_FONTSIZE_THREAD "12"
#define CONF_FONTSIZE_MAIL   "12"
#define CONF_FONTSIZE_POPUP  "10"
#define CONF_FONTSIZE_TREE   "10"

// レスを参照するときに前に付ける文字
#define CONF_REF_PREFIX ">"

// レスにアスキーアートがあると判定する正規表現
#define CONF_REGEX_RES_AA_DEFAULT "　 "
#define CONF_REGEX_RES_AA "\"" CONF_REGEX_RES_AA_DEFAULT "\""

// 2ch にアクセスするときのエージェント名
#define CONF_AGENT_FOR2CH "Monazilla/1.00 JD/2006.1.1"

// 2ch外にアクセスするときのエージェント名
#define CONF_AGENT_FOR_DATA "Monazilla/1.00 JD/2006.1.1"

// 2ch にログインするときのX-2ch-UA
#define CONF_X_2CH_UA "Navigator for 2ch 1.7.5"

// JD ホームページのアドレス
#define CONF_URL_JDHP "https://jd4linux.osdn.jp/"

// JDim ホームページのアドレス
#define CONF_URL_JDIMHP "https://github.com/JDimproved/JDim"

// 2chの認証サーバのアドレス
#define CONF_LOGIN2CH "https://2chv.tora3.net/futen.cgi"

// BEの認証サーバのアドレス
#define CONF_LOGINBE "http://be.2ch.net/test/login.php"

// bbsmenu.htmlのURL
#define CONF_URL_BBSMENU "https://menu.5ch.net/bbsmenu.html"

// スレタイ検索用メニュータイトルアドレス
#define CONF_MENU_SEARCH_TITLE  "スレタイ検索 (ff5ch.syoboi.jp)"
#define CONF_URL_SEARCH_TITLE "https://ff5ch.syoboi.jp/?alt=tsv&q=$TEXTU"

// スレタイ検索用正規表現
#define CONF_REGEX_SEARCH_TITLE R"=(^(?=[^\t\n]+\t[^\t\n]+\t([^\t\n]+))([^\t\n]+)\t([^\t\n]+))="

// WEB検索用メニュータイトルアドレス
#define CONF_MENU_SEARCH_WEB  "WEB検索 (google)"
#define CONF_URL_SEARCH_WEB "http://www.google.co.jp/search?hl=ja&q=$TEXTU&btnG=Google+%E6%A4%9C%E7%B4%A2&lr="

/// @brief GTKテーマの名前
#define CONF_GTK_THEME_NAME ""

/// @brief アイコンテーマの名前
#define CONF_GTK_ICON_THEME_NAME ""

// 2chのクッキー (互換性のため設定名は旧名称を使う)
#define CONF_COOKIE_HAP ""
#define CONF_COOKIE_HAP_BBSPINK ""

// 色
#define CONF_COLOR_CHAR    "#000000000000"     // スレの文字
#define CONF_COLOR_CHAR_NAME "#000064640000"   //名前欄の文字色
#define CONF_COLOR_CHAR_NAME_B "#000000008b8b" // トリップ等の名前欄の文字色
#define CONF_COLOR_CHAR_NAME_NOMAIL "#000064640000"   //メール無し時の名前欄の文字色
#define CONF_COLOR_CHAR_AGE "#fde800000000"    // ageの時のメール欄の文字色
#define CONF_COLOR_CHAR_SELECTION "#ffffffffffff"  // 選択範囲の文字色
#define CONF_COLOR_CHAR_HIGHLIGHT CONF_COLOR_CHAR  // ハイライトの文字色
#define CONF_COLOR_CHAR_HIGHLIGHT_TREE CONF_COLOR_CHAR  // ハイライトの文字色
#define CONF_COLOR_CHAR_LINK "#00000000ffff" //通常のリンクの文字色
#define CONF_COLOR_CHAR_LINK_ID_LOW  CONF_COLOR_CHAR_LINK // 複数発言したIDの文字色
#define CONF_COLOR_CHAR_LINK_ID_HIGH CONF_COLOR_CHAR_AGE // 多く発言したIDの文字色
#define CONF_COLOR_CHAR_LINK_RES CONF_COLOR_CHAR_LINK // 参照されていないレス番号の文字色
#define CONF_COLOR_CHAR_LINK_LOW "#ffff0000ffff" // 他のレスから参照されたレス番号の文字色
#define CONF_COLOR_CHAR_LINK_HIGH CONF_COLOR_CHAR_AGE  // 参照された数が多いレス番号の文字色
#define CONF_COLOR_CHAR_MESSAGE CONF_COLOR_CHAR  // メッセージビューの文字色
#define CONF_COLOR_CHAR_MESSAGE_SELECTION CONF_COLOR_CHAR_SELECTION  // メッセージビュー(選択範囲)の文字色

#define CONF_COLOR_IMG_NOCACHE "#a5a52a2a2a2a" // 画像(キャッシュ無)の色
#define CONF_COLOR_IMG_CACHED  "#00008b8b8b8b" // 画像(キャッシュ有)の色
#define CONF_COLOR_IMG_LOADING "#ffff8c8c0000"  // 画像(ロード中)の色
#define CONF_COLOR_IMG_ERR CONF_COLOR_CHAR_AGE  // 画像(エラー)の色

#define CONF_COLOR_BACK           "#fde8fde8f618" // スレ背景色
#define CONF_COLOR_BACK_POPUP     CONF_COLOR_BACK  // ポップアップ背景色
#define CONF_COLOR_BACK_SELECTION CONF_COLOR_CHAR_LINK  // 選択範囲の背景色

#define CONF_COLOR_BACK_HIGHLIGHT      "#ffffffff0000"  // ハイライト色
#define CONF_COLOR_BACK_HIGHLIGHT_TREE CONF_COLOR_BACK_HIGHLIGHT  // ツリーのハイライト色

#define CONF_COLOR_BACK_MESSAGE CONF_COLOR_BACK  // メッセージビューの背景色
#define CONF_COLOR_BACK_MESSAGE_SELECTION CONF_COLOR_BACK_SELECTION  // メッセージビューの選択色

#define CONF_COLOR_SEPARATOR_NEW "#7d007d007d00" // セパレータ

#define CONF_COLOR_FRAME CONF_COLOR_CHAR  // ポップアップフレーム色
#define CONF_COLOR_MARKER CONF_COLOR_CHAR  // オートスクロールマーカー色    

#define CONF_COLOR_CHAR_BBS   CONF_COLOR_CHAR  // 板一覧の文字
#define CONF_COLOR_CHAR_BBS_COMMENT  CONF_COLOR_CHAR  // 板一覧のコメント
#define CONF_COLOR_CHAR_BOARD CONF_COLOR_CHAR  // スレ一覧の文字

#define CONF_COLOR_BACK_BBS   CONF_COLOR_BACK  // 板一覧の背景色
#define CONF_COLOR_BACK_BBS_EVEN CONF_COLOR_BACK_BBS  // 板一覧の背景色(偶数行)
#define CONF_COLOR_BACK_BOARD CONF_COLOR_BACK  // スレ一覧の背景色
#define CONF_COLOR_BACK_BOARD_EVEN CONF_COLOR_BACK_BOARD  // スレ一覧の背景色(偶数行)

/// @brief ダークテーマ用のプリセット
constexpr const char* kDarkColors[] = {
    "",        // COLOR_NONE
    "#DCDCDC", // COLOR_CHAR  スレの文字
    "#ADDAA0", // COLOR_CHAR_NAME  名前欄の文字色
    "#89B4FA", // COLOR_CHAR_NAME_B  トリップや fusianasan 等、<b>が含まれている名前欄
    "#ADDAA0", // COLOR_CHAR_NAME_NOMAIL  メールが無いときの名前欄
    "#F08080", // COLOR_CHAR_AGE  非sageのメール欄
    "#F0F0F0", // COLOR_CHAR_SELECTION  選択範囲の文字
    "#222222", // COLOR_CHAR_HIGHLIGHT  ハイライトの文字
    "#222222", // COLOR_CHAR_HIGHLIGHT_TREE  ツリー表示における検索結果や抽出項目のハイライト文字色
    "#89B4FA", // COLOR_CHAR_LINK  通常のリンクの文字色
    "#89B4FA", // COLOR_CHAR_LINK_ID_LOW  複数発言したIDの文字色
    "#F08080", // COLOR_CHAR_LINK_ID_HIGH  多く発言したIDの文字色
    "#89B4FA", // COLOR_CHAR_LINK_RES  参照されていないレス番号の文字色
    "#EE82EE", // COLOR_CHAR_LINK_LOW  他のレスから参照されたレス番号の文字色
    "#F08080", // COLOR_CHAR_LINK_HIGH  参照された数が多いレス番号の文字色
    "#F0F0F0", // COLOR_CHAR_MESSAGE  メッセージビューの文字
    "#F0F0F0", // COLOR_CHAR_MESSAGE_SELECTION  メッセージビュー(選択範囲)の文字
    "#F0F0F0", // COLOR_CHAR_ENTRY_DEFAULT  Gtk::Entryのデフォルトの文字色
    "#FFA07A", // COLOR_IMG_NOCACHE  画像のリンク(キャッシュ無)
    "#66CDAA", // COLOR_IMG_CACHED  画像のリンク(キャッシュ有)
    "#FF8C00", // COLOR_IMG_LOADING  画像のリンク(ロード中)
    "#F08080", // COLOR_IMG_ERR  画像のリンク(エラー)
    "#323234", // COLOR_BACK  スレビューなど基本の背景
    "#38383A", // COLOR_BACK_POPUP  ポップアップの背景
    "#14539E", // COLOR_BACK_SELECTION  選択範囲
    "#F0E68C", // COLOR_BACK_HIGHLIGHT  ハイライト文字の背景色
    "#F0E68C", // COLOR_BACK_HIGHLIGHT_TREE  ハイライト文字の背景色(ツリー用)
    "#2C2C2C", // COLOR_BACK_MESSAGE  メッセージビューの背景色
    "#14539E", // COLOR_BACK_MESSAGE_SELECTION  メッセージビュー(選択範囲)の背景色
    "#38383A", // COLOR_BACK_ENTRY_DEFAULT  Gtk::Entryのデフォルトの背景色
    "#D0D0C0", // COLOR_SEPARATOR_NEW  新着セパレータ
    "#F0F0F0", // COLOR_FRAME  ポップアップフレーム色
    "#F0F0F0", // COLOR_MARKER  オートスクロールのマーク色
    "",        // USRCOLOR_BASE = END_COLOR_FOR_THREAD  cssで使用する色番号のベース
    "#F0F0F0", // COLOR_CHAR_BBS  板一覧の文字
    "#F0F0F0", // COLOR_CHAR_BBS_COMMENT  板一覧のコメント
    "#F0F0F0", // COLOR_CHAR_BOARD  スレ一覧の文字
    "#38383A", // COLOR_BACK_BBS  板一覧の背景
    "#38383A", // COLOR_BACK_BBS_EVEN  板一覧の背景(偶数行)
    "#38383A", // COLOR_BACK_BOARD  スレ一覧の背景
    "#38383A", // COLOR_BACK_BOARD_EVEN  スレ一覧の背景(偶数行)
    "",        // COLOR_NUM
};

#define CONF_WRITE_NAME ""  // デフォルトの書き込み名
#define CONF_WRITE_MAIL "sage"  // デフォルトのメールアドレス

// migemo-dictの場所
#ifdef MIGEMODICT
#define CONF_MIGEMO_PATH MIGEMODICT
#else    
#define CONF_MIGEMO_PATH "/usr/share/migemo/utf-8/migemo-dict"
#endif

}

#endif
