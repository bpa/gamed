use serde::Serialize;
use tokio::sync::mpsc;

pub struct Client {
    pub game: usize,
    pub instance: usize,
    pub player: usize,
    pub(crate) tx: mpsc::UnboundedSender<String>,
}

#[derive(Serialize)]
#[serde(tag = "cmd", rename = "error")]
struct Error<'a> {
    reason: &'a str,
}
impl Client {
    pub fn new(
        game: usize,
        instance: usize,
        player: usize,
        tx: mpsc::UnboundedSender<String>,
    ) -> Self {
        Self {
            game,
            instance,
            player,
            tx,
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
