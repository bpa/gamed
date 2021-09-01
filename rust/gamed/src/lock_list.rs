use std::sync::Arc;
use tokio::sync::{Mutex, RwLock};

struct List<T> {
    free: Vec<usize>,
    items: Vec<Option<Arc<Mutex<T>>>>,
}
pub struct LockList<T> {
    list: RwLock<List<T>>,
}

impl<T> LockList<T> {
    pub fn new() -> Self {
        Self {
            list: RwLock::new(List::<T> {
                free: Vec::new(),
                items: Vec::new(),
            }),
        }
    }

    pub async fn get(&self, ind: usize) -> Option<Arc<Mutex<T>>> {
        let list = self.list.read().await;
        match &list.items[ind] {
            Some(item) => Some(item.clone()),
            None => None,
        }
    }

    pub async fn append(&self, item: T) {
        let mut list = self.list.write().await;
        match list.free.pop() {
            Some(ind) => {
                list.items[ind] = Some(Arc::new(Mutex::new(item)));
            }
            None => list.items.push(Some(Arc::new(Mutex::new(item)))),
        }
    }

    pub async fn remove(&self, ind: usize) {
        let mut list = self.list.write().await;
        match list.items[ind] {
            Some(_) => {
                list.free.push(ind);
                list.items[ind] = None;
            }
            None => {}
        }
    }
}
