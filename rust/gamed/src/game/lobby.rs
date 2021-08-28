use crate::{client::Client, game_instances::GameInstances};
use gamed_macros::{message, state};

pub struct Lobby {}
impl crate::game::Game for Lobby {}

#[state(Lobby)]
impl Lobby {
    #[message]
    pub async fn games(&self, client: Client, games: &GameInstances) {
        client.send(games).await
    }

    #[message]
    pub async fn create(
        &self,
        client: Client,
        game: String,
        name: String,
        games: &mut GameInstances,
    ) {
        games.create_instance(client, game, name).await;
    }

    #[message]
    pub async fn join(&self, client: Client, name: String, games: &mut GameInstances) {
        games.join(client, name).await
    }
}
