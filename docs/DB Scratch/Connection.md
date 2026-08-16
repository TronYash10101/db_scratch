###### There are 2 components, DB and TUI

Two functions `DB_Pipeline()` and `TUI_Pipeline()`:-

-  `DB_Pipeline()` receives single query at a time, gets ast internally and process particular ast accordingly.

-  `TUI_Pipeline()` handles output of different queries individually,
	-  Accordion reads from schema file for schema and table (not columns).
	- Table gets it input from answer of SELECT query.
	- Input Text Box acts as input to ` DB_Pipeline `.

 - Very specific request-response layout, object shared between DB and TUI looks like, 
	-  Object type (Request or Response)
	-  Variant holding Request/Response object.
	-  Request Object :- 
		-  Text Box input must be filled.
		-  *Nothing need most probably for now*.
	- Response Object :-
		- Text Box input must be empty.
		- New Table structure should be given.
		- New schema structure should be given for accordion (no reading from schema file).

- Changes in Structure,
	- Structure hold vector for each component, this avoids both dynamic casting (can cause issues) and iterating over all components for specific component.

bug found in db, without WHERE, SELECT does not work as it requires predicate