use proc_macro::TokenStream;
#[macro_use]
extern crate lazy_static;
use quote::quote;
use syn::{parse_macro_input, DeriveInput, ItemFn};

use std::sync::Mutex;

lazy_static! {
    static ref GAMES: Mutex<Vec<String>> = Mutex::new(Vec::new());
}

#[proc_macro_attribute]
pub fn create(_attr: TokenStream, item: TokenStream) -> TokenStream {
    let output = item.clone();
    let input = parse_macro_input!(item as ItemFn);
    println!("create: {}", input.sig.ident);
    output
}

#[proc_macro_attribute]
pub fn message(_attr: TokenStream, item: TokenStream) -> TokenStream {
    let output = item.clone();
    let input = parse_macro_input!(item as ItemFn);
    println!("message: {}", input.sig.ident);
    output
}

#[proc_macro_derive(Game)]
pub fn derive_game(item: TokenStream) -> TokenStream {
    let input = parse_macro_input!(item as DeriveInput);
    let ident = input.ident;
    println!("Game: {}", ident);
    GAMES.lock().unwrap().push(ident.to_string());
    let qq = quote! {
        impl gamed::game::Game for #ident {}
    };
    qq.into()
}

#[proc_macro_attribute]
pub fn state(args: TokenStream, item: TokenStream) -> TokenStream {
    // let input = parse_macro_input!(item as ItemFn);
    println!("State {}", args);
    // println!("{:?}", input);
    item
}

#[proc_macro]
pub fn main(args: TokenStream) -> TokenStream {
    let input = parse_macro_input!(args as syn::Expr);
    let g = input.clone();
    let qq = quote! {
        #[tokio::main]
        async fn main() -> Result<(), std::io::Error> {
            let gamed = std::sync::Arc::new(gamed::Gamed::new(#g));

            let _res = tokio::join!(
                gamed::websocket(gamed.clone(), "127.0.0.1:3000"),
                gamed::tcp_client(gamed.clone(), "127.0.0.1:3001"),
                // gamed::tcp_admin(game.clone(), "127.0.0.1:3002"),
            );
            Ok(())
        }
    };
    qq.into()
}
