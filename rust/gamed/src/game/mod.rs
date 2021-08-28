pub mod lobby;
use std::{collections::HashMap, net::SocketAddr, sync::Mutex};
use tokio::sync::mpsc;

type Tx = mpsc::UnboundedSender<String>;

pub trait Game: Send {
    // fn new(players: Players, config: Map<String, Value>) -> Self;
}

pub struct Players {
    state: Mutex<HashMap<SocketAddr, Tx>>,
}

impl Players {
    #[gamed_macros::message]
    pub fn new() -> Self {
        Self {
            state: Mutex::new(HashMap::<SocketAddr, Tx>::new()),
        }
    }

    pub fn insert(&self, addr: SocketAddr, tx: Tx) {
        self.state.lock().unwrap().insert(addr, tx);
    }

    pub async fn broadcast(&self, _client: SocketAddr, msg: &String) {
        let mut peers = self.state.lock().unwrap();

        for peer in peers.iter_mut() {
            let _ = peer.1.send(msg.into());
        }
    }
    pub fn remove(&self, addr: &SocketAddr) {
        self.state.lock().unwrap().remove(addr);
    }
}
