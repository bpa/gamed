use async_trait::async_trait;
use gamed::{create, message, state, Client, Game};
use rand::Rng;
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::cmp::Ordering;
use tokio::sync::Mutex;

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
    pub fn guess(&mut self, number: usize, client: &Client) {
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
#[serde(tag = "cmd")]
struct HiLoGuess {
    number: usize,
    test: Option<String>,
}
#[derive(Deserialize)]
#[serde(tag = "cmd", rename_all = "snake_case")]
enum HiLoMessage {
    Guess(HiLoGuess),
    Quit,
}
struct GameImpl {
    game: Mutex<HiLo>,
}

#[async_trait]
impl gamed::Games for GameImpl {
    async fn on_connect(&self, _client: &mut gamed::Client) {
        println!("Client joined!");
    }

    async fn on_message(&self, client: &mut gamed::Client, msg: &str) {
        let message: serde_json::Result<HiLoMessage> = serde_json::from_str(msg);
        match message {
            Ok(message) => match message {
                HiLoMessage::Guess(message) => {
                    if let Some(test) = message.test {
                        client.error(&test);
                    }
                    self.game.lock().await.guess(message.number, client);
                }
                HiLoMessage::Quit => client.error("Quit not implemented"),
            },
            Err(e) => client.error(&format!("Bad message: {}", e)),
        }
    }

    async fn on_disconnect(&self, _client: &mut gamed::Client) {
        println!("Client quit!");
    }
}
#[tokio::main]
async fn main() -> Result<(), std::io::Error> {
    let gamed = std::sync::Arc::new(gamed::Gamed::new(Box::new(GameImpl {
        game: Mutex::new(HiLo::init()),
    })));

    let _res = tokio::join!(
        gamed::websocket(gamed.clone(), "127.0.0.1:3000"),
        gamed::tcp_client(gamed.clone(), "127.0.0.1:3001"),
        // gamed::tcp_admin(game.clone(), "127.0.0.1:3002"),
    );
    Ok(())
}
