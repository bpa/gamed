use serde::Serialize;
use std::cmp::Ordering;

use async_trait::async_trait;
use gamed::{create, message, state, Game};
use rand::Rng;

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
            answer: rand::thread_rng().gen(),
            guesses: 0,
        }
    }

    #[message]
    pub async fn guess(number: usize, hilo: &mut HiLo, client: gamed::Client) {
        hilo.guesses += 1;
        let answer = match number.cmp(&(hilo.answer as usize)) {
            Ordering::Less => "Too low",
            Ordering::Equal => "Correct",
            Ordering::Greater => "Too high",
        };
        client
            .send(&Reply {
                guesses: hilo.guesses,
                answer: answer.to_string(),
            })
            .await;
        if number == hilo.answer as usize {
            hilo.answer = rand::thread_rng().gen();
            hilo.guesses = 0;
        }
    }
}

enum GamedMessages {
    Quit,
}
struct HiLoGuess {
    number: usize,
}
enum HiLoMessages {
    Guess(HiLoGuess),
}
enum HiloGameMessages {
    HiLo(HiLoMessages),
    Gamed(GamedMessages),
}

struct GameImpl {}

#[async_trait]
impl gamed::Games for GameImpl {
    async fn on_connect(&self, client: &mut gamed::Client) {
        todo!()
    }

    async fn on_message(&self, client: &mut gamed::Client, msg: &String) {
        todo!()
    }

    async fn on_disconnect(&self, client: &mut gamed::Client) {
        todo!()
    }
}

gamed::main!(Box::new(GameImpl {}));
