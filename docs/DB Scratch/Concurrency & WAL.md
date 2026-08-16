ACID property stands for Atomicity, Consistency, Isolation, Durability.

Everything here tries to ensure above in a database.

**Transaction** : Multiple read, write, other operations can be considered as one *transaction*, this ensures **atomicity**. 

**Serializability** : If transactions are taking place concurrently, their internal operation can interleave and so can this produce same output as if both transaction had run one-after-other, if yes then it is Serializable, this ensures **consistency**.

Serializability can be implemented via *2PL, MVCC, OCC*:

- 2PL : Uses locks over data structures to commit transactions and releases atomically.
- MVCC : Does not use locks, but use state of database to ensure consistency.
- OCC : Does not use locks or states, ensures safety by rolling back operations causing harm.


