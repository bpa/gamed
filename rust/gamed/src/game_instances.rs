use serde::Serialize;

use crate::client::Client;

#[derive(Serialize)]
pub struct GameInstances {}

impl GameInstances {
    pub async fn create_instance(&mut self, _client: Client, _game: String, _name: String) {}

    pub async fn join(&mut self, _client: Client, _name: String) {}
}
