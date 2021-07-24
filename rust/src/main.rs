mod game;

use std::{error::Error, io::Error as IoError, net::SocketAddr, sync::Arc};

use futures_util::{SinkExt, StreamExt};
use tokio::{
    net::{TcpListener, TcpStream},
    sync::mpsc,
};
use tokio_util::codec::{Framed, LinesCodec};
use tungstenite::protocol::Message;

use crate::game::Game;

#[tokio::main]
async fn main() -> Result<(), IoError> {
    let game = Arc::new(Game::new());

    let _res = tokio::join!(
        websocket_listener(game.clone(), "127.0.0.1:3000"),
        tcp_client_listener(game.clone(), "127.0.0.1:3001"),
        // tcp_admin_listener(game.clone(), "127.0.0.1:3002"),
    );
    Ok(())
}

async fn websocket_listener(state: Arc<Game>, addr: &str) -> Result<(), IoError> {
    let try_socket = TcpListener::bind(&addr).await;
    let listener = try_socket.expect("Failed to bind");

    while let Ok((stream, addr)) = listener.accept().await {
        tokio::spawn(handle_websocket(state.clone(), stream, addr));
    }

    Ok(())
}

async fn handle_websocket(state: Arc<Game>, stream: TcpStream, addr: SocketAddr) {
    let mut ws_stream = tokio_tungstenite::accept_async(stream)
        .await
        .expect("Error during the websocket handshake occurred");

    let (tx, mut rx) = mpsc::unbounded_channel::<String>();
    state.insert(addr, tx);

    loop {
        tokio::select! {
            Some(msg) = rx.recv() => {
                if let Err(_) = ws_stream.send(Message::Text(msg)).await{
                    break;
                }
            }
            result = ws_stream.next() => match result {
                Some(Ok(msg)) => {
                    if let Message::Text(txt) = msg {
                        state.broadcast(addr, &txt).await;
                    }
                }
                Some(Err(_)) => {
                }
                None => break,
            },
        }
    }

    state.remove(&addr);
}

async fn tcp_client_listener(state: Arc<Game>, addr: &str) -> Result<(), IoError> {
    let try_socket = TcpListener::bind(&addr).await;
    let listener = try_socket.expect("Failed to bind");

    while let Ok((stream, addr)) = listener.accept().await {
        let state = state.clone();
        tokio::spawn(async move {
            if let Err(e) = handle_tcp_client(state, stream, addr).await {
                println!("{}", e);
            }
        });
    }

    Ok(())
}

async fn handle_tcp_client(
    state: Arc<Game>,
    stream: TcpStream,
    addr: SocketAddr,
) -> Result<(), Box<dyn Error>> {
    let mut lines = Framed::new(stream, LinesCodec::new());

    let (tx, mut rx) = mpsc::unbounded_channel();
    state.insert(addr, tx);

    loop {
        tokio::select! {
            Some(msg) = rx.recv() => {
                lines.send(&msg).await?;
            }
            result = lines.next() => match result {
                Some(Ok(msg)) => {
                    state.broadcast(addr, &msg).await;
                }
                Some(Err(e)) => {
                    println!("{}", e);
                }
                None => break,
            },
        }
    }

    state.remove(&addr);
    Ok(())
}
