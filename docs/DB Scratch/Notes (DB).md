***a lot of places can use Hash maps instead of vectors***
***for each query schema is always available, table is available via query***

### 1. SARG matching

Rows on heap pages does not have any column name associated with them, hence planner creates a physical SARG which has column_type, operator and constant value.

*heap scan should scan only pages allocated for particular table, given via planner*

### 2. Inserting row correctly

Most important thing, a sql insert query can list columns in any order, construct a row which is a vector, first reserve size of that (through column size available in table_attr), compare order from query with order in schema catalog if column name match insert corresponding value into that vector, now get the base position where first column would be stored and then use offsets **(fixed)** to store next columns relative to this 

### 3. table_attr change

Table should get whole page, if full should get new page linked with previous one

*as currently this won't happen now focus on correctly inserting a row into heap page following schema*

### 4. local returns

a lot of things update on local variables introducing bugs, use references henceforth

### 5. file opening

while opening a file mostly open in ab+ mode as it creates a file if does not exists (which rb+ does not) and does not truncate the file (which wb+ does), and wipe file while testing phase.

### 6. Heap scan & Operator issue

Each operator calls next() for next entry.

heap scan on called should return next row (nothing else) and **check for end of page**, **skip deleted row**, **return nullopt only on error**.

seq_scan operator should call heap scan to get only **only one row**, should return **nullopt only when some error is encountered by heap scan**, if EOP received then should call heap scan only if new page exists for that table, else return nullopt.

filter does sarg matching, selecting the correct row based on predicate, if current row is not the one again ask seq_scan to give next row, do this until correct row found (internal loop), if not found at all return nullopt, terminates on single page scan later support multiple.

operators above filter can assume that if filters gives something it is valid.