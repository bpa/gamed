use async_trait::async_trait;
use serde::Serialize;
use std::net::SocketAddr;
use std::sync::Arc;

use tokio::sync::{mpsc, Mutex, RwLock};

use crate::client::Client;

type Tx = mpsc::UnboundedSender<String>;

#[derive(Serialize)]
pub struct GameMetaData {
    name: String,
    game: String,
    players: usize,
    status: String,
}
#[async_trait]
pub trait Games: Sync + Send {
    async fn on_connect(&self, client: Arc<Client>);
    async fn on_message(&self, client: Arc<Client>, msg: &str);
    async fn on_disconnect(&self, client: Arc<Client>);
    async fn create_instance(&self, client: Arc<Client>, game: String, name: String) {}
    fn join(&self, client: Arc<Client>, name: String) {}
    fn names(&self) -> &Vec<String>;
}

pub struct Gamed {
    clients: RwLock<(Vec<Arc<Client>>, Vec<usize>)>,
    // state: Mutex<HashMap<SocketAddr, Tx>>,
    handler: Box<dyn Games>,
}

impl Gamed {
    pub fn new(handler: Box<dyn Games>) -> Self {
        Self {
            clients: RwLock::new((Vec::new(), Vec::new())),
            // state: Mutex::new(HashMap::<SocketAddr, Tx>::new()),
            handler,
        }
    }

    pub async fn on_connect(&self, _addr: SocketAddr, tx: Tx) -> usize {
        let (client, client_id) = {
            let mut clients = self.clients.write().await;
            let client_id = match clients.1.pop() {
                Some(ind) => ind,
                None => {
                    let ind = clients.0.len();
                    let client = Arc::new(Client::new(0, 0, 0, tx));
                    clients.0.push(client.clone());
                    ind
                }
            };
            let client = clients.0[client_id].clone();
            (client, client_id)
        };
        self.handler.on_connect(client).await;

        client_id
    }

    pub async fn on_message(&self, client_id: usize, msg: &String) {
        let client = { self.clients.read().await.0[client_id].clone() };

        println!("Message from client: {}", msg);
        self.handler.on_message(client, msg).await;
    }

    pub async fn on_disconnect(&self, client_id: usize) {
        let client = { self.clients.read().await.0[client_id].clone() };
        self.handler.on_disconnect(client).await;

        let mut clients = self.clients.write().await;
        clients.1.push(client_id);
    }
}
