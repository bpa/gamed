use serde::Serialize;
use std::sync::Arc;

use tokio::sync::{mpsc, Mutex};

use crate::{game::Game, player::Player};

pub struct Client {
    pub(crate) _game: Arc<Mutex<dyn Game>>,
    _player: Arc<Mutex<Player>>,
    pub(crate) tx: mpsc::UnboundedSender<String>,
}

#[derive(Serialize)]
#[serde(tag = "cmd", rename = "error")]
struct Error<'a> {
    reason: &'a str,
}
impl Client {
    pub fn new(game: Arc<Mutex<dyn Game>>, tx: mpsc::UnboundedSender<String>) -> Self {
        Self {
            _game: game,
            tx,
            _player: Arc::new(Mutex::new(Player {})),
        }
    }

    pub fn error(&self, reason: &str) {
        self.send(&Error { reason });
    }

    pub fn send<T>(&self, message: &T)
    where
        T: ?Sized + Serialize,
    {
        match serde_json::to_string(message) {
            Ok(s) => self.tx.send(s).unwrap(),
            Err(e) => self
                .tx
                .send(
                    serde_json::to_string(&Error {
                        reason: &e.to_string(),
                    })
                    .unwrap(),
                )
                .unwrap(),
        };
    }
}
