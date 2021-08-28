use serde::ser::Serialize;
use std::sync::Arc;

use tokio::sync::{mpsc, Mutex};

use crate::{game::Game, player::Player};

pub struct Client {
    pub(crate) _game: Arc<Mutex<dyn Game>>,
    _player: Arc<Mutex<Player>>,
    pub(crate) _tx: mpsc::UnboundedSender<String>,
}

impl Client {
    pub fn new(game: Arc<Mutex<dyn Game>>, tx: mpsc::UnboundedSender<String>) -> Self {
        Self {
            _game: game,
            _tx: tx,
            _player: Arc::new(Mutex::new(Player {})),
        }
    }

    pub async fn send<T>(&self, _message: &T)
    where
        T: ?Sized + Serialize,
    {
    }
}
