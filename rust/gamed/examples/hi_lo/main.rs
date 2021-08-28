use serde::Serialize;
use serde_json::Value;
use std::cmp::Ordering;

use async_trait::async_trait;
use gamed::{create, message, state, Client, Game};
use rand::Rng;
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

struct GameImpl {
    game: Mutex<HiLo>,
}

#[async_trait]
impl gamed::Games for GameImpl {
    async fn on_connect(&self, _client: &mut gamed::Client) {
        println!("Client joined!");
    }

    async fn on_message(
        &self,
        client: &mut gamed::Client,
        msg: serde_json::Map<String, serde_json::Value>,
    ) {
        let cmd = msg.get("cmd").unwrap();
        if let Value::String(cmd) = cmd {
            println!("Client sent {}", cmd);
            match cmd.as_str() {
                "guess" => {
                    if let Some(number) = msg.get("number") {
                        if let Value::Number(number) = number {
                            if let Some(number) = number.as_u64() {
                                let mut game = self.game.lock().await;
                                game.guess(number as usize, client);
                            }
                        }
                    }
                }
                "quit" => client.error("Quit not implemented"),
                _ => client.error(&format!("Unknown cmd `{}`", cmd)),
            }
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
