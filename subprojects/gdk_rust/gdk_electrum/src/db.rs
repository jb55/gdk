use std::collections::HashSet;
use std::ops::Drop;

use bitcoin::blockdata::transaction::OutPoint;

use serde_json::json;

use sled::{Batch, Db, Tree};

use crate::error::Error;
use crate::model::*;

pub trait GetTree {
    fn get_tree(&self, wallet_name: &str) -> Result<WalletDB, Error>;
}

impl GetTree for Db {
    fn get_tree(&self, wallet_name: &str) -> Result<WalletDB, Error> {
        println!("opening tree {}", wallet_name);
        Ok(WalletDB::from(self.open_tree(wallet_name)?))
    }
}

#[derive(Debug)]
pub struct WalletDB {
    tree: Tree,
}

impl From<Tree> for WalletDB {
    fn from(tree: Tree) -> Self {
        WalletDB {
            tree,
        }
    }
}

impl Drop for WalletDB {
    fn drop(&mut self) {
        self.tree.flush().unwrap();
    }
}

impl WalletDB {
    // TODO: we should have an higher-level api maybe
    pub fn apply_batch(&self, batch: Batch) -> Result<(), Error> {
        self.tree.apply_batch(batch)?;
        Ok(())
    }

    pub fn flush(&self) -> Result<usize, Error> {
        self.tree.flush().map_err(Into::into)
    }

    pub fn save_tx(&self, tx: WGTransaction, batch: &mut Batch) -> Result<(), Error> {
        let mut key = vec!['t' as u8];
        key.append(&mut hex::decode(&tx.txid)?);

        batch.insert(key, serde_json::to_vec(&tx)?);
        Ok(())
    }

    pub fn get_tx(&self, txid: &String) -> Result<Option<WGTransaction>, Error> {
        let mut key = vec!['t' as u8];
        key.append(&mut hex::decode(txid)?);

        Ok(self.tree.get(key)?.and_then(|data| serde_json::from_slice(&data).unwrap()))
    }

    pub fn save_spent(&self, outpoint: &OutPoint, batch: &mut Batch) -> Result<(), Error> {
        use bitcoin::consensus::serialize;

        let mut key = vec!['s' as u8];
        key.append(&mut serialize(outpoint));

        batch.insert(key, &[]);
        Ok(())
    }

    pub fn get_spent(&self) -> Result<HashSet<OutPoint>, Error> {
        use bitcoin::consensus::deserialize;

        let r = self.tree.scan_prefix(b"s");

        Ok(r.keys().map(|e| deserialize(&e.unwrap()[1..]).unwrap()).collect())
    }

    pub fn list_tx(&self) -> Result<Vec<WGTransaction>, Error> {
        let r = self.tree.scan_prefix(b"t");

        Ok(r.values().map(|e| serde_json::from_slice(&e.unwrap()).unwrap()).collect())
    }

    fn get_index(&self, key: &[u8]) -> Result<u32, Error> {
        let data = self.tree.get(key)?;

        match data {
            Some(bytes) => Ok(serde_json::from_slice(&bytes).unwrap()),
            None => Ok(0),
        }
    }

    fn get_internal_index(&self) -> Result<u32, Error> {
        self.get_index(b"i")
    }

    fn get_extenral_index(&self) -> Result<u32, Error> {
        self.get_index(b"e")
    }

    fn increment_index(&self, key: &[u8]) -> Result<u32, Error> {
        let data = self.tree.update_and_fetch(key, |old| {
            let num = match old {
                Some(bytes) => {
                    let val: u32 = serde_json::from_slice(bytes).unwrap();
                    println!("val is {}", val);
                    val + 1
                }
                None => 0,
            };

            Some(serde_json::to_vec(&json!(num)).unwrap())
        });

        Ok(serde_json::from_slice(&data?.unwrap()).unwrap())
    }

    pub fn increment_internal_index(&self) -> Result<u32, Error> {
        self.increment_index(b"i")
    }

    pub fn increment_external_index(&self) -> Result<u32, Error> {
        self.increment_index(b"e")
    }

    // TODO: only in debug
    pub fn dump(&self) -> Result<(), Error> {
        let r = self.tree.scan_prefix(&[]);
        for e in r {
            let e = e.unwrap();
            println!("{:?} {:?}", hex::encode(&e.0), std::str::from_utf8(&e.1));
        }

        Ok(())
    }
}