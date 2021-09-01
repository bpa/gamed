use crate::{client::Client, Games};
use gamed_macros::{message, state};
use std::sync::Arc;

pub struct Lobby {}
impl crate::game::Game for Lobby {}

//TODO: make macro
#[derive(serde::Deserialize)]
pub struct LobbycreateMessage {
    pub game: String,
    pub name: String,
}
#[derive(serde::Deserialize)]
pub struct LobbyjoinMessage {
    pub name: String,
}
//END TODO
#[derive(serde::Serialize)]
pub struct LobbygamesClientMessage {
    games: Vec<crate::gamed::GameMetaData>,
}
#[derive(serde::Serialize)]
pub struct LobbycreateClientMessage {
    name: String,
    game: String,
}
#[derive(serde::Serialize)]
#[serde(tag = "cmd")]
#[allow(non_camel_case_types)]
enum LobbyClientMessage {
    games(LobbygamesClientMessage),
    create(LobbycreateClientMessage),
}
#[state(Lobby)]
impl Lobby {
    #[message]
    pub fn games(&self, client: Arc<Client>, _games: Box<dyn Games>) {
        client.send(&LobbygamesClientMessage { games: vec![] });
    }

    #[message]
    pub async fn create(
        &self,
        client: Arc<Client>,
        game: String,
        name: String,
        games: Box<dyn Games>,
    ) {
        games.create_instance(client, game, name).await;
    }

    #[message]
    pub async fn join(&self, client: Arc<Client>, name: String, games: Box<dyn Games>) {
        games.join(client, name)
    }
}
