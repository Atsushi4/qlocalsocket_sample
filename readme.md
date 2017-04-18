# Usage

- terminal1
$ localsocket
  -> output log

- terminal2
$ localsocket --logging-rules *.debug=false
  -> stop terminal1 debug log
$ localsocket.exe --message-pattern "%{time hh:mm:ss.zzz} %{pid}[%{threadid}]:(%{type}%{if-category}/%{category}%{endif}):%{file}(%{line}):%{message}"
  -> change terminal1 message pattern

# brief
Qt勉強会 @Tokyo #46 (https://qt-users.connpass.com/event/54738/) で書いたコードです。
おおむね↓こんなカンジのサンプルです

1. QLocalSocketを使って、多重起動を検知する
2. QLocalSocketを使って、先行起動しているプロセスにコマンドライン引数を渡す
3. QLoggingCategoryを使ってログを出力する
4. プロセスの実行中にログ出力ルールとメッセージのフォーマットを変更する

- "--logging-rules *.debug=false:*.info=false"ってやるとなんか怒られる。QT_LOGGING_RULES環境変数に設定したら動くのに。
