#Usage

- terminal1
$ localsocket
  -> output log

- terminal2
$ localsocket --logging-rules *.debug=false
  -> stop terminal1 debug log
$ localsocket --message-pattern localsocket.exe --message-pattern %{time}:%{pid}[%{threadid}](%{type}%{if-category}/%{category}%{endif}):%{message}
  -> change terminal1 message pattern
