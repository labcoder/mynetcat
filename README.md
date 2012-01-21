<!doctype html>
<html>
  <head>
    <title>mynetcat</title>
  </head>
  <body>
    <h1>mynetcat</h1>
    <p>mynetcat is a simple netcat program which handles UDP packets.</p>
    <p>This can be used as either a server or a client. To test it out, simply open two terminals (actually, you can use your own server/client and just use mynetcat separately). On one terminal, run the server version:</p>
    <pre>./mynetcat -l -u -p port [-C]</pre>
    <p>and on another, run the client:</p>
    <pre>./mynetcat -p port -u -h host [message(s) to send]</pre>
    <hr>
    <h2>Usage:</h2>
    In the first form, mynetcat listens on the given port for<br>
    UDP packets and writes them to standard out. With the C option<br>
    they are written both in hexidecimal and (when possible) as<br>
    ASCII, in a manned such as the hexdump -C command.<br><br>
    
    In the second form, takes data from stdin and on close of stdin<br>
    sends the data as a UDP packet to the given port and host.<br><br>
    
    The host can be either a raw IP or a DNS name. The port number will<br>
    be in decimal.
    <hr>
    <h2>Example</h2>
    Set up the server:<br><pre>./mynetcat -l -u -p 4444 -C</pre>
    Set up the client:<br><pre>./mynetcat -p 4444 -u -h localhost hey there! This is an example message.</pre>
    And your output should echo what you wrote.
  </body>
</html>
