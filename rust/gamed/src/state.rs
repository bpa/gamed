use async_trait::async_trait;

#[async_trait]
pub trait State {
    async fn on_enter(&mut self);
    async fn on_exit(&mut self);
}
