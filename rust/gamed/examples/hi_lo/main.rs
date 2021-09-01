use async_trait::async_trait;
use gamed::{
    create,
    game::lobby::{LobbycreateMessage, LobbyjoinMessage},
    message, state, Client, Game,
};
use rand::Rng;
use serde::{Deserialize, Serialize};
use std::{cmp::Ordering, sync::Arc};

#[derive(Game)]
struct HiLo {
    answer: u8,
    guesses: usize,
}

#[derive(Serialize)]
struct Reply {
    guesses: usize,
    answer: String,
}

#[state(HiLo)]
impl HiLo {
    #[create]
    pub fn init() -> HiLo {
        HiLo {
            answer: rand::thread_rng().gen_range(1..100),
            guesses: 0,
        }
    }

    #[message]
    pub fn guess(&mut self, number: usize, client: Arc<Client>) {
        self.guesses += 1;
        let answer = match number.cmp(&(self.answer as usize)) {
            Ordering::Less => "Too low",
            Ordering::Equal => "Correct",
            Ordering::Greater => "Too high",
        };
        client.send(&Reply {
            guesses: self.guesses,
            answer: answer.to_string(),
        });
        if number == self.answer as usize {
            self.answer = rand::thread_rng().gen();
            self.guesses = 0;
        }
    }
}
#[derive(Deserialize)]
struct HiLoGuess {
    number: usize,
    test: Option<String>,
}

#[derive(Deserialize)]
#[serde(tag = "cmd", rename_all = "snake_case")]
enum LobbyMessage {
    Games,
    Create(LobbycreateMessage),
    Join(LobbyjoinMessage),
    Quit,
}

#[derive(Deserialize)]
#[serde(tag = "cmd", rename_all = "snake_case")]
enum HiLoMessage {
    Guess(HiLoGuess),
    Quit,
}
struct GameImpl {
    lobby_instances: gamed::LockList<gamed::Lobby>,
    hilo_instances: gamed::LockList<HiLo>,
    names: Vec<String>,
}

#[async_trait]
impl gamed::Games for GameImpl {
    fn names(&self) -> &Vec<String> {
        &self.names
    }
    async fn on_connect(&self, _client: std::sync::Arc<gamed::Client>) {
        println!("Client joined!");
    }

    async fn on_message(&self, client: std::sync::Arc<gamed::Client>, msg: &str) {
        match client.game {
            0 => {
                if let Some(game) = self.lobby_instances.get(client.instance).await {
                    let game = game.lock().await;
                    let message: serde_json::Result<LobbyMessage> = serde_json::from_str(msg);
                    match message {
                        Ok(message) => match message {
                            LobbyMessage::Games => game.games(client, self),
                            LobbyMessage::Create(a) => {
                                game.create(client, a.game, a.name, self).await
                            }
                            LobbyMessage::Join(a) => game.join(client, a.name, self),
                            LobbyMessage::Quit => todo!(),
                        },
                        Err(e) => client.error(&format!("Bad message: {}", e)),
                    }
                }
            }
            1 => {
                if let Some(game) = self.hilo_instances.get(client.instance).await {
                    let message: serde_json::Result<HiLoMessage> = serde_json::from_str(msg);
                    match message {
                        Ok(message) => match message {
                            HiLoMessage::Guess(message) => {
                                game.lock().await.guess(message.number, client);
                            }
                            HiLoMessage::Quit => client.error("Quit not implemented"),
                        },
                        Err(e) => client.error(&format!("Bad message: {}", e)),
                    }
                }
            }
            _ => {}
        }
    }

    async fn on_disconnect(&self, _client: std::sync::Arc<gamed::Client>) {
        println!("Client quit!");
    }
}
#[tokio::main]
async fn main() -> Result<(), std::io::Error> {
    let gamed = std::sync::Arc::new(gamed::Gamed::new(Box::new(GameImpl {
        lobby_instances: gamed::LockList::new(),
        hilo_instances: gamed::LockList::new(),
        names: vec!["HiLo"],
    })));

    let _res = tokio::join!(
        gamed::websocket(gamed.clone(), "127.0.0.1:3000"),
        gamed::tcp_client(gamed.clone(), "127.0.0.1:3001"),
        // gamed::tcp_admin(game.clone(), "127.0.0.1:3002"),
    );
    Ok(())
}
