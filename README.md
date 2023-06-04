# Basic Websocketpp Client Example

This is a basic example of a websocketpp client, which maintains a single connection to a websocket server. It waits for user input and sends it to the server in a loop.

If the connection to the server is lost, it will attempt to reconnect upon every subsequent message. If the connection is reestablished, it will resume sending messages.

It is intended to be a very barebones example derived from the [full example](https://github.com/zaphoyd/websocketpp/blob/master/tutorials/utility_client/step6.cpp) on the websocketpp website. It does not contain any code for handlers or other features.

See the comments in `main.cpp` for more information.

See the following for other useful examples and explanation:
 - https://github.com/zaphoyd/websocketpp/blob/master/tutorials/utility_client/utility_client.md
 - https://gist.github.com/zaphoyd/6021634

## Building and Running

Install wscat with `npm install -g wscat`.

To build the `ws_test` executable, run the following from the root of the repo:
```bash
cmake .
make
```

Then in one terminal run
```bash
wscat --listen 12000
```

and in another run
```bash
./ws_test
```

To test reconnect functionality, kill the wscat process and restart it. The client should reconnect upon the next attempt to send a message and resume sending messages.

