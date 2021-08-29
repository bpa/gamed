use async_trait::async_trait;
use serde_json::Value;
use std::net::SocketAddr;
use std::sync::Arc;

use tokio::sync::{mpsc, Mutex};

use crate::client::Client;

type Tx = mpsc::UnboundedSender<String>;

#[async_trait]
pub trait Games: Sync + Send {
    async fn on_connect(&self, client: &mut Client);
    async fn on_message(&self, client: &mut Client, msg: &str);
    async fn on_disconnect(&self, client: &mut Client);
}

pub struct Gamed {
    clients: Mutex<(Vec<Arc<Mutex<Client>>>, Vec<usize>)>,
    // state: Mutex<HashMap<SocketAddr, Tx>>,
    handler: Box<dyn Games>,
}

impl Gamed {
    pub fn new(handler: Box<dyn Games>) -> Self {
        Self {
            clients: Mutex::new((Vec::new(), Vec::new())),
            // state: Mutex::new(HashMap::<SocketAddr, Tx>::new()),
            handler,
        }
    }

    pub async fn on_connect(&self, _addr: SocketAddr, tx: Tx) -> usize {
        let (client_arc, client_id) = {
            let mut clients = self.clients.lock().await;
            let client_id = match clients.1.pop() {
                Some(ind) => ind,
                None => {
                    let ind = clients.0.len();
                    let client_arc = Arc::new(Mutex::new(Client::new(
                        Arc::new(Mutex::new(crate::Lobby {})),
                        tx,
                    )));
                    clients.0.push(client_arc.clone());
                    ind
                }
            };
            let client = clients.0[client_id].clone();
            (client, client_id)
        };
        let mut client = client_arc.lock().await;
        self.handler.on_connect(&mut client).await;

        client_id
    }

    pub async fn on_message(&self, client_id: usize, msg: &String) {
        let client = { self.clients.lock().await.0[client_id].clone() };
        let mut client = client.lock().await;

        println!("Message from client: {}", msg);
        self.handler.on_message(&mut client, msg).await;
    }

    pub async fn on_disconnect(&self, client_id: usize) {
        let client = { self.clients.lock().await.0[client_id].clone() };
        let mut client = client.lock().await;
        self.handler.on_disconnect(&mut client).await;

        let mut clients = self.clients.lock().await;
        clients.1.push(client_id);
    }
}
