use std::{error::Error, io::Error as IoError, net::SocketAddr, sync::Arc};

use futures_util::{SinkExt, StreamExt};
use tokio::{
    net::{TcpListener, TcpStream},
    sync::mpsc,
};
use tokio_util::codec::{Framed, LinesCodec};
use tungstenite::protocol::Message;

use crate::gamed::Gamed;

pub async fn websocket(gamed: Arc<Gamed>, addr: &str) -> Result<(), IoError> {
    let try_socket = TcpListener::bind(&addr).await;
    let listener = try_socket.expect("Failed to bind");

    while let Ok((stream, addr)) = listener.accept().await {
        tokio::spawn(handle_websocket(gamed.clone(), stream, addr));
    }

    Ok(())
}

pub async fn handle_websocket(gamed: Arc<Gamed>, stream: TcpStream, addr: SocketAddr) {
    let mut ws_stream = tokio_tungstenite::accept_async(stream)
        .await
        .expect("Error during the websocket handshake occurred");

    let (tx, mut rx) = mpsc::unbounded_channel::<String>();
    let client_id = gamed.on_connect(addr, tx).await;

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
                        gamed.on_message(client_id, &txt).await;
                    }
                }
                Some(Err(_)) => {
                }
                None => break,
            },
        }
    }

    gamed.on_disconnect(client_id).await;
}

pub async fn tcp_client(gamed: Arc<Gamed>, addr: &str) -> Result<(), IoError> {
    let try_socket = TcpListener::bind(&addr).await;
    let listener = try_socket.expect("Failed to bind");

    while let Ok((stream, addr)) = listener.accept().await {
        let gamed = gamed.clone();
        tokio::spawn(async move {
            if let Err(e) = handle_tcp_client(gamed, stream, addr).await {
                println!("{}", e);
            }
        });
    }

    Ok(())
}

pub async fn handle_tcp_client(
    gamed: Arc<Gamed>,
    stream: TcpStream,
    addr: SocketAddr,
) -> Result<(), Box<dyn Error>> {
    let mut lines = Framed::new(stream, LinesCodec::new());

    let (tx, mut rx) = mpsc::unbounded_channel();
    let client_id = gamed.on_connect(addr, tx).await;

    loop {
        tokio::select! {
            Some(msg) = rx.recv() => {
                lines.send(&msg).await?;
            }
            result = lines.next() => match result {
                Some(Ok(msg)) => {
                    gamed.on_message(client_id, &msg).await;
                }
                Some(Err(e)) => {
                    println!("{}", e);
                }
                None => break,
            },
        }
    }

    gamed.on_disconnect(client_id).await;
    Ok(())
}
